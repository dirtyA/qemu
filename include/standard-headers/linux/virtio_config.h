#ifndef _LINUX_VIRTIO_CONFIG_H
#define _LINUX_VIRTIO_CONFIG_H
/* This header, excluding the #ifdef __KERNEL__ part, is BSD licensed so
 * anyone can use the definitions to implement compatible drivers/servers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of IBM nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL IBM OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE. */

/* Virtio devices use a standardized configuration space to define their
 * features and pass configuration information, but each implementation can
 * store and access that space differently. */
#include "standard-headers/linux/types.h"

/* Status byte for guest to report progress, and synchronize features. */
/* We have seen device and processed generic fields (VIRTIO_CONFIG_F_VIRTIO) */
#define VIRTIO_CONFIG_S_ACKNOWLEDGE	1
/* We have found a driver for the device. */
#define VIRTIO_CONFIG_S_DRIVER		2
/* Driver has used its parts of the config, and is happy */
#define VIRTIO_CONFIG_S_DRIVER_OK	4
/* Driver has finished configuring features */
#define VIRTIO_CONFIG_S_FEATURES_OK	8
/* Device entered invalid state, driver must reset it */
#define VIRTIO_CONFIG_S_NEEDS_RESET	0x40
/* We've given up on this device. */
#define VIRTIO_CONFIG_S_FAILED		0x80

/*
 * Virtio feature bits VIRTIO_TRANSPORT_F_START through
 * VIRTIO_TRANSPORT_F_END are reserved for the transport
 * being used (e.g. virtio_ring, virtio_pci etc.), the
 * rest are per-device feature bits.
 */
#define VIRTIO_TRANSPORT_F_START	28
#define VIRTIO_TRANSPORT_F_END		38

#ifndef VIRTIO_CONFIG_NO_LEGACY
/* Do we get callbacks when the ring is completely used, even if we've
 * suppressed them? */
#define VIRTIO_F_NOTIFY_ON_EMPTY	24

/* Can the device handle any descriptor layout? */
#define VIRTIO_F_ANY_LAYOUT		27
#endif /* VIRTIO_CONFIG_NO_LEGACY */

/* v1.0 compliant. */
#define VIRTIO_F_VERSION_1		32

/*
 * If clear - device has the IOMMU bypass quirk feature.
 * If set - use platform tools to detect the IOMMU.
 *
 * Note the reverse polarity (compared to most other features),
 * this is for compatibility with legacy systems.
 */
#define VIRTIO_F_IOMMU_PLATFORM		33

/* This feature indicates support for the packed virtqueue layout. */
#define VIRTIO_F_RING_PACKED		34

/* Enable events */
#define RING_EVENT_FLAGS_ENABLE 0x0
/* Disable events */
#define RING_EVENT_FLAGS_DISABLE 0x1
/*
 *  * Enable events for a specific descriptor
 *   * (as specified by Descriptor Ring Change Event Offset/Wrap Counter).
 *    * Only valid if VIRTIO_F_RING_EVENT_IDX has been negotiated.
 *     */
#define RING_EVENT_FLAGS_DESC 0x2
/* The value 0x3 is reserved */

/*
 * Does the device support Single Root I/O Virtualization?
 */
#define VIRTIO_F_SR_IOV			37
#endif /* _LINUX_VIRTIO_CONFIG_H */
