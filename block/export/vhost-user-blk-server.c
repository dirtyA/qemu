/*
 * Sharing QEMU block devices via vhost-user protocal
 *
 * Author: Coiby Xu <coiby.xu@gmail.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or
 * later.  See the COPYING file in the top-level directory.
 */
#include "qemu/osdep.h"
#include "block/block.h"
#include "vhost-user-blk-server.h"
#include "qapi/error.h"
#include "qom/object_interfaces.h"
#include "sysemu/block-backend.h"

enum {
    VHOST_USER_BLK_MAX_QUEUES = 1,
};
struct virtio_blk_inhdr {
    unsigned char status;
};

static QTAILQ_HEAD(, VuBlockDev) vu_block_devs =
                                 QTAILQ_HEAD_INITIALIZER(vu_block_devs);


typedef struct VuBlockReq {
    VuVirtqElement *elem;
    int64_t sector_num;
    size_t size;
    struct virtio_blk_inhdr *in;
    struct virtio_blk_outhdr out;
    VuServer *server;
    struct VuVirtq *vq;
} VuBlockReq;


static void vu_block_req_complete(VuBlockReq *req)
{
    VuDev *vu_dev = &req->server->vu_dev;

    /* IO size with 1 extra status byte */
    vu_queue_push(vu_dev, req->vq, req->elem, req->size + 1);
    vu_queue_notify(vu_dev, req->vq);

    if (req->elem) {
        free(req->elem);
    }

    g_free(req);
}

static VuBlockDev *get_vu_block_device_by_server(VuServer *server)
{
    return container_of(server, VuBlockDev, vu_server);
}

static int coroutine_fn
vu_block_discard_write_zeroes(VuBlockReq *req, struct iovec *iov,
                              uint32_t iovcnt, uint32_t type)
{
    struct virtio_blk_discard_write_zeroes desc;
    ssize_t size = iov_to_buf(iov, iovcnt, 0, &desc, sizeof(desc));
    if (unlikely(size != sizeof(desc))) {
        error_report("Invalid size %ld, expect %ld", size, sizeof(desc));
        return -EINVAL;
    }

    VuBlockDev *vdev_blk = get_vu_block_device_by_server(req->server);
    uint64_t range[2] = { le64toh(desc.sector) << 9,
                          le32toh(desc.num_sectors) << 9 };
    if (type == VIRTIO_BLK_T_DISCARD) {
        if (blk_co_pdiscard(vdev_blk->backend, range[0], range[1]) == 0) {
            return 0;
        }
    } else if (type == VIRTIO_BLK_T_WRITE_ZEROES) {
        if (blk_co_pwrite_zeroes(vdev_blk->backend,
                                 range[0], range[1], 0) == 0) {
            return 0;
        }
    }

    return -EINVAL;
}


static void coroutine_fn vu_block_flush(VuBlockReq *req)
{
    VuBlockDev *vdev_blk = get_vu_block_device_by_server(req->server);
    BlockBackend *backend = vdev_blk->backend;
    blk_co_flush(backend);
}


struct req_data {
    VuServer *server;
    VuVirtq *vq;
    VuVirtqElement *elem;
};

