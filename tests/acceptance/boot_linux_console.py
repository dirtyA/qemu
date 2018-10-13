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
import logging
import subprocess

from avocado import skip
from avocado_qemu import Test
from avocado.utils.wait import wait_for


def read_stream_for_string(stream, expected_string, logger=None):
    msg = stream.readline()
    if len(msg) == 0:
        return False
    if logger:
        logger.debug(msg.strip())
    return expected_string in msg


class BootLinuxConsole(Test):
    """
    Boots a Linux kernel and checks that the console is operational
    and the kernel command line is properly passed from QEMU to the kernel

    :avocado: enable
    """

    timeout = 60

    def test_x86_64_pc(self):
        if self.arch != 'x86_64':
            self.cancel('Currently specific to the x86_64 target arch')
        kernel_url = ('https://mirrors.kernel.org/fedora/releases/28/'
                      'Everything/x86_64/os/images/pxeboot/vmlinuz')
        kernel_hash = '238e083e114c48200f80d889f7e32eeb2793e02a'
        kernel_path = self.fetch_asset(kernel_url, asset_hash=kernel_hash)

        self.vm.set_arch(self.arch)
        self.vm.set_machine()
        self.vm.set_console()
        kernel_command_line = 'console=ttyS0'
        self.vm.add_args('-kernel', kernel_path,
                         '-append', kernel_command_line)
        self.vm.launch()
        console = self.vm.console_socket.makefile()
        console_logger = logging.getLogger('console')
        while True:
            msg = console.readline()
            console_logger.debug(msg.strip())
            if 'Kernel command line: %s' % kernel_command_line in msg:
                break
            if 'Kernel panic - not syncing' in msg:
                self.fail("Kernel panic reached")

    def test_mips_4kc_malta(self):
        """
        This test requires the dpkg-deb tool (apt/dnf install dpkg) to extract
        the kernel from the Debian package.

        The kernel can be rebuilt using this Debian kernel source [1] and
        following the instructions on [2].

        [1] https://kernel-team.pages.debian.net/kernel-handbook/ch-common-tasks.html#s-common-official
        [2] http://snapshot.debian.org/package/linux-2.6/2.6.32-48/#linux-source-2.6.32_2.6.32-48

        :avocado: tags=arch:mips
        """
        if self.arch != 'mips': # FIXME use 'arch' tag in parent class?
            self.cancel('Currently specific to the %s target arch' % self.arch)

        deb_url = ('http://snapshot.debian.org/archive/debian/20130217T032700Z/'
                   'pool/main/l/linux-2.6/'
                   'linux-image-2.6.32-5-4kc-malta_2.6.32-48_mips.deb')
        deb_hash = 'a8cfc28ad8f45f54811fc6cf74fc43ffcfe0ba04'
        deb_path = self.fetch_asset(deb_url, asset_hash=deb_hash)
        subprocess.check_call(['dpkg-deb', '--extract', deb_path, self.workdir]) # FIXME move to avocado ...
        kernel_path = self.workdir + '/boot/vmlinux-2.6.32-5-4kc-malta'          # FIXME ... and use from assets?

        self.vm.set_arch(self.arch)
        self.vm.set_machine('malta')
        self.vm.set_console("") # XXX this disable isa-serial to use -serial ...
        kernel_command_line = 'console=ttyS0 printk.time=0'
        self.vm.add_args('-m', "64",
                         '-serial', "chardev:console", # XXX ... here.
                         '-kernel', kernel_path,
                         '-append', kernel_command_line)

        # FIXME below to parent class?
        self.vm.launch()
        console = self.vm.console_socket.makefile()
        console_logger = logging.getLogger('console')
        while True:
            msg = console.readline()
            console_logger.debug(msg.strip())
            if 'Kernel command line: %s' % kernel_command_line in msg:
                break
            if 'Kernel panic - not syncing' in msg:
                self.fail("Kernel panic reached")

    def test_mipsel_5kc_malta(self):
        """
        This test requires the dpkg-deb tool (apt/dnf install dpkg) to extract
        the kernel from the Debian package.

        The kernel can be rebuilt using this Debian kernel source [1] and
        following the instructions on [2].

        [1] https://kernel-team.pages.debian.net/kernel-handbook/ch-common-tasks.html#s-common-official
        [2] http://snapshot.debian.org/package/linux-2.6/2.6.32-48/#linux-source-2.6.32_2.6.32-48

        :avocado: tags=arch:mips64el
        """
        if self.arch != 'mips64el':
            self.cancel('Currently specific to the %s target arch' % self.arch)

        deb_url = ('http://snapshot.debian.org/archive/debian/20130217T032700Z/'
                   'pool/main/l/linux-2.6/'
                   'linux-image-2.6.32-5-5kc-malta_2.6.32-48_mipsel.deb')
        deb_hash = '1aaec92083bf22fda31e0d27fa8d9a388e5fc3d5'
        deb_path = self.fetch_asset(deb_url, asset_hash=deb_hash)
        subprocess.check_call(['dpkg-deb', '--extract', deb_path, self.workdir])
        kernel_path = self.workdir + '/boot/vmlinux-2.6.32-5-5kc-malta'

        self.vm.set_arch(self.arch)
        self.vm.set_machine('malta')
        self.vm.set_console("") # XXX
        kernel_command_line = 'console=ttyS0 printk.time=0'
        self.vm.add_args('-m', "64",
                         '-serial', "chardev:console",
                         '-kernel', kernel_path,
                         '-append', kernel_command_line)

        self.vm.launch()
        console = self.vm.console_socket.makefile()
        console_logger = logging.getLogger('console')
        while True:
            msg = console.readline()
            console_logger.debug(msg.strip())
            if 'Kernel command line: %s' % kernel_command_line in msg:
                break
            if 'Kernel panic - not syncing' in msg:
                self.fail("Kernel panic reached")

    @skip("console not working on r2d machine")
    def test_sh4_r2d(self):
        """
        This test requires the dpkg-deb tool (apt/dnf install dpkg) to extract
        the kernel from the Debian package.
        This test also requires the QEMU binary to be compiled with:

          $ configure ... --enable-trace-backends=log

        The kernel can be rebuilt using this Debian kernel source [1] and
        following the instructions on [2].

        [1] https://kernel-team.pages.debian.net/kernel-handbook/ch-common-tasks.html#s-common-official
        [2] http://snapshot.debian.org/package/linux-2.6/2.6.32-30/#linux-source-2.6.32_2.6.32-30

        :avocado: tags=arch:sh4
        """
        if self.arch != 'sh4':
            self.cancel('Currently specific to the %s target arch' % self.arch)

        deb_url = ('http://snapshot.debian.org/archive/'
                   'debian-ports/20110116T065852Z/pool-sh4/main/l/'
                   'linux-2.6/linux-image-2.6.32-5-sh7751r_2.6.32-30_sh4.deb')
        deb_hash = '8025e503319dc8ad786756e3afaa8eb868e9ef59'
        deb_path = self.fetch_asset(deb_url, asset_hash=deb_hash)
        subprocess.check_call(['dpkg-deb', '--extract', deb_path, self.workdir])
        kernel_path = self.workdir + '/boot/vmlinuz-2.6.32-5-sh7751r'

        self.vm.set_arch(self.arch)
        self.vm.set_machine('r2d')
        self.vm.set_console("") # XXX
        kernel_command_line = 'console=ttyS0 printk.time=0 noiotrap'
        self.vm.add_args('-serial', "chardev:console",
                         '-kernel', kernel_path,
                         '-append', kernel_command_line)

        self.vm.launch()
        console = self.vm.console_socket.makefile()
        console_logger = logging.getLogger('console')
        while True:
            msg = console.readline()
            console_logger.debug(msg.strip())
            if 'Kernel command line: %s' % kernel_command_line in msg:
                break
            if 'Kernel panic - not syncing' in msg:
                self.fail("Kernel panic reached")

