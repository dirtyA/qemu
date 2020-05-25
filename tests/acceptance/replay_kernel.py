# Record/replay test that boots a Linux kernel
#
# Copyright (c) 2020 ISP RAS
#
# Author:
#  Pavel Dovgalyuk <Pavel.Dovgaluk@ispras.ru>
#
# This work is licensed under the terms of the GNU GPL, version 2 or
# later.  See the COPYING file in the top-level directory.

import os
import gzip

from avocado_qemu import Test
from avocado_qemu import wait_for_console_pattern
from avocado.utils import process
from avocado.utils import archive

class ReplayKernel(Test):
    """
    Boots a Linux kernel in record mode and checks that the console
    is operational and the kernel command line is properly passed
    from QEMU to the kernel.
    Then replays the same scenario and verifies, that QEMU correctly
    terminates.
    """

    timeout = 90

    KERNEL_COMMON_COMMAND_LINE = 'printk.time=0 '

    def wait_for_console_pattern(self, success_message, vm):
        wait_for_console_pattern(self, success_message,
                                 failure_message='Kernel panic - not syncing',
                                 vm=vm)

    def extract_from_deb(self, deb, path):
        """
        Extracts a file from a deb package into the test workdir

        :param deb: path to the deb archive
        :param path: path within the deb archive of the file to be extracted
        :returns: path of the extracted file
        """
        cwd = os.getcwd()
        os.chdir(self.workdir)
        file_path = process.run("ar t %s" % deb).stdout_text.split()[2]
        process.run("ar x %s %s" % (deb, file_path))
        archive.extract(file_path, self.workdir)
        os.chdir(cwd)
        # Return complete path to extracted file.  Because callers to
        # extract_from_deb() specify 'path' with a leading slash, it is
        # necessary to use os.path.relpath() as otherwise os.path.join()
        # interprets it as an absolute path and drops the self.workdir part.
        return os.path.normpath(os.path.join(self.workdir,
                                             os.path.relpath(path, '/')))

    def run_vm(self, kernel_path, kernel_command_line, console_pattern, record, shift, args):
        vm = self.get_vm()
        vm.set_console()
        if record:
            mode = 'record'
        else:
            mode = 'replay'
        vm.add_args('-icount', 'shift=%s,rr=%s,rrfile=%s' %
                    (shift, mode, os.path.join(self.workdir, 'replay.bin')),
                    '-kernel', kernel_path,
                    '-append', kernel_command_line,
                    '-net', 'none',
                    *args)
        vm.launch()
        self.wait_for_console_pattern(console_pattern, vm)
        if record:
            vm.shutdown()
        else:
            vm.wait()

    def run_rr(self, kernel_path, kernel_command_line, console_pattern, shift=7, args=()):
        self.run_vm(kernel_path, kernel_command_line, console_pattern, True, shift, args)
        self.run_vm(kernel_path, kernel_command_line, console_pattern, False, shift, args)

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

        kernel_command_line = self.KERNEL_COMMON_COMMAND_LINE + 'console=ttyS0'
        console_pattern = 'Kernel command line: %s' % kernel_command_line

        self.run_rr(kernel_path, kernel_command_line, console_pattern)

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

        kernel_command_line = (self.KERNEL_COMMON_COMMAND_LINE +
                               'console=ttyAMA0')
        console_pattern = 'Kernel command line: %s' % kernel_command_line

        self.run_rr(kernel_path, kernel_command_line, console_pattern,
            args=('-cpu', 'cortex-a53'))

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

        kernel_command_line = (self.KERNEL_COMMON_COMMAND_LINE +
                               'console=ttyAMA0')
        console_pattern = 'Kernel command line: %s' % kernel_command_line

        self.run_rr(kernel_path, kernel_command_line, console_pattern)

    def test_arm_cubieboard_initrd(self):
        """
        :avocado: tags=arch:arm
        :avocado: tags=machine:cubieboard
        """
        deb_url = ('https://apt.armbian.com/pool/main/l/'
                   'linux-4.20.7-sunxi/linux-image-dev-sunxi_5.75_armhf.deb')
        deb_hash = '1334c29c44d984ffa05ed10de8c3361f33d78315'
        deb_path = self.fetch_asset(deb_url, asset_hash=deb_hash)
        kernel_path = self.extract_from_deb(deb_path,
                                            '/boot/vmlinuz-4.20.7-sunxi')
        dtb_path = '/usr/lib/linux-image-dev-sunxi/sun4i-a10-cubieboard.dtb'
        dtb_path = self.extract_from_deb(deb_path, dtb_path)
        initrd_url = ('https://github.com/groeck/linux-build-test/raw/'
                      '2eb0a73b5d5a28df3170c546ddaaa9757e1e0848/rootfs/'
                      'arm/rootfs-armv5.cpio.gz')
        initrd_hash = '2b50f1873e113523967806f4da2afe385462ff9b'
        initrd_path_gz = self.fetch_asset(initrd_url, asset_hash=initrd_hash)
        initrd_path = os.path.join(self.workdir, 'rootfs.cpio')
        archive.gzip_uncompress(initrd_path_gz, initrd_path)

        kernel_command_line = (self.KERNEL_COMMON_COMMAND_LINE +
                               'console=ttyS0,115200 '
                               'usbcore.nousb '
                               'panic=-1 noreboot')
        console_pattern = 'Boot successful.'
        self.run_rr(kernel_path, kernel_command_line, console_pattern,
            args=('-dtb', dtb_path, '-initrd', initrd_path, '-no-reboot'))

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

        kernel_command_line = self.KERNEL_COMMON_COMMAND_LINE + 'console=hvc0'
        console_pattern = 'Kernel command line: %s' % kernel_command_line
        self.run_rr(kernel_path, kernel_command_line, console_pattern)

    def test_m68k_q800(self):
        """
        :avocado: tags=arch:m68k
        :avocado: tags=machine:q800
        """
        deb_url = ('https://snapshot.debian.org/archive/debian-ports'
                   '/20191021T083923Z/pool-m68k/main'
                   '/l/linux/kernel-image-5.3.0-1-m68k-di_5.3.7-1_m68k.udeb')
        deb_hash = '044954bb9be4160a3ce81f8bc1b5e856b75cccd1'
        deb_path = self.fetch_asset(deb_url, asset_hash=deb_hash)
        kernel_path = self.extract_from_deb(deb_path,
                                            '/boot/vmlinux-5.3.0-1-m68k')

        kernel_command_line = (self.KERNEL_COMMON_COMMAND_LINE +
                               'console=ttyS0 vga=off')
        console_pattern = 'No filesystem could mount root'
        self.run_rr(kernel_path, kernel_command_line, console_pattern)

    def do_test_advcal_2018(self, day, tar_hash, kernel_name, args=()):
        tar_url = ('https://www.qemu-advent-calendar.org'
                   '/2018/download/day' + day + '.tar.xz')
        file_path = self.fetch_asset(tar_url, asset_hash=tar_hash)
        archive.extract(file_path, self.workdir)

        kernel_path = self.workdir + '/day' + day + '/' + kernel_name
        kernel_command_line = ''
        console_pattern = 'QEMU advent calendar'
        self.run_rr(kernel_path, kernel_command_line, console_pattern,
            args=args)

    def test_arm_vexpressa9(self):
        """
        :avocado: tags=arch:arm
        :avocado: tags=machine:vexpress-a9
        """
        tar_hash = '32b7677ce8b6f1471fb0059865f451169934245b'
        self.do_test_advcal_2018('16', tar_hash, 'winter.zImage',
            ('-dtb', self.workdir + '/day16/vexpress-v2p-ca9.dtb'))

    def test_m68k_mcf5208evb(self):
        """
        :avocado: tags=arch:m68k
        :avocado: tags=machine:mcf5208evb
        """
        tar_hash = 'ac688fd00561a2b6ce1359f9ff6aa2b98c9a570c'
        self.do_test_advcal_2018('07', tar_hash, 'sanity-clause.elf')

    def test_microblaze_s3adsp1800(self):
        """
        :avocado: tags=arch:microblaze
        :avocado: tags=machine:petalogix-s3adsp1800
        """
        tar_hash = '08bf3e3bfb6b6c7ce1e54ab65d54e189f2caf13f'
        self.do_test_advcal_2018('17', tar_hash, 'ballerina.bin')

    def test_ppc64_e500(self):
        """
        :avocado: tags=arch:ppc64
        :avocado: tags=machine:ppce500
        """
        tar_hash = '6951d86d644b302898da2fd701739c9406527fe1'
        self.do_test_advcal_2018('19', tar_hash, 'uImage', ('-cpu', 'e5500'))

    def test_ppc_g3beige(self):
        """
        :avocado: tags=arch:ppc
        :avocado: tags=machine:g3beige
        """
        tar_hash = 'e0b872a5eb8fdc5bed19bd43ffe863900ebcedfc'
        self.do_test_advcal_2018('15', tar_hash, 'invaders.elf',
            ('-M', 'graphics=off'))

    def test_ppc_mac99(self):
        """
        :avocado: tags=arch:ppc
        :avocado: tags=machine:mac99
        """
        tar_hash = 'e0b872a5eb8fdc5bed19bd43ffe863900ebcedfc'
        self.do_test_advcal_2018('15', tar_hash, 'invaders.elf',
            ('-M', 'graphics=off'))

    def test_sparc_ss20(self):
        """
        :avocado: tags=arch:sparc
        :avocado: tags=machine:SS-20
        """
        tar_hash = 'b18550d5d61c7615d989a06edace051017726a9f'
        self.do_test_advcal_2018('11', tar_hash, 'zImage.elf')

    def test_xtensa_lx60(self):
        """
        :avocado: tags=arch:xtensa
        :avocado: tags=machine:lx60
        """
        tar_hash = '49e88d9933742f0164b60839886c9739cb7a0d34'
        self.do_test_advcal_2018('02', tar_hash, 'santas-sleigh-ride.elf',
            ('-cpu', 'dc233c'))