static void coroutine_fn vu_block_virtio_process_req(void *opaque)
{
    struct req_data *data = opaque;
    VuServer *server = data->server;
    VuVirtq *vq = data->vq;
    VuVirtqElement *elem = data->elem;
    uint32_t type;
    VuBlockReq *req;

    VuBlockDev *vdev_blk = get_vu_block_device_by_server(server);
    BlockBackend *backend = vdev_blk->backend;

    struct iovec *in_iov = elem->in_sg;
    struct iovec *out_iov = elem->out_sg;
    unsigned in_num = elem->in_num;
    unsigned out_num = elem->out_num;
    /* refer to hw/block/virtio_blk.c */
    if (elem->out_num < 1 || elem->in_num < 1) {
        error_report("virtio-blk request missing headers");
        free(elem);
        return;
    }

    req = g_new0(VuBlockReq, 1);
    req->server = server;
    req->vq = vq;
    req->elem = elem;

    if (unlikely(iov_to_buf(out_iov, out_num, 0, &req->out,
                            sizeof(req->out)) != sizeof(req->out))) {
        error_report("virtio-blk request outhdr too short");
        goto err;
    }

    iov_discard_front(&out_iov, &out_num, sizeof(req->out));

    if (in_iov[in_num - 1].iov_len < sizeof(struct virtio_blk_inhdr)) {
        error_report("virtio-blk request inhdr too short");
        goto err;
    }

    /* We always touch the last byte, so just see how big in_iov is.  */
    req->in = (void *)in_iov[in_num - 1].iov_base
              + in_iov[in_num - 1].iov_len
              - sizeof(struct virtio_blk_inhdr);
    iov_discard_back(in_iov, &in_num, sizeof(struct virtio_blk_inhdr));


    type = le32toh(req->out.type);
    switch (type & ~VIRTIO_BLK_T_BARRIER) {
    case VIRTIO_BLK_T_IN:
    case VIRTIO_BLK_T_OUT: {
        ssize_t ret = 0;
        bool is_write = type & VIRTIO_BLK_T_OUT;
        req->sector_num = le64toh(req->out.sector);

        int64_t offset = req->sector_num * vdev_blk->blk_size;
        QEMUIOVector *qiov = g_new0(QEMUIOVector, 1);
        if (is_write) {
            qemu_iovec_init_external(qiov, out_iov, out_num);
            ret = blk_co_pwritev(backend, offset, qiov->size,
                                 qiov, 0);
        } else {
            qemu_iovec_init_external(qiov, in_iov, in_num);
            ret = blk_co_preadv(backend, offset, qiov->size,
                                qiov, 0);
        }
        if (ret >= 0) {
            req->in->status = VIRTIO_BLK_S_OK;
        } else {
            req->in->status = VIRTIO_BLK_S_IOERR;
        }
        g_free(qiov);
        break;
    }
    case VIRTIO_BLK_T_FLUSH:
        vu_block_flush(req);
        req->in->status = VIRTIO_BLK_S_OK;
        break;
    case VIRTIO_BLK_T_GET_ID: {
        size_t size = MIN(iov_size(&elem->in_sg[0], in_num),
                          VIRTIO_BLK_ID_BYTES);
        snprintf(elem->in_sg[0].iov_base, size, "%s", "vhost_user_blk_server");
        req->in->status = VIRTIO_BLK_S_OK;
        req->size = elem->in_sg[0].iov_len;
        break;
    }
    case VIRTIO_BLK_T_DISCARD:
    case VIRTIO_BLK_T_WRITE_ZEROES: {
        int rc;
        rc = vu_block_discard_write_zeroes(req, &elem->out_sg[1],
                                           out_num, type);
        if (rc == 0) {
            req->in->status = VIRTIO_BLK_S_OK;
        } else {
            req->in->status = VIRTIO_BLK_S_IOERR;
        }
        break;
    }
    default:
        req->in->status = VIRTIO_BLK_S_UNSUPP;
        break;
    }

    vu_block_req_complete(req);
    return;

err:
    free(elem);
    g_free(req);
    return;
}



static void vu_block_process_vq(VuDev *vu_dev, int idx)
{
    VuServer *server;
    VuVirtq *vq;

    server = container_of(vu_dev, VuServer, vu_dev);
    assert(server);

    vq = vu_get_queue(vu_dev, idx);
    assert(vq);
    VuVirtqElement *elem;
    while (1) {
        elem = vu_queue_pop(vu_dev, vq, sizeof(VuVirtqElement) +
                                    sizeof(VuBlockReq));
        if (elem) {
            struct req_data req_data = {
                .server = server,
                .vq = vq,
                .elem = elem
            };
            Coroutine *co = qemu_coroutine_create(vu_block_virtio_process_req,
                                                  &req_data);
            aio_co_enter(server->ioc->ctx, co);
        } else {
            break;
        }
    }
}

static void vu_block_queue_set_started(VuDev *vu_dev, int idx, bool started)
{
    VuVirtq *vq;

    assert(vu_dev);

    vq = vu_get_queue(vu_dev, idx);
    vu_set_queue_handler(vu_dev, vq, started ? vu_block_process_vq : NULL);
}

static uint64_t vu_block_get_features(VuDev *dev)
{
    uint64_t features;
    VuServer *server = container_of(dev, VuServer, vu_dev);
    VuBlockDev *vdev_blk = get_vu_block_device_by_server(server);
    features = 1ull << VIRTIO_BLK_F_SIZE_MAX |
               1ull << VIRTIO_BLK_F_SEG_MAX |
               1ull << VIRTIO_BLK_F_TOPOLOGY |
               1ull << VIRTIO_BLK_F_BLK_SIZE |
               1ull << VIRTIO_BLK_F_FLUSH |
               1ull << VIRTIO_BLK_F_DISCARD |
               1ull << VIRTIO_BLK_F_WRITE_ZEROES |
               1ull << VIRTIO_BLK_F_CONFIG_WCE |
               1ull << VIRTIO_F_VERSION_1 |
               1ull << VIRTIO_RING_F_INDIRECT_DESC |
               1ull << VIRTIO_RING_F_EVENT_IDX |
               1ull << VHOST_USER_F_PROTOCOL_FEATURES;

    if (!vdev_blk->writable) {
        features |= 1ull << VIRTIO_BLK_F_RO;
    }

    return features;
}

