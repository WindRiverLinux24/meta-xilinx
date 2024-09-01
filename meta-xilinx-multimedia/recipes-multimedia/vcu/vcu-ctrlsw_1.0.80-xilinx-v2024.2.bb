SUMMARY = "Control Software for VCU"
DESCRIPTION = "Control software libraries, test applications and headers provider for VCU encoder/decoded software API"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE.md;md5=002a0a92906100955ea6ed02dcd2c2cd"

# Recipe has been renamed
PROVIDES += "libvcu-xlnx"

PV .= "+git"

BRANCH ?= "master"
REPO   ?= "git://github.com/Xilinx/vcu-ctrl-sw.git;protocol=https"
SRCREV = "1485f8fe481569131d9566a0dda2ec1fb6265f08"

BRANCHARG = "${@['nobranch=1', 'branch=${BRANCH}'][d.getVar('BRANCH', True) != '']}"
SRC_URI = "${REPO};${BRANCHARG}"

S  = "${WORKDIR}/git"

inherit features_check

REQUIRED_MACHINE_FEATURES = "vcu"

PACKAGE_ARCH = "${MACHINE_ARCH}"

RDEPENDS:${PN} = "kernel-module-vcu"
RDEPENDS:libvcu-ctrlsw = "kernel-module-vcu"

EXTRA_OEMAKE = "CC='${CC}' CXX='${CXX} ${CXXFLAGS}'"

do_install() {
    install -d ${D}${libdir}
    install -d ${D}${includedir}/vcu-ctrl-sw/include

    oe_runmake install_headers INSTALL_HDR_PATH=${D}${includedir}/vcu-ctrl-sw/include INSTALL_PATH=${D}/${bindir}
    oe_libinstall -C ${S}/bin/ -so liballegro_decode ${D}/${libdir}/
    oe_libinstall -C ${S}/bin/ -so liballegro_encode ${D}/${libdir}/
}

PACKAGES =+ "libvcu-ctrlsw"
FILES:libvcu-ctrlsw += "${libdir}/liballegro*.so.*"

# These libraries shouldn't get installed in world builds unless something
# explicitly depends upon them.

EXCLUDE_FROM_WORLD = "1"

# Disable buildpaths QA check warnings.
INSANE_SKIP:${PN} += "buildpaths"
