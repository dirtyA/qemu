#include "qemu/osdep.h"
#include "hw/virtio/virtio-gpu.h"

bool virtio_gpu_have_udmabuf(void)
{
    /* nothing (stub) */
    return false;
}

void virtio_gpu_init_udmabuf(struct virtio_gpu_simple_resource *res)
{
    /* nothing (stub) */
}

void virtio_gpu_fini_udmabuf(struct virtio_gpu_simple_resource *res)
{
    /* nothing (stub) */
}
