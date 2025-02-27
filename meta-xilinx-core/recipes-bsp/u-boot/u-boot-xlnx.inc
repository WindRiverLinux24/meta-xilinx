require recipes-bsp/u-boot/u-boot-common.inc
require recipes-bsp/u-boot/u-boot.inc

DEPENDS += "bc-native dtc-native python3-setuptools-native bison-native xxd-native"

require u-boot-xlnx-common.inc

SYSROOT_DIRS += "/boot"

BASE_DTS ?= "${@os.path.splitext(os.path.basename(d.getVar('CONFIG_DTFILE') or ''))[0] or 'system-top'}"
DTB_PATH ?= "boot/devicetree/"
DTB_FILE_NAME ?= ""

# This vairable is used for U-boot user specified dts from u-boot repo path
# u-boot-xlnx/arch/arm/dts/versal-vck190-revA-x-ebm-01-revA.dts.
# Note: .dts extension is not required for this variable settings.
# Example: UBOOT_USER_SPECIFIED_DTS = "versal-vck190-revA-x-ebm-01-revA"
UBOOT_USER_SPECIFIED_DTS ?= ""

EXTRA_OEMAKE += "${@'EXT_DTB=${RECIPE_SYSROOT}/${DTB_PATH}/${DTB_FILE_NAME}' if (d.getVar('DTB_FILE_NAME') != '' and d.getVar('UBOOT_USER_SPECIFIED_DTS') == '') else '' }"
EXTRA_OEMAKE += "${@'DEVICE_TREE=${UBOOT_USER_SPECIFIED_DTS}' if (d.getVar('UBOOT_USER_SPECIFIED_DTS') != '') else '' }"

python __anonymous () {
    #check if there are any dtb providers
    providerdtb = d.getVar("PREFERRED_PROVIDER_virtual/dtb")
    if providerdtb:
       d.appendVarFlag('do_configure', 'depends', ' virtual/dtb:do_populate_sysroot')
       if d.getVar("DTB_FILE_NAME") is not None:
           d.setVar('DTB_FILE_NAME', d.getVar('BASE_DTS')+ '.dtb')
}

do_configure:prepend () {
    if [ -n "${UBOOT_USER_SPECIFIED_DTS}" && ! -f ${S}/arch/arm/dts/${UBOOT_USER_SPECIFIED_DTS}.dts ]; then
        bbfatal "Uboot user specified dts (${UBOOT_USER_SPECIFIED_DTS}.dts) is not found in \
the${S}/arch/arm/dts directory, you need to patch dts file to u-boot source and use this configuration. \
For more details refer https://u-boot.readthedocs.io/en/latest/develop/devicetree/control.html#configuration "
    fi
}

# Split u-boot into .bin and .elf packages
# If the user just installs u-boot they get both
PACKAGES =+ "${PN}-bin ${PN}-elf"
RDEPENDS:${PN} = "${PN}-bin ${PN}-elf"
ALLOW_EMPTY:${PN} = "1"

SUMMARY:${PN}-bin = "${SUMMARY} - files in .bin or .img format"
FILES:${PN}-bin = "/boot/u-boot*.${UBOOT_SUFFIX}"

SUMMARY:${PN}-elf = "${SUMMARY} - files in .elf format"
FILES:${PN}-elf = "/boot/u-boot*.elf"

require u-boot-xlnx-blob.inc

UBOOTELF_NODTB_IMAGE ?= "u-boot-nodtb.elf"
UBOOTELF_NODTB_BINARY ?= "u-boot"
do_deploy:prepend() {
    cd ${B}

    if [ -f "${UBOOTELF_NODTB_BINARY}" ]; then
            install ${UBOOTELF_NODTB_BINARY} ${DEPLOYDIR}/${UBOOTELF_NODTB_IMAGE}
    fi

    #following lines are from uboot-sign.bbclass, vars are defined there
    if [ -e "${UBOOT_DTB_BINARY}" ]; then
            if [ "${UBOOT_SIGN_ENABLE}" = "0" ]; then
                    install ${UBOOT_DTB_BINARY} ${DEPLOYDIR}/${UBOOT_DTB_IMAGE}
            fi
            ln -sf ${UBOOT_DTB_IMAGE} ${DEPLOYDIR}/${UBOOT_DTB_BINARY}
            ln -sf ${UBOOT_DTB_IMAGE} ${DEPLOYDIR}/${UBOOT_DTB_SYMLINK}
    fi
    if [ -f "${UBOOT_NODTB_BINARY}" ]; then
            install ${UBOOT_NODTB_BINARY} ${DEPLOYDIR}/${UBOOT_NODTB_IMAGE}
            ln -sf ${UBOOT_NODTB_IMAGE} ${DEPLOYDIR}/${UBOOT_NODTB_SYMLINK}
            ln -sf ${UBOOT_NODTB_IMAGE} ${DEPLOYDIR}/${UBOOT_NODTB_BINARY}
    fi

    # In ZynqMP u-boot.dtb is generated by default but not for versal, Hence manually deploy.
    if [ "${SOC_FAMILY}" == "versal" -o "${SOC_FAMILY}" == "versal-net" ]; then
        if [ -f ${B}/arch/arm/dts/${UBOOT_USER_SPECIFIED_DTS}.dtb ]; then
            install -Dm 0644 ${B}/arch/arm/dts/${UBOOT_USER_SPECIFIED_DTS}.dtb ${DEPLOYDIR}/u-boot.dtb
        fi
    fi
}
