.. _reporting-a-bug:

Reporting a bug
===============

Bugs can be filed at our `bug
tracker <https://gitlab.com/qemu-project/qemu/-/issues>`__, which is
hosted on GitLab. Note: If you’ve got a problem with how your Linux
distribution packages QEMU, please use the bug tracker from your distro
instead.

When submitting a bug report, please try to do the following:

-  Include the QEMU release version or the git commit hash into the
   description, so that it is later still clear in which version you
   have found the bug. Reports against the `latest
   release </download/#source>`__ or even the latest development tree
   are usually acted upon faster.

-  Include the full command line used to launch the QEMU guest.

-  Reproduce the problem directly with a QEMU command-line. Avoid
   frontends and management stacks, to ensure that the bug is in QEMU
   itself and not in a frontend.

-  Include information about the host and guest (operating system,
   version, 32/64-bit).

QEMU does not use GitLab merge requests; patches are sent to the mailing
list according to the guidelines mentioned here: :ref:`submitting-a-patch`.

Do **NOT** report security issues (or other bugs, too) as “confidential”
bugs in the bug tracker. QEMU has a :ref:`security-process` for issues
that should be reported in a non-public way instead.

For problems with KVM in the kernel, use the kernel bug tracker instead;
the `KVM wiki <https://www.linux-kvm.org/page/Bugs>`__ has the details.