static uint64_t vu_block_get_protocol_features(VuDev *dev)
{
    return 1ull << VHOST_USER_PROTOCOL_F_CONFIG |
           1ull << VHOST_USER_PROTOCOL_F_INFLIGHT_SHMFD;
}

static int
vu_block_get_config(VuDev *vu_dev, uint8_t *config, uint32_t len)
{
    VuServer *server = container_of(vu_dev, VuServer, vu_dev);
    VuBlockDev *vdev_blk = get_vu_block_device_by_server(server);
    memcpy(config, &vdev_blk->blkcfg, len);

    return 0;
}

static int
vu_block_set_config(VuDev *vu_dev, const uint8_t *data,
                    uint32_t offset, uint32_t size, uint32_t flags)
{
    VuServer *server = container_of(vu_dev, VuServer, vu_dev);
    VuBlockDev *vdev_blk = get_vu_block_device_by_server(server);
    uint8_t wce;

    /* don't support live migration */
    if (flags != VHOST_SET_CONFIG_TYPE_MASTER) {
        return -EINVAL;
    }


    if (offset != offsetof(struct virtio_blk_config, wce) ||
        size != 1) {
        return -EINVAL;
    }

    wce = *data;
    if (wce == vdev_blk->blkcfg.wce) {
        /* Do nothing as same with old configuration */
        return 0;
    }

    vdev_blk->blkcfg.wce = wce;
    blk_set_enable_write_cache(vdev_blk->backend, wce);
    return 0;
}


/*
 * When the client disconnects, it sends a VHOST_USER_NONE request
 * and vu_process_message will simple call exit which cause the VM
 * to exit abruptly.
 * To avoid this issue,  process VHOST_USER_NONE request ahead
 * of vu_process_message.
 *
 */
static int vu_block_process_msg(VuDev *dev, VhostUserMsg *vmsg, int *do_reply)
{
    if (vmsg->request == VHOST_USER_NONE) {
        dev->panic(dev, "disconnect");
        return true;
    }
    return false;
}


static const VuDevIface vu_block_iface = {
    .get_features          = vu_block_get_features,
    .queue_set_started     = vu_block_queue_set_started,
    .get_protocol_features = vu_block_get_protocol_features,
    .get_config            = vu_block_get_config,
    .set_config            = vu_block_set_config,
    .process_msg           = vu_block_process_msg,
};

static void blk_aio_attached(AioContext *ctx, void *opaque)
{
    VuBlockDev *vub_dev = opaque;
    aio_context_acquire(ctx);
    vhost_user_server_set_aio_context(ctx, &vub_dev->vu_server);
    aio_context_release(ctx);
}

static void blk_aio_detach(void *opaque)
{
    VuBlockDev *vub_dev = opaque;
    AioContext *ctx = vub_dev->vu_server.ctx;
    aio_context_acquire(ctx);
    vhost_user_server_set_aio_context(NULL, &vub_dev->vu_server);
    aio_context_release(ctx);
}

static void vu_block_free(VuBlockDev *vu_block_dev)
{
    if (!vu_block_dev) {
        return;
    }

    if (vu_block_dev->backend) {
        blk_remove_aio_context_notifier(vu_block_dev->backend, blk_aio_attached,
                                        blk_aio_detach, vu_block_dev);
    }

    blk_unref(vu_block_dev->backend);

    if (vu_block_dev->next.tqe_circ.tql_prev) {
        /*
         * remove vu_block_device from the list
         *
         * if vu_block_dev->next.tqe_circ.tql_prev = null,
         * vu_block_dev hasn't been inserted into the queue and
         * vu_block_free is called by obj->instance_finalize.
         */
        QTAILQ_REMOVE(&vu_block_devs, vu_block_dev, next);
    }
}

static void
vu_block_initialize_config(BlockDriverState *bs,
                           struct virtio_blk_config *config, uint32_t blk_size)
{
    config->capacity = bdrv_getlength(bs) >> BDRV_SECTOR_BITS;
    config->blk_size = blk_size;
    config->size_max = 0;
    config->seg_max = 128 - 2;
    config->min_io_size = 1;
    config->opt_io_size = 1;
    config->num_queues = VHOST_USER_BLK_MAX_QUEUES;
    config->max_discard_sectors = 32768;
    config->max_discard_seg = 1;
    config->discard_sector_alignment = config->blk_size >> 9;
    config->max_write_zeroes_sectors = 32768;
    config->max_write_zeroes_seg = 1;
}