class BootLinuxTracing(Test):
    """
    Boots a Linux kernel and checks that via the Tracing framework that
    a specific trace events occured, demostrating the kernel is operational.

    :avocado: enable
    """

    timeout = 60

    def test_sh4_r2d(self):
        """
        This test requires the dpkg-deb tool (apt/dnf install dpkg) to extract
        the kernel from the Debian package.
        This test also requires the QEMU binary to be compiled with:

          $ configure ... --enable-trace-backends=log

        The kernel can be rebuilt using this Debian kernel source [1] and
        following the instructions on [2].

        [1] https://kernel-team.pages.debian.net/kernel-handbook/ch-common-tasks.html#s-common-official
        [2] http://snapshot.debian.org/package/linux-2.6/2.6.32-30/#linux-source-2.6.32_2.6.32-30

        :avocado: tags=arch:sh4
        """
        if self.arch != 'sh4':
            self.cancel('Currently specific to the %s target arch' % self.arch)

        deb_url = ('http://snapshot.debian.org/archive/'
                   'debian-ports/20110116T065852Z/pool-sh4/main/l/'
                   'linux-2.6/linux-image-2.6.32-5-sh7751r_2.6.32-30_sh4.deb')
        deb_hash = '8025e503319dc8ad786756e3afaa8eb868e9ef59'
        deb_path = self.fetch_asset(deb_url, asset_hash=deb_hash)
        subprocess.check_call(['dpkg-deb', '--extract', deb_path, self.workdir])
        kernel_path = self.workdir + '/boot/vmlinuz-2.6.32-5-sh7751r'
        trace_path = os.path.join(self.workdir, 'trace.log')
        trace_logger = logging.getLogger('trace')

        self.vm.set_arch(self.arch)
        self.vm.set_machine('r2d')
        kernel_command_line = 'noiotrap'
        self.vm.add_args('-trace', "enable=usb_ohci_*,file=" + trace_path,
                         '-kernel', kernel_path,
                         '-append', kernel_command_line)

        self.vm.launch()
        if not wait_for(read_stream_for_string, timeout=15, step=0,
                        args=(open(trace_path),
                              'usb_ohci_hub_power_up powered up all ports',
                              trace_logger)):
            self.fail("Machine failed to boot")
