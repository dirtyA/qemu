/*
 * Copyright (c) 2013
 * Guillaume Subiron, Yann Bordenave, Serigne Modou Wagne.
 */

#ifndef SLIRP_IP6_H
#define SLIRP_IP6_H

#include "net/eth.h"

#ifdef __GNUC__
# undef IN6_IS_ADDR_UNSPECIFIED
# pragma GCC poison IN6_IS_ADDR_UNSPECIFIED
# undef IN6_IS_ADDR_LOOPBACK
# pragma GCC poison IN6_IS_ADDR_LOOPBACK
# undef IN6_IS_ADDR_LINKLOCAL
# pragma GCC poison IN6_IS_ADDR_LINKLOCAL
# undef IN6_IS_ADDR_SITELOCAL
# pragma GCC poison IN6_IS_ADDR_SITELOCAL
# undef IN6_IS_ADDR_V4MAPPED
# pragma GCC poison IN6_IS_ADDR_V4MAPPED
# undef IN6_IS_ADDR_V4COMPAT
# pragma GCC poison IN6_IS_ADDR_V4COMPAT
# undef IN6_ARE_ADDR_EQUAL
# pragma GCC poison IN6_ARE_ADDR_EQUAL
# undef IN6_IS_ADDR_MULTICAST
# pragma GCC poison IN6_IS_ADDR_MULTICAST
#endif

#define ALLNODES_MULTICAST  { .s6_addr = \
                            { 0xff, 0x02, 0x00, 0x00,\
                            0x00, 0x00, 0x00, 0x00,\
                            0x00, 0x00, 0x00, 0x00,\
                            0x00, 0x00, 0x00, 0x01 } }

#define SOLICITED_NODE_PREFIX { .s6_addr = \
                            { 0xff, 0x02, 0x00, 0x00,\
                            0x00, 0x00, 0x00, 0x00,\
                            0x00, 0x00, 0x00, 0x01,\
                            0xff, 0x00, 0x00, 0x00 } }

#define LINKLOCAL_ADDR  { .s6_addr = \
                        { 0xfe, 0x80, 0x00, 0x00,\
                        0x00, 0x00, 0x00, 0x00,\
                        0x00, 0x00, 0x00, 0x00,\
                        0x00, 0x00, 0x00, 0x02 } }

#define ZERO_ADDR  { .s6_addr = \
                        { 0x00, 0x00, 0x00, 0x00,\
                        0x00, 0x00, 0x00, 0x00,\
                        0x00, 0x00, 0x00, 0x00,\
                        0x00, 0x00, 0x00, 0x00 } }

#ifdef HOST_SUPPORTS_UNALIGNED_ACCESS
static inline bool in6_equal(const struct in6_addr *a, const struct in6_addr *b)
{
    return memcmp(a, b, sizeof(*a)) == 0;
}
#else
static inline bool in6_equal(const void *a, const void *b)
{
    return memcmp(a, b, sizeof(struct in6_addr)) == 0;
}
#endif

#ifdef HOST_SUPPORTS_UNALIGNED_ACCESS
static inline bool in6_equal_net(const struct in6_addr *a,
                                 const struct in6_addr *b,
                                 int prefix_len)
{
    if (memcmp(a, b, prefix_len / 8) != 0) {
        return 0;
    }

    if (prefix_len % 8 == 0) {
        return 1;
    }

    return a->s6_addr[prefix_len / 8] >> (8 - (prefix_len % 8))
        == b->s6_addr[prefix_len / 8] >> (8 - (prefix_len % 8));
}
#else
static inline bool in6_equal_net(const void *a,
                                 const void *b,
                                 int prefix_len)
{
    const uint8_t *aa = (uint8_t *)a;
    const uint8_t *ab = (uint8_t *)b;

    if (memcmp(a, b, prefix_len / 8) != 0) {
        return 0;
    }

    if (prefix_len % 8 == 0) {
        return 1;
    }

    return (aa[prefix_len / 8] >> (8 - (prefix_len % 8)))
        == (ab[prefix_len / 8] >> (8 - (prefix_len % 8)));
}
#endif