static VuBlockDev *vu_block_init(VuBlockDev *vu_block_device, Error **errp)
{

    BlockBackend *blk;
    Error *local_error = NULL;
    const char *node_name = vu_block_device->node_name;
    bool writable = vu_block_device->writable;
    /*
     * Don't allow resize while the vhost user server is running,
     * otherwise we don't care what happens with the node.
     */
    uint64_t perm = BLK_PERM_CONSISTENT_READ;
    int ret;

    AioContext *ctx;

    BlockDriverState *bs = bdrv_lookup_bs(node_name, node_name, &local_error);

    if (!bs) {
        error_propagate(errp, local_error);
        return NULL;
    }

    if (bdrv_is_read_only(bs)) {
        writable = false;
    }

    if (writable) {
        perm |= BLK_PERM_WRITE;
    }

    ctx = bdrv_get_aio_context(bs);
    aio_context_acquire(ctx);
    bdrv_invalidate_cache(bs, NULL);
    aio_context_release(ctx);

    blk = blk_new(bdrv_get_aio_context(bs), perm,
                  BLK_PERM_CONSISTENT_READ | BLK_PERM_WRITE_UNCHANGED |
                  BLK_PERM_WRITE | BLK_PERM_GRAPH_MOD);
    ret = blk_insert_bs(blk, bs, errp);

    if (ret < 0) {
        goto fail;
    }

    blk_set_enable_write_cache(blk, false);

    blk_set_allow_aio_context_change(blk, true);

    vu_block_device->blkcfg.wce = 0;
    vu_block_device->backend = blk;
    if (!vu_block_device->blk_size) {
        vu_block_device->blk_size = BDRV_SECTOR_SIZE;
    }
    vu_block_device->blkcfg.blk_size = vu_block_device->blk_size;
    blk_set_guest_block_size(blk, vu_block_device->blk_size);
    vu_block_initialize_config(bs, &vu_block_device->blkcfg,
                                   vu_block_device->blk_size);
    return vu_block_device;

fail:
    blk_unref(blk);
    return NULL;
}

static void vhost_user_blk_server_free(VuBlockDev *vu_block_device)
{
    if (!vu_block_device) {
        return;
    }
    vhost_user_server_stop(&vu_block_device->vu_server);
    vu_block_free(vu_block_device);

}

/*
 * A exported drive can serve multiple multiple clients simutateously,
 * thus no need to export the same drive twice.
 *
 */
static VuBlockDev *vu_block_dev_find(const char *node_name)
{
    VuBlockDev *vu_block_device;
    QTAILQ_FOREACH(vu_block_device, &vu_block_devs, next) {
        if (strcmp(node_name, vu_block_device->node_name) == 0) {
            return vu_block_device;
        }
    }

    return NULL;
}


static VuBlockDev
*vu_block_dev_find_by_unix_socket(const char *unix_socket)
{
    VuBlockDev *vu_block_device;
    QTAILQ_FOREACH(vu_block_device, &vu_block_devs, next) {
        if (strcmp(unix_socket, vu_block_device->addr->u.q_unix.path) == 0) {
            return vu_block_device;
        }
    }

    return NULL;
}


static void vhost_user_blk_server_start(VuBlockDev *vu_block_device,
                                        Error **errp)
{

    const char *name = vu_block_device->node_name;
    SocketAddress *addr = vu_block_device->addr;
    char *unix_socket = vu_block_device->addr->u.q_unix.path;

    if (vu_block_dev_find(name)) {
        error_setg(errp, "Vhost-user-blk server with node-name '%s' "
                   "has already been started",
                   name);
        return;
    }

    if (vu_block_dev_find_by_unix_socket(unix_socket)) {
        error_setg(errp, "Vhost-user-blk server with with socket_path '%s' "
                   "has already been started", unix_socket);
        return;
    }

    if (!vu_block_init(vu_block_device, errp)) {
        return;
    }


    AioContext *ctx = bdrv_get_aio_context(blk_bs(vu_block_device->backend));

    if (!vhost_user_server_start(VHOST_USER_BLK_MAX_QUEUES, addr, ctx,
                                 &vu_block_device->vu_server,
                                 NULL, &vu_block_iface,
                                 errp)) {
        goto error;
    }

    QTAILQ_INSERT_TAIL(&vu_block_devs, vu_block_device, next);
    blk_add_aio_context_notifier(vu_block_device->backend, blk_aio_attached,
                                 blk_aio_detach, vu_block_device);
    return;

 error:
    vu_block_free(vu_block_device);
}

