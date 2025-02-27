BPN = "qemu-xilinx"

require qemu-system-native-alt.inc
require qemu-xilinx-native-8.1.inc

# As some of the files installed by qemu-native and qemu-system-native
# are the same, we depend on qemu-native to get the full installation set
# and avoid file clashes
DEPENDS += "glib-2.0-native zlib-native pixman-native qemu-native"

DEPENDS += "qemu-xilinx-multiarch-helper-native bootgen-native"

EXTRA_OECONF:append = " --target-list=${@get_qemu_system_target_list(d)}"

PACKAGECONFIG ??= "fdt alsa kvm pie slirp png gcrypt \
    ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'virglrenderer epoxy', '', d)} \
"

# Handle distros such as CentOS 5 32-bit that do not have kvm support
PACKAGECONFIG:remove = "${@'kvm' if not os.path.exists('/usr/include/linux/kvm.h') else ''}"

do_install:append() {
    install -Dm 0755 ${WORKDIR}/powerpc_rom.bin ${D}${datadir}/qemu-xilinx

    # The following is also installed by qemu-native
    rm -f ${D}${datadir}/qemu-xilinx/trace-events-all
    rm -rf ${D}${datadir}/qemu-xilinx/keymaps
    rm -rf ${D}${datadir}/icons/
    rm -rf ${D}${includedir}/qemu-plugin.h

    # Install qmp.py to be used with testimage
    install -d ${D}${libdir}/qemu-python/qmp/
    install -D ${S}/python/qemu/qmp/* ${D}${libdir}/qemu-python/qmp/
}
