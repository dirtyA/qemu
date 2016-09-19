/*
 * Virtio crypto Support
 *
 * Copyright (c) 2016 HUAWEI TECHNOLOGIES CO., LTD.
 *
 * Authors:
 *    Gonglei <arei.gonglei@huawei.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or
 * (at your option) any later version.  See the COPYING file in the
 * top-level directory.
 */

#ifndef _QEMU_VIRTIO_CRYPTO_H
#define _QEMU_VIRTIO_CRYPTO_H

#include "standard-headers/linux/virtio_crypto.h"
#include "hw/virtio/virtio.h"
#include "sysemu/iothread.h"
#include "crypto/cryptodev.h"

#define VIRTIO_ID_CRYPTO 20

/* #define DEBUG_VIRTIO_CRYPTO */

#ifdef DEBUG_VIRTIO_CRYPTO
#define DPRINTF(fmt, ...) \
do { printf("virtio_crypto: " fmt , ## __VA_ARGS__); } while (0)
#else
#define DPRINTF(fmt, ...) do { } while (0)
#endif

#define TYPE_VIRTIO_CRYPTO "virtio-crypto-device"
#define VIRTIO_CRYPTO(obj) \
        OBJECT_CHECK(VirtIOCrypto, (obj), TYPE_VIRTIO_CRYPTO)
#define VIRTIO_CRYPTO_GET_PARENT_CLASS(obj) \
        OBJECT_GET_PARENT_CLASS(obj, TYPE_VIRTIO_CRYPTO)

/* Max entries of scatter gather list in one virtio-crypto buffer */
#define VIRTIO_CRYPTO_SG_MAX 256

typedef struct VirtIOCryptoBuffer {
    unsigned int num;
    /* Guest physical address */
    hwaddr *addr;
    /* Store host virtual address and length */
    struct iovec *sg;
    uint8_t data[0];
} VirtIOCryptoBuffer;

typedef struct VirtIOCryptoConf {
    QCryptoCryptoDevBackend *cryptodev;

    /* Supported service mask */
    uint32_t crypto_services;

    /* Detailed algorithms mask */
    uint32_t cipher_algo_l;
    uint32_t cipher_algo_h;
    uint32_t hash_algo;
    uint32_t mac_algo_l;
    uint32_t mac_algo_h;
    uint32_t asym_algo;
    uint32_t kdf_algo;
    uint32_t aead_algo;
    uint32_t primitive_algo;
} VirtIOCryptoConf;

struct VirtIOCrypto;

typedef struct VirtIOCryptoReq {
    VirtQueueElement elem;
    /* flags of operation, such as type of algorithm */
    uint32_t flags;
    /* address of in data (Device to Driver) */
    void *idata_hva;
    VirtQueue *vq;
    struct VirtIOCrypto *vcrypto;
    union {
        QCryptoCryptoDevBackendSymOpInfo *sym_op_info;
    } u;
} VirtIOCryptoReq;

typedef struct VirtIOCrypto {
    VirtIODevice parent_obj;

    VirtQueue *ctrl_vq;

    VirtIOCryptoConf conf;
    QCryptoCryptoDevBackend *cryptodev;

    uint32_t max_queues;
    uint32_t status;

    int multiqueue;
    uint32_t curr_queues;
    size_t config_size;
} VirtIOCrypto;

#endif /* _QEMU_VIRTIO_CRYPTO_H */
