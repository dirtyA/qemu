# Functional test that boots a Linux kernel and checks the console
#
# Copyright (c) 2018 Red Hat, Inc.
#
# Author:
#  Cleber Rosa <crosa@redhat.com>
#
# This work is licensed under the terms of the GNU GPL, version 2 or
# later.  See the COPYING file in the top-level directory.

import os
import lzma
import gzip
import shutil

from avocado import skipUnless
from avocado_qemu import MachineTest
from avocado_qemu import exec_command_and_wait_for_pattern
from avocado_qemu import wait_for_console_pattern
from avocado.utils import process
from avocado.utils import archive


class BootLinuxConsole(MachineTest):
    """
    Boots a Linux kernel and checks that the console is operational and the
    kernel command line is properly passed from QEMU to the kernel
    """

    timeout = 90

    KERNEL_COMMON_COMMAND_LINE = 'printk.time=0 '

    def wait_for_console_pattern(self, success_message):
        wait_for_console_pattern(self, success_message,
                                 failure_message='Kernel panic - not syncing')

    def extract_from_deb(self, deb, path):
        """
        Extracts a file from a deb package into the test workdir

        :param deb: path to the deb archive
        :param file: path within the deb archive of the file to be extracted
        :returns: path of the extracted file
        """
        cwd = os.getcwd()
        os.chdir(self.workdir)
        file_path = process.run("ar t %s" % deb).stdout_text.split()[2]
        process.run("ar x %s %s" % (deb, file_path))
        archive.extract(file_path, self.workdir)
        os.chdir(cwd)
        return self.workdir + path

    def test_x86_64_pc(self):
        """
        :avocado: tags=arch:x86_64
        :avocado: tags=machine:pc
        """
        kernel_url = ('https://archives.fedoraproject.org/pub/archive/fedora'
                      '/linux/releases/29/Everything/x86_64/os/images/pxeboot'
                      '/vmlinuz')
        kernel_hash = '23bebd2680757891cf7adedb033532163a792495'
        kernel_path = self.fetch_asset(kernel_url, asset_hash=kernel_hash)

        self.vm.set_machine('pc')
        self.vm.set_console()
        kernel_command_line = self.KERNEL_COMMON_COMMAND_LINE + 'console=ttyS0'
        self.vm.add_args('-kernel', kernel_path,
                         '-append', kernel_command_line)
        self.vm.launch()
        console_pattern = 'Kernel command line: %s' % kernel_command_line
        self.wait_for_console_pattern(console_pattern)

    def test_mips_malta(self):
        """
        :avocado: tags=arch:mips
        :avocado: tags=machine:malta
        :avocado: tags=endian:big
        """
        deb_url = ('http://snapshot.debian.org/archive/debian/'
                   '20130217T032700Z/pool/main/l/linux-2.6/'
                   'linux-image-2.6.32-5-4kc-malta_2.6.32-48_mips.deb')
        deb_hash = 'a8cfc28ad8f45f54811fc6cf74fc43ffcfe0ba04'
        deb_path = self.fetch_asset(deb_url, asset_hash=deb_hash)
        kernel_path = self.extract_from_deb(deb_path,
                                            '/boot/vmlinux-2.6.32-5-4kc-malta')

        self.vm.set_machine('malta')
        self.vm.set_console()
        kernel_command_line = self.KERNEL_COMMON_COMMAND_LINE + 'console=ttyS0'
        self.vm.add_args('-kernel', kernel_path,
                         '-append', kernel_command_line)
        self.vm.launch()
        console_pattern = 'Kernel command line: %s' % kernel_command_line
        self.wait_for_console_pattern(console_pattern)

    def test_mips64el_malta(self):
        """
        This test requires the ar tool to extract "data.tar.gz" from
        the Debian package.

        The kernel can be rebuilt using this Debian kernel source [1] and
        following the instructions on [2].

        [1] http://snapshot.debian.org/package/linux-2.6/2.6.32-48/
            #linux-source-2.6.32_2.6.32-48
        [2] https://kernel-team.pages.debian.net/kernel-handbook/
            ch-common-tasks.html#s-common-official

        :avocado: tags=arch:mips64el
        :avocado: tags=machine:malta
        """
        deb_url = ('http://snapshot.debian.org/archive/debian/'
                   '20130217T032700Z/pool/main/l/linux-2.6/'
                   'linux-image-2.6.32-5-5kc-malta_2.6.32-48_mipsel.deb')
        deb_hash = '1aaec92083bf22fda31e0d27fa8d9a388e5fc3d5'
        deb_path = self.fetch_asset(deb_url, asset_hash=deb_hash)
        kernel_path = self.extract_from_deb(deb_path,
                                            '/boot/vmlinux-2.6.32-5-5kc-malta')

        self.vm.set_machine('malta')
        self.vm.set_console()
        kernel_command_line = self.KERNEL_COMMON_COMMAND_LINE + 'console=ttyS0'
        self.vm.add_args('-kernel', kernel_path,
                         '-append', kernel_command_line)
        self.vm.launch()
        console_pattern = 'Kernel command line: %s' % kernel_command_line
        self.wait_for_console_pattern(console_pattern)

    def test_mips_malta_cpio(self):
        """
        :avocado: tags=arch:mips
        :avocado: tags=machine:malta
        :avocado: tags=endian:big
        """
        deb_url = ('http://snapshot.debian.org/archive/debian/'
                   '20160601T041800Z/pool/main/l/linux/'
                   'linux-image-4.5.0-2-4kc-malta_4.5.5-1_mips.deb')
        deb_hash = 'a3c84f3e88b54e06107d65a410d1d1e8e0f340f8'
        deb_path = self.fetch_asset(deb_url, asset_hash=deb_hash)
        kernel_path = self.extract_from_deb(deb_path,
                                            '/boot/vmlinux-4.5.0-2-4kc-malta')
        initrd_url = ('https://github.com/groeck/linux-build-test/raw/'
                      '8584a59ed9e5eb5ee7ca91f6d74bbb06619205b8/rootfs/'
                      'mips/rootfs.cpio.gz')
        initrd_hash = 'bf806e17009360a866bf537f6de66590de349a99'
        initrd_path_gz = self.fetch_asset(initrd_url, asset_hash=initrd_hash)
        initrd_path = self.workdir + "rootfs.cpio"
        archive.gzip_uncompress(initrd_path_gz, initrd_path)

        self.vm.set_machine('malta')
        self.vm.set_console()
        kernel_command_line = (self.KERNEL_COMMON_COMMAND_LINE
                               + 'console=ttyS0 console=tty '
                               + 'rdinit=/sbin/init noreboot')
        self.vm.add_args('-kernel', kernel_path,
                         '-initrd', initrd_path,
                         '-append', kernel_command_line,
                         '-no-reboot')
        self.vm.launch()
        self.wait_for_console_pattern('Boot successful.')

        exec_command_and_wait_for_pattern(self, 'cat /proc/cpuinfo',
                                                'BogoMIPS')
        exec_command_and_wait_for_pattern(self, 'uname -a',
                                                'Debian')
        exec_command_and_wait_for_pattern(self, 'reboot',
                                                'reboot: Restarting system')

    def do_test_mips_malta32el_nanomips(self, kernel_url, kernel_hash):
        kernel_path_xz = self.fetch_asset(kernel_url, asset_hash=kernel_hash)
        kernel_path = self.workdir + "kernel"
        with lzma.open(kernel_path_xz, 'rb') as f_in:
            with open(kernel_path, 'wb') as f_out:
                shutil.copyfileobj(f_in, f_out)

        self.vm.set_machine('malta')
        self.vm.set_console()
        kernel_command_line = (self.KERNEL_COMMON_COMMAND_LINE
                               + 'mem=256m@@0x0 '
                               + 'console=ttyS0')
        self.vm.add_args('-no-reboot',
                         '-cpu', 'I7200',
                         '-kernel', kernel_path,
                         '-append', kernel_command_line)
        self.vm.launch()
        console_pattern = 'Kernel command line: %s' % kernel_command_line
        self.wait_for_console_pattern(console_pattern)

    def test_mips_malta32el_nanomips_4k(self):
        """
        :avocado: tags=arch:mipsel
        :avocado: tags=machine:malta
        :avocado: tags=endian:little
        """
        kernel_url = ('https://mipsdistros.mips.com/LinuxDistro/nanomips/'
                      'kernels/v4.15.18-432-gb2eb9a8b07a1-20180627102142/'
                      'generic_nano32r6el_page4k.xz')
        kernel_hash = '477456aafd2a0f1ddc9482727f20fe9575565dd6'
        self.do_test_mips_malta32el_nanomips(kernel_url, kernel_hash)

    def test_mips_malta32el_nanomips_16k_up(self):
        """
        :avocado: tags=arch:mipsel
        :avocado: tags=machine:malta
        :avocado: tags=endian:little
        """
        kernel_url = ('https://mipsdistros.mips.com/LinuxDistro/nanomips/'
                      'kernels/v4.15.18-432-gb2eb9a8b07a1-20180627102142/'
                      'generic_nano32r6el_page16k_up.xz')
        kernel_hash = 'e882868f944c71c816e832e2303b7874d044a7bc'
        self.do_test_mips_malta32el_nanomips(kernel_url, kernel_hash)

    def test_mips_malta32el_nanomips_64k_dbg(self):
        """
        :avocado: tags=arch:mipsel
        :avocado: tags=machine:malta
        :avocado: tags=endian:little
        """
        kernel_url = ('https://mipsdistros.mips.com/LinuxDistro/nanomips/'
                      'kernels/v4.15.18-432-gb2eb9a8b07a1-20180627102142/'
                      'generic_nano32r6el_page64k_dbg.xz')
        kernel_hash = '18d1c68f2e23429e266ca39ba5349ccd0aeb7180'
        self.do_test_mips_malta32el_nanomips(kernel_url, kernel_hash)

    def test_aarch64_virt(self):
        """
        :avocado: tags=arch:aarch64
        :avocado: tags=machine:virt
        """
        kernel_url = ('https://archives.fedoraproject.org/pub/archive/fedora'
                      '/linux/releases/29/Everything/aarch64/os/images/pxeboot'
                      '/vmlinuz')
        kernel_hash = '8c73e469fc6ea06a58dc83a628fc695b693b8493'
        kernel_path = self.fetch_asset(kernel_url, asset_hash=kernel_hash)

        self.vm.set_machine('virt')
        self.vm.set_console()
        kernel_command_line = (self.KERNEL_COMMON_COMMAND_LINE +
                               'console=ttyAMA0')
        self.vm.add_args('-cpu', 'cortex-a53',
                         '-kernel', kernel_path,
                         '-append', kernel_command_line)
        self.vm.launch()
        console_pattern = 'Kernel command line: %s' % kernel_command_line
        self.wait_for_console_pattern(console_pattern)

    def test_arm_virt(self):
        """
        :avocado: tags=arch:arm
        :avocado: tags=machine:virt
        """
        kernel_url = ('https://archives.fedoraproject.org/pub/archive/fedora'
                      '/linux/releases/29/Everything/armhfp/os/images/pxeboot'
                      '/vmlinuz')
        kernel_hash = 'e9826d741b4fb04cadba8d4824d1ed3b7fb8b4d4'
        kernel_path = self.fetch_asset(kernel_url, asset_hash=kernel_hash)

        self.vm.set_machine('virt')
        self.vm.set_console()
        kernel_command_line = (self.KERNEL_COMMON_COMMAND_LINE +
                               'console=ttyAMA0')
        self.vm.add_args('-kernel', kernel_path,
                         '-append', kernel_command_line)
        self.vm.launch()
        console_pattern = 'Kernel command line: %s' % kernel_command_line
        self.wait_for_console_pattern(console_pattern)

    def test_arm_emcraft_sf2(self):
        """
        :avocado: tags=arch:arm
        :avocado: tags=machine:emcraft_sf2
        :avocado: tags=endian:little
        """
        uboot_url = ('https://raw.githubusercontent.com/'
                     'Subbaraya-Sundeep/qemu-test-binaries/'
                     'fa030bd77a014a0b8e360d3b7011df89283a2f0b/u-boot')
        uboot_hash = 'abba5d9c24cdd2d49cdc2a8aa92976cf20737eff'
        uboot_path = self.fetch_asset(uboot_url, asset_hash=uboot_hash)
        spi_url = ('https://raw.githubusercontent.com/'
                   'Subbaraya-Sundeep/qemu-test-binaries/'
                   'fa030bd77a014a0b8e360d3b7011df89283a2f0b/spi.bin')
        spi_hash = '85f698329d38de63aea6e884a86fbde70890a78a'
        spi_path = self.fetch_asset(spi_url, asset_hash=spi_hash)

        self.vm.set_machine('emcraft-sf2')
        self.vm.set_console()
        kernel_command_line = self.KERNEL_COMMON_COMMAND_LINE
        self.vm.add_args('-kernel', uboot_path,
                         '-append', kernel_command_line,
                         '-drive', 'file=' + spi_path + ',if=mtd,format=raw',
                         '-no-reboot')
        self.vm.launch()
        self.wait_for_console_pattern('init started: BusyBox')

    def do_test_arm_raspi2(self, uart_id):
        """
        The kernel can be rebuilt using the kernel source referenced
        and following the instructions on the on:
        https://www.raspberrypi.org/documentation/linux/kernel/building.md
        """
        serial_kernel_cmdline = {
            0: 'earlycon=pl011,0x3f201000 console=ttyAMA0',
            1: 'earlycon=uart8250,mmio32,0x3f215040 console=ttyS1,115200'
        }
        deb_url = ('http://archive.raspberrypi.org/debian/'
                   'pool/main/r/raspberrypi-firmware/'
                   'raspberrypi-kernel_1.20190215-1_armhf.deb')
        deb_hash = 'cd284220b32128c5084037553db3c482426f3972'
        deb_path = self.fetch_asset(deb_url, asset_hash=deb_hash)
        kernel_path = self.extract_from_deb(deb_path, '/boot/kernel7.img')
        dtb_path = self.extract_from_deb(deb_path, '/boot/bcm2709-rpi-2-b.dtb')

        self.vm.set_machine('raspi2')
        self.vm.set_console(console_index=uart_id)
        kernel_command_line = (self.KERNEL_COMMON_COMMAND_LINE +
                               serial_kernel_cmdline[uart_id])
        self.vm.add_args('-kernel', kernel_path,
                         '-dtb', dtb_path,
                         '-append', kernel_command_line)
        self.vm.launch()
        console_pattern = 'Kernel command line: %s' % kernel_command_line
        self.wait_for_console_pattern(console_pattern)

    def test_arm_raspi2_uart0(self):
        """
        :avocado: tags=arch:arm
        :avocado: tags=machine:raspi2
        :avocado: tags=device:pl011
        """
        self.do_test_arm_raspi2(0)

    def test_arm_raspi2_uart1(self):
        """
        :avocado: tags=arch:arm
        :avocado: tags=machine:raspi2
        :avocado: tags=device:bcm2835_aux
        """
        self.do_test_arm_raspi2(1)

    def test_arm_raspi3_initrd_uart0(self):
        """
        :avocado: tags=arch:aarch64
        :avocado: tags=machine:raspi3
        """
        deb_url = ('https://snapshot.debian.org/archive/debian/'
                   '20180106T174014Z/pool/main/l/linux/'
                   'linux-image-4.14.0-3-arm64_4.14.12-2_arm64.deb')
        deb_hash = 'e71c995de57921921895c1162baea5df527d1c6b'
        deb_path = self.fetch_asset(deb_url, asset_hash=deb_hash)
        kernel_path = self.extract_from_deb(deb_path,
                                            '/boot/vmlinuz-4.14.0-3-arm64')

        dtb_url = ('https://github.com/raspberrypi/firmware/raw/'
                   '1.20180313/boot/bcm2710-rpi-3-b.dtb')
        dtb_hash = 'eb14d67133401ef2f93523be7341456d38bfc077'
        dtb_path = self.fetch_asset(dtb_url, asset_hash=dtb_hash)

        initrd_url = ('https://github.com/groeck/linux-build-test/raw/'
                      '9b6b392ea7bc15f0d6632328d429d31c9c6273da/rootfs/'
                      'arm64/rootfs.cpio.gz')
        initrd_hash = '6fd05324f17bb950196b5ad9d3a0fa996339f4cd'
        initrd_path_gz = self.fetch_asset(initrd_url, asset_hash=initrd_hash)
        initrd_path = self.workdir + "rootfs.cpio"
        archive.gzip_uncompress(initrd_path_gz, initrd_path)

        self.vm.set_machine('raspi3')
        self.vm.set_console()
        kernel_command_line = (self.KERNEL_COMMON_COMMAND_LINE +
                               'earlycon=pl011,0x3f201000 console=ttyAMA0 ' +
                               'panic=-1 noreboot')
        self.vm.add_args('-kernel', kernel_path,
                         '-dtb', dtb_path,
                         '-initrd', initrd_path,
                         '-append', kernel_command_line,
                         '-no-reboot')
        self.vm.launch()

        self.wait_for_console_pattern('Boot successful.',
                                      failure_message='-----[ cut here ]-----')

        exec_command_and_wait_for_pattern(self, 'cat /proc/cpuinfo',
                                                'BogoMIPS')
        exec_command_and_wait_for_pattern(self, 'uname -a',
                                                'Debian')
        exec_command_and_wait_for_pattern(self, 'reboot',
                                                'reboot: Restarting system')

    def test_arm_raspi3_initrd_sd_temp(self):
        """
        :avocado: tags=arch:aarch64
        :avocado: tags=machine:raspi3
        """
        tarball_url = ('https://github.com/sakaki-/bcmrpi3-kernel/releases/'
                       'download/4.19.71.20190910/'
                       'bcmrpi3-kernel-4.19.71.20190910.tar.xz')
        tarball_hash = '844f117a1a6de0532ba92d2a7bceb5b2acfbb298'
        tarball_path = self.fetch_asset(tarball_url, asset_hash=tarball_hash)
        archive.extract(tarball_path, self.workdir)
        dtb_path    = self.workdir + "/boot/bcm2837-rpi-3-b.dtb"
        kernel_path = self.workdir + "/boot/kernel8.img"

        rootfs_url = ('https://github.com/groeck/linux-build-test/raw/'
                      '9b6b392ea7bc15f0d6632328d429d31c9c6273da/rootfs/'
                      'arm64/rootfs.ext2.gz')
        rootfs_hash = 'dbe4136f0b4a0d2180b93fd2a3b9a784f9951d10'
        rootfs_path_gz = self.fetch_asset(rootfs_url, asset_hash=rootfs_hash)
        rootfs_path = self.workdir + "rootfs.ext2"
        archive.gzip_uncompress(rootfs_path_gz, rootfs_path)

        self.vm.set_machine('raspi3')
        self.vm.set_console()
        kernel_command_line = (self.KERNEL_COMMON_COMMAND_LINE +
                               'earlycon=pl011,0x3f201000 console=ttyAMA0 ' +
                               'root=/dev/mmcblk0 rootwait rw ' +
                               'panic=-1 noreboot')
        self.vm.add_args('-kernel', kernel_path,
                         '-dtb', dtb_path,
                         '-append', kernel_command_line,
                         '-drive', 'file=' + rootfs_path + ',if=sd,format=raw',
                         '-no-reboot')
        self.vm.launch()

        self.wait_for_console_pattern('Boot successful.',
                                      failure_message='-----[ cut here ]-----')

        exec_command_and_wait_for_pattern(self, 'cat /proc/cpuinfo',
                                                'Raspberry Pi 3 Model B')
        exec_command_and_wait_for_pattern(self, ('cat /sys/devices/virtual/'
                                                 'thermal/thermal_zone0/temp'),
                                                '25178')
        exec_command_and_wait_for_pattern(self, 'reboot',
                                                'reboot: Restarting system')

    def test_s390x_s390_ccw_virtio(self):
        """
        :avocado: tags=arch:s390x
        :avocado: tags=machine:s390_ccw_virtio
        """
        kernel_url = ('https://archives.fedoraproject.org/pub/archive'
                      '/fedora-secondary/releases/29/Everything/s390x/os/images'
                      '/kernel.img')
        kernel_hash = 'e8e8439103ef8053418ef062644ffd46a7919313'
        kernel_path = self.fetch_asset(kernel_url, asset_hash=kernel_hash)

        self.vm.set_machine('s390-ccw-virtio')
        self.vm.set_console()
        kernel_command_line = self.KERNEL_COMMON_COMMAND_LINE + 'console=sclp0'
        self.vm.add_args('-nodefaults',
                         '-kernel', kernel_path,
                         '-append', kernel_command_line)
        self.vm.launch()
        console_pattern = 'Kernel command line: %s' % kernel_command_line
        self.wait_for_console_pattern(console_pattern)

    def test_alpha_clipper(self):
        """
        :avocado: tags=arch:alpha
        :avocado: tags=machine:clipper
        """
        kernel_url = ('http://archive.debian.org/debian/dists/lenny/main/'
                      'installer-alpha/current/images/cdrom/vmlinuz')
        kernel_hash = '3a943149335529e2ed3e74d0d787b85fb5671ba3'
        kernel_path = self.fetch_asset(kernel_url, asset_hash=kernel_hash)

        uncompressed_kernel = archive.uncompress(kernel_path, self.workdir)

        self.vm.set_machine('clipper')
        self.vm.set_console()
        kernel_command_line = self.KERNEL_COMMON_COMMAND_LINE + 'console=ttyS0'
        self.vm.add_args('-vga', 'std',
                         '-kernel', uncompressed_kernel,
                         '-append', kernel_command_line)
        self.vm.launch()
        console_pattern = 'Kernel command line: %s' % kernel_command_line
        self.wait_for_console_pattern(console_pattern)

    def test_ppc64_pseries(self):
        """
        :avocado: tags=arch:ppc64
        :avocado: tags=machine:pseries
        """
        kernel_url = ('https://archives.fedoraproject.org/pub/archive'
                      '/fedora-secondary/releases/29/Everything/ppc64le/os'
                      '/ppc/ppc64/vmlinuz')
        kernel_hash = '3fe04abfc852b66653b8c3c897a59a689270bc77'
        kernel_path = self.fetch_asset(kernel_url, asset_hash=kernel_hash)

        self.vm.set_machine('pseries')
        self.vm.set_console()
        kernel_command_line = self.KERNEL_COMMON_COMMAND_LINE + 'console=hvc0'
        self.vm.add_args('-kernel', kernel_path,
                         '-append', kernel_command_line)
        self.vm.launch()
        console_pattern = 'Kernel command line: %s' % kernel_command_line
        self.wait_for_console_pattern(console_pattern)

    @skipUnless(os.getenv('AVOCADO_ALLOW_UNTRUSTED_CODE'), 'untrusted code')
    def test_hppa_fwupdate(self):
        """
        :avocado: tags=arch:hppa
        :avocado: tags=device:lsi53c895a
        """
        cdrom_url = ('https://github.com/philmd/qemu-testing-blob/raw/ec1b741/'
                     'hppa/hp9000/712/C7120023.frm')
        cdrom_hash = '17944dee46f768791953009bcda551be5ab9fac9'
        cdrom_path = self.fetch_asset(cdrom_url, asset_hash=cdrom_hash)

        self.vm.set_console()
        self.vm.add_args('-cdrom', cdrom_path,
                         '-boot', 'd',
                         '-no-reboot')
        self.vm.launch()
        self.wait_for_console_pattern('Unrecognized MODEL TYPE = 502')

        exec_command_and_wait_for_pattern(self, 'exit',
                                                'UPDATE>')
        exec_command_and_wait_for_pattern(self, 'ls',
                                                'IMAGE1B')
        exec_command_and_wait_for_pattern(self, 'exit',
                                                'THIS UTILITY WILL NOW '
                                                'RESET THE SYSTEM.....')