static void vu_set_node_name(Object *obj, const char *value, Error **errp)
{
    VuBlockDev *vus = VHOST_USER_BLK_SERVER(obj);

    if (vus->node_name) {
        error_setg(errp, "evdev property already set");
        return;
    }

    vus->node_name = g_strdup(value);
}

static char *vu_get_node_name(Object *obj, Error **errp)
{
    VuBlockDev *vus = VHOST_USER_BLK_SERVER(obj);
    return g_strdup(vus->node_name);
}


static void vu_set_unix_socket(Object *obj, const char *value,
                               Error **errp)
{
    VuBlockDev *vus = VHOST_USER_BLK_SERVER(obj);

    if (vus->addr) {
        error_setg(errp, "unix_socket property already set");
        return;
    }

    SocketAddress *addr = g_new0(SocketAddress, 1);
    addr->type = SOCKET_ADDRESS_TYPE_UNIX;
    addr->u.q_unix.path = g_strdup(value);
    vus->addr = addr;
}

static char *vu_get_unix_socket(Object *obj, Error **errp)
{
    VuBlockDev *vus = VHOST_USER_BLK_SERVER(obj);
    return g_strdup(vus->addr->u.q_unix.path);
}

static bool vu_get_block_writable(Object *obj, Error **errp)
{
    VuBlockDev *vus = VHOST_USER_BLK_SERVER(obj);
    return vus->writable;
}

static void vu_set_block_writable(Object *obj, bool value, Error **errp)
{
    VuBlockDev *vus = VHOST_USER_BLK_SERVER(obj);

    vus->writable = value;
}

static void vu_get_blk_size(Object *obj, Visitor *v, const char *name,
                            void *opaque, Error **errp)
{
    VuBlockDev *vus = VHOST_USER_BLK_SERVER(obj);
    uint32_t value = vus->blk_size;

    visit_type_uint32(v, name, &value, errp);
}

static void vu_set_blk_size(Object *obj, Visitor *v, const char *name,
                            void *opaque, Error **errp)
{
    VuBlockDev *vus = VHOST_USER_BLK_SERVER(obj);

    Error *local_err = NULL;
    uint32_t value;

    visit_type_uint32(v, name, &value, &local_err);
    if (local_err) {
        goto out;
    }
    if (value != BDRV_SECTOR_SIZE && value != 4096) {
        error_setg(&local_err,
                   "Property '%s.%s' can only take value 512 or 4096",
                   object_get_typename(obj), name);
        goto out;
    }

    vus->blk_size = value;

out:
    error_propagate(errp, local_err);
    vus->blk_size = value;
}


static void vhost_user_blk_server_instance_finalize(Object *obj)
{
    VuBlockDev *vub = VHOST_USER_BLK_SERVER(obj);

    vhost_user_blk_server_free(vub);
}

static void vhost_user_blk_server_complete(UserCreatable *obj, Error **errp)
{
    Error *local_error = NULL;
    VuBlockDev *vub = VHOST_USER_BLK_SERVER(obj);

    vhost_user_blk_server_start(vub, &local_error);

    if (local_error) {
        error_propagate(errp, local_error);
        return;
    }
}

static void vhost_user_blk_server_class_init(ObjectClass *klass,
                                             void *class_data)
{
    UserCreatableClass *ucc = USER_CREATABLE_CLASS(klass);
    ucc->complete = vhost_user_blk_server_complete;

    object_class_property_add_bool(klass, "writable",
                                   vu_get_block_writable,
                                   vu_set_block_writable);

    object_class_property_add_str(klass, "node-name",
                                  vu_get_node_name,
                                  vu_set_node_name);

    object_class_property_add_str(klass, "unix-socket",
                                  vu_get_unix_socket,
                                  vu_set_unix_socket);

    object_class_property_add(klass, "blk-size", "uint32",
                              vu_get_blk_size, vu_set_blk_size,
                              NULL, NULL);
}

static const TypeInfo vhost_user_blk_server_info = {
    .name = TYPE_VHOST_USER_BLK_SERVER,
    .parent = TYPE_OBJECT,
    .instance_size = sizeof(VuBlockDev),
    .instance_finalize = vhost_user_blk_server_instance_finalize,
    .class_init = vhost_user_blk_server_class_init,
    .interfaces = (InterfaceInfo[]) {
        {TYPE_USER_CREATABLE},
        {}
    },
};

static void vhost_user_blk_server_register_types(void)
{
    type_register_static(&vhost_user_blk_server_info);
}

type_init(vhost_user_blk_server_register_types)