#ifdef HOST_SUPPORTS_UNALIGNED_ACCESS
static inline bool in6_equal_mach(const struct in6_addr *a,
                                  const struct in6_addr *b,
                                  int prefix_len)
{
    if (memcmp(&(a->s6_addr[DIV_ROUND_UP(prefix_len, 8)]),
               &(b->s6_addr[DIV_ROUND_UP(prefix_len, 8)]),
               16 - DIV_ROUND_UP(prefix_len, 8)) != 0) {
        return 0;
    }

    if (prefix_len % 8 == 0) {
        return 1;
    }

    return (a->s6_addr[prefix_len / 8] & ((1U << (8 - (prefix_len % 8))) - 1))
        == (b->s6_addr[prefix_len / 8] & ((1U << (8 - (prefix_len % 8))) - 1));
}
#else
static inline bool in6_equal_mach(const void *a,
                                  const void *b,
                                  int prefix_len)
{
    const uint8_t *aa = (uint8_t *)a;
    const uint8_t *ab = (uint8_t *)b;

    if (memcmp(&(aa[DIV_ROUND_UP(prefix_len, 8)]),
               &(ab[DIV_ROUND_UP(prefix_len, 8)]),
               16 - DIV_ROUND_UP(prefix_len, 8)) != 0) {
        return 0;
    }

    if (prefix_len % 8 == 0) {
        return 1;
    }

    return (aa[prefix_len / 8] & ((1U << (8 - (prefix_len % 8))) - 1))
        == (ab[prefix_len / 8] & ((1U << (8 - (prefix_len % 8))) - 1));
}
#endif


#define in6_equal_router(a)\
    ((in6_equal_net(a, &slirp->vprefix_addr6, slirp->vprefix_len)\
      && in6_equal_mach(a, &slirp->vhost_addr6, slirp->vprefix_len))\
  || (in6_equal_net(a, &(struct in6_addr)LINKLOCAL_ADDR, 64)\
      && in6_equal_mach(a, &slirp->vhost_addr6, 64)))

#define in6_equal_dns(a)\
    ((in6_equal_net(a, &slirp->vprefix_addr6, slirp->vprefix_len)\
      && in6_equal_mach(a, &slirp->vnameserver_addr6, slirp->vprefix_len))\
  || (in6_equal_net(a, &(struct in6_addr)LINKLOCAL_ADDR, 64)\
      && in6_equal_mach(a, &slirp->vnameserver_addr6, 64)))

#define in6_equal_host(a)\
    (in6_equal_router(a) || in6_equal_dns(a))

#define in6_solicitednode_multicast(a)\
    (in6_equal_net(a, &(struct in6_addr)SOLICITED_NODE_PREFIX, 104))

#define in6_zero(a)\
    (in6_equal(a, &(struct in6_addr)ZERO_ADDR))

#ifdef HOST_SUPPORTS_UNALIGNED_ACCESS
static inline bool in6_multicast(const struct in6_addr *a)
{
    return a->s6_addr[0] == 0xff;
}
#else
static inline bool in6_multicast(const void *a)
{
    return ((const uint8_t *)a)[0] == 0xff;
}
#endif

/* Compute emulated host MAC address from its ipv6 address */
static inline void in6_compute_ethaddr(struct in6_addr ip,
                                       uint8_t eth[ETH_ALEN])
{
    eth[0] = 0x52;
    eth[1] = 0x56;
    memcpy(&eth[2], &ip.s6_addr[16 - (ETH_ALEN - 2)], ETH_ALEN - 2);
}

/*
 * Definitions for internet protocol version 6.
 * Per RFC 2460, December 1998.
 */
#define IP6VERSION      6
#define IP6_HOP_LIMIT 255

/*
 * Structure of an internet header, naked of options.
 */
struct ip6 {
#ifdef HOST_WORDS_BIGENDIAN
    uint32_t
        ip_v:4,         /* version */
        ip_tc_hi:4,     /* traffic class */
        ip_tc_lo:4,
        ip_fl_hi:4,     /* flow label */
        ip_fl_lo:16;
#else
    uint32_t
        ip_tc_hi:4,
        ip_v:4,
        ip_fl_hi:4,
        ip_tc_lo:4,
        ip_fl_lo:16;
#endif
    uint16_t    ip_pl;               /* payload length */
    uint8_t     ip_nh;               /* next header */
    uint8_t     ip_hl;               /* hop limit */
    struct in6_addr ip_src, ip_dst;  /* source and dest address */
} QEMU_PACKED;

/*
 * IPv6 pseudo-header used by upper-layer protocols
 */
struct ip6_pseudohdr {
    struct      in6_addr ih_src;  /* source internet address */
    struct      in6_addr ih_dst;  /* destination internet address */
    uint32_t    ih_pl;            /* upper-layer packet length */
    uint16_t    ih_zero_hi;       /* zero */
    uint8_t     ih_zero_lo;       /* zero */
    uint8_t     ih_nh;            /* next header */
} QEMU_PACKED;


#endif
