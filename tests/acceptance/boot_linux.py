# Functional test that boots a complete Linux system via a cloud image
#
# Copyright (c) 2018-2020 Red Hat, Inc.
#
# Author:
#  Cleber Rosa <crosa@redhat.com>
#
# This work is licensed under the terms of the GNU GPL, version 2 or
# later.  See the COPYING file in the top-level directory.

import os

from avocado_qemu import Test, BUILD_DIR

from qemu.accel import kvm_available
from qemu.accel import tcg_available

from avocado.utils import cloudinit
from avocado.utils import network
from avocado.utils import vmimage
from avocado.utils import datadrainer

ACCEL_NOT_AVAILABLE_FMT = "%s accelerator does not seem to be available"
KVM_NOT_AVAILABLE = ACCEL_NOT_AVAILABLE_FMT % "KVM"
TCG_NOT_AVAILABLE = ACCEL_NOT_AVAILABLE_FMT % "TCG"


class BootLinux(Test):
    """
    Boots a Linux system, checking for a successful initialization
    """

    timeout = 900
    chksum = None

    def setUp(self):
        super(BootLinux, self).setUp()
        self.prepare_boot()
        self.vm.add_args('-smp', '2')
        self.vm.add_args('-m', '1024')
        self.vm.add_args('-drive', 'file=%s' % self.boot.path)
        self.prepare_cloudinit()

    def prepare_boot(self):
        self.log.info('Downloading/preparing boot image')
        # Fedora 31 only provides ppc64le images
        image_arch = self.arch
        if image_arch == 'ppc64':
            image_arch = 'ppc64le'
        # If qemu-img has been built, use it, otherwise the system wide one
        # will be used.  If none is available, the test will cancel.
        qemu_img = os.path.join(BUILD_DIR, 'qemu-img')
        if os.path.exists(qemu_img):
            vmimage.QEMU_IMG = qemu_img
        try:
            self.boot = vmimage.get(
                'fedora', arch=image_arch, version='31',
                checksum=self.chksum,
                algorithm='sha256',
                cache_dir=self.cache_dirs[0],
                snapshot_dir=self.workdir)
        except:
            self.cancel('Failed to download/prepare boot image')

    def prepare_cloudinit(self):
        self.log.info('Preparing cloudinit image')
        try:
            cloudinit_iso = os.path.join(self.workdir, 'cloudinit.iso')
            self.phone_home_port = network.find_free_port()
            cloudinit.iso(cloudinit_iso, self.name,
                          username='root',
                          password='password',
                          # QEMU's hard coded usermode router address
                          phone_home_host='10.0.2.2',
                          phone_home_port=self.phone_home_port)
            self.vm.add_args('-drive', 'file=%s,format=raw' % cloudinit_iso)
        except Exception:
            self.cancel('Failed to prepared cloudinit image')

    def launch_and_wait(self):
        self.vm.set_console()
        self.vm.launch()
        console_drainer = datadrainer.LineLogger(self.vm.console_socket.fileno(),
                                                 logger=self.log.getChild('console'))
        console_drainer.start()
        self.log.info('VM launched, waiting for boot confirmation from guest')
        cloudinit.wait_for_phone_home(('0.0.0.0', self.phone_home_port), self.name)


class BootLinuxX8664(BootLinux):
    """
    :avocado: tags=arch:x86_64
    """

    chksum = 'e3c1b309d9203604922d6e255c2c5d098a309c2d46215d8fc026954f3c5c27a0'

    def test_pc_i440fx_tcg(self):
        """
        :avocado: tags=machine:pc
        :avocado: tags=machine:pc-i440fx
        :avocado: tags=accel:tcg
        """
        if not tcg_available(self.qemu_bin):
            self.cancel(TCG_NOT_AVAILABLE)
        self.vm.add_args("-accel", "tcg")
        self.launch_and_wait()

    def test_pc_i440fx_kvm(self):
        """
        :avocado: tags=machine:pc
        :avocado: tags=machine:pc-i440fx
        :avocado: tags=accel:kvm
        """
        if not kvm_available(self.arch, self.qemu_bin):
            self.cancel(KVM_NOT_AVAILABLE)
        self.vm.add_args("-accel", "kvm")
        self.launch_and_wait()

    def test_pc_q35_tcg(self):
        """
        :avocado: tags=machine:pc
        :avocado: tags=machine:pc-q35
        :avocado: tags=accel:tcg
        """
        if not tcg_available(self.qemu_bin):
            self.cancel(TCG_NOT_AVAILABLE)
        self.vm.add_args("-accel", "tcg")
        self.launch_and_wait()

    def test_pc_q35_kvm(self):
        """
        :avocado: tags=machine:q35
        :avocado: tags=machine:pc-q35
        :avocado: tags=accel:kvm
        """
        if not kvm_available(self.arch, self.qemu_bin):
            self.cancel(KVM_NOT_AVAILABLE)
        self.vm.add_args("-accel", "kvm")
        self.launch_and_wait()


class BootLinuxAarch64(BootLinux):
    """
    :avocado: tags=arch:aarch64
    :avocado: tags=machine:virt
    """

    chksum = '1e18d9c0cf734940c4b5d5ec592facaed2af0ad0329383d5639c997fdf16fe49'

    def add_common_args(self):
        self.vm.add_args('-bios',
                         os.path.join(BUILD_DIR, 'pc-bios',
                                      'edk2-aarch64-code.fd'))
        self.vm.add_args('-device', 'virtio-rng-pci,rng=rng0')
        self.vm.add_args('-object', 'rng-random,id=rng0,filename=/dev/urandom')

    def test_virt_tcg(self):
        """
        :avocado: tags=accel:tcg
        :avocado: tags=cpu:cortex-a53
        """
        if not tcg_available(self.qemu_bin):
            self.cancel(TCG_NOT_AVAILABLE)
        self.vm.add_args("-accel", "tcg")
        self.vm.add_args('-cpu', 'cortex-a53')
        self.add_common_args()
        self.launch_and_wait()

    def test_virt_kvm(self):
        """
        :avocado: tags=accel:kvm
        :avocado: tags=cpu:host
        """
        if not kvm_available(self.arch, self.qemu_bin):
            self.cancel(KVM_NOT_AVAILABLE)
        self.vm.add_args("-accel", "kvm")
        self.vm.add_args("-cpu", "host")
        self.add_common_args()
        self.launch_and_wait()


class BootLinuxPPC64(BootLinux):
    """
    :avocado: tags=arch:ppc64
    """

    chksum = '7c3528b85a3df4b2306e892199a9e1e43f991c506f2cc390dc4efa2026ad2f58'

    def test_pseries_tcg(self):
        """
        :avocado: tags=machine:pseries
        :avocado: tags=accel:tcg
        """
        if not tcg_available(self.qemu_bin):
            self.cancel(TCG_NOT_AVAILABLE)
        self.vm.add_args("-accel", "tcg")
        self.launch_and_wait()


class BootLinuxS390X(BootLinux):
    """
    :avocado: tags=arch:s390x
    """

    chksum = '4caaab5a434fd4d1079149a072fdc7891e354f834d355069ca982fdcaf5a122d'

    def test_s390_ccw_virtio_tcg(self):
        """
        :avocado: tags=machine:s390-ccw-virtio
        :avocado: tags=accel:tcg
        """
        if not tcg_available(self.qemu_bin):
            self.cancel(TCG_NOT_AVAILABLE)
        self.vm.add_args("-accel", "tcg")
        self.launch_and_wait()
