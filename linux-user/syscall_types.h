
STRUCT_SPECIAL(termios)

STRUCT(winsize,
       TYPE_SHORT, TYPE_SHORT, TYPE_SHORT, TYPE_SHORT)

STRUCT(serial_multiport_struct,
       TYPE_INT, TYPE_INT, TYPE_CHAR, TYPE_CHAR, TYPE_INT, TYPE_CHAR, TYPE_CHAR,
       TYPE_INT, TYPE_CHAR, TYPE_CHAR, TYPE_INT, TYPE_CHAR, TYPE_CHAR, TYPE_INT,
       MK_ARRAY(TYPE_INT, 32))

STRUCT(serial_icounter_struct,
       TYPE_INT, TYPE_INT, TYPE_INT, TYPE_INT, MK_ARRAY(TYPE_INT, 16))

STRUCT(sockaddr,
       TYPE_SHORT, MK_ARRAY(TYPE_CHAR, 14))

STRUCT(rtentry,
       TYPE_ULONG, MK_STRUCT(STRUCT_sockaddr), MK_STRUCT(STRUCT_sockaddr), MK_STRUCT(STRUCT_sockaddr),
       TYPE_SHORT, TYPE_SHORT, TYPE_ULONG, TYPE_PTRVOID, TYPE_SHORT, TYPE_PTRVOID,
       TYPE_ULONG, TYPE_ULONG, TYPE_SHORT)

STRUCT(ifmap,
       TYPE_ULONG, TYPE_ULONG, TYPE_SHORT, TYPE_CHAR, TYPE_CHAR, TYPE_CHAR,
       /* Spare 3 bytes */
       TYPE_CHAR, TYPE_CHAR, TYPE_CHAR)

/* The *_ifreq_list arrays deal with the fact that struct ifreq has unions */

STRUCT(sockaddr_ifreq,
       MK_ARRAY(TYPE_CHAR, IFNAMSIZ), MK_STRUCT(STRUCT_sockaddr))

STRUCT(short_ifreq,
       MK_ARRAY(TYPE_CHAR, IFNAMSIZ), TYPE_SHORT)

STRUCT(int_ifreq,
       MK_ARRAY(TYPE_CHAR, IFNAMSIZ), TYPE_INT)

STRUCT(ifmap_ifreq,
       MK_ARRAY(TYPE_CHAR, IFNAMSIZ), MK_STRUCT(STRUCT_ifmap))

STRUCT(char_ifreq,
       MK_ARRAY(TYPE_CHAR, IFNAMSIZ),
       MK_ARRAY(TYPE_CHAR, IFNAMSIZ))

STRUCT(ptr_ifreq,
       MK_ARRAY(TYPE_CHAR, IFNAMSIZ), TYPE_PTRVOID)

STRUCT(ifconf,
       TYPE_INT, TYPE_PTRVOID)

STRUCT(arpreq,
       MK_STRUCT(STRUCT_sockaddr), MK_STRUCT(STRUCT_sockaddr), TYPE_INT, MK_STRUCT(STRUCT_sockaddr),
       MK_ARRAY(TYPE_CHAR, 16))

STRUCT(arpreq_old,
       MK_STRUCT(STRUCT_sockaddr), MK_STRUCT(STRUCT_sockaddr), TYPE_INT, MK_STRUCT(STRUCT_sockaddr))

STRUCT(cdrom_read_audio,
       TYPE_CHAR, TYPE_CHAR, TYPE_CHAR, TYPE_CHAR, TYPE_CHAR, TYPE_INT, TYPE_PTRVOID,
       TYPE_NULL)

STRUCT(hd_geometry,
       TYPE_CHAR, TYPE_CHAR, TYPE_SHORT, TYPE_ULONG)

STRUCT(dirent,
       TYPE_LONG, TYPE_LONG, TYPE_SHORT, MK_ARRAY(TYPE_CHAR, 256))

STRUCT(kbentry,
       TYPE_CHAR, TYPE_CHAR, TYPE_SHORT)

STRUCT(kbsentry,
       TYPE_CHAR, MK_ARRAY(TYPE_CHAR, 512))

STRUCT(audio_buf_info,
       TYPE_INT, TYPE_INT, TYPE_INT, TYPE_INT)

STRUCT(count_info,
       TYPE_INT, TYPE_INT, TYPE_INT)

STRUCT(buffmem_desc,
       TYPE_PTRVOID, TYPE_INT)

STRUCT(mixer_info,
       MK_ARRAY(TYPE_CHAR, 16), MK_ARRAY(TYPE_CHAR, 32), TYPE_INT, MK_ARRAY(TYPE_INT, 10))

STRUCT(snd_timer_id,
       TYPE_INT, /* dev_class */
       TYPE_INT, /* dev_sclass */
       TYPE_INT, /* card */
       TYPE_INT, /* device */
       TYPE_INT) /* subdevice */

STRUCT(snd_timer_ginfo,
       MK_STRUCT(STRUCT_snd_timer_id), /* tid */
       TYPE_INT, /* flags */
       TYPE_INT, /* card */
       MK_ARRAY(TYPE_CHAR, 64), /* id */
       MK_ARRAY(TYPE_CHAR, 80), /* name */
       TYPE_ULONG, /* reserved0 */
       TYPE_ULONG, /* resolution */
       TYPE_ULONG, /* resolution_min */
       TYPE_ULONG, /* resolution_max */
       TYPE_INT, /* clients */
       MK_ARRAY(TYPE_CHAR, 32)) /* reserved */

STRUCT(snd_timer_gparams,
       MK_STRUCT(STRUCT_snd_timer_id), /* tid */
       TYPE_ULONG, /* period_num */
       TYPE_ULONG, /* period_den */
       MK_ARRAY(TYPE_CHAR, 32)) /* reserved */

STRUCT(snd_timer_gstatus,
       MK_STRUCT(STRUCT_snd_timer_id), /* tid */
       TYPE_ULONG, /* resolution */
       TYPE_ULONG, /* resolution_num */
       TYPE_ULONG, /* resolution_den */
       MK_ARRAY(TYPE_CHAR, 32)) /* reserved */

STRUCT(snd_timer_select,
       MK_STRUCT(STRUCT_snd_timer_id), /* id */
       MK_ARRAY(TYPE_CHAR, 32)) /* reserved */

STRUCT(snd_timer_info,
       TYPE_INT, /* flags */
       TYPE_INT, /* card */
       MK_ARRAY(TYPE_CHAR, 64), /* id */
       MK_ARRAY(TYPE_CHAR, 80), /* name */
       TYPE_ULONG, /* reserved0 */
       TYPE_ULONG, /* resolution */
       MK_ARRAY(TYPE_CHAR, 64)) /* reserved */

STRUCT(snd_timer_params,
       TYPE_INT, /* flags */
       TYPE_INT, /* ticks */
       TYPE_INT, /* queue_size */
       TYPE_INT, /* reserved0 */
       TYPE_INT, /* filter */
       MK_ARRAY(TYPE_CHAR, 60)) /* reserved */

#if defined(TARGET_SPARC64) && !defined(TARGET_ABI32)
STRUCT(timeval,
       TYPE_LONG, /* tv_sec */
       TYPE_INT) /* tv_usec */

STRUCT(_kernel_sock_timeval,
       TYPE_LONG, /* tv_sec */
       TYPE_INT) /* tv_usec */
#else
STRUCT(timeval,
       TYPE_LONG, /* tv_sec */
       TYPE_LONG) /* tv_usec */

STRUCT(_kernel_sock_timeval,
       TYPE_LONGLONG, /* tv_sec */
       TYPE_LONGLONG) /* tv_usec */
#endif

STRUCT(timespec,
       TYPE_LONG, /* tv_sec */
       TYPE_LONG) /* tv_nsec */

STRUCT(_kernel_timespec,
       TYPE_LONGLONG, /* tv_sec */
       TYPE_LONGLONG) /* tv_nsec */

STRUCT(snd_timer_status,
       MK_STRUCT(STRUCT_timespec), /* tstamp */
       TYPE_INT, /* resolution */
       TYPE_INT, /* lost */
       TYPE_INT, /* overrun */
       TYPE_INT, /* queue */
       MK_ARRAY(TYPE_CHAR, 64)) /* reserved */

/* loop device ioctls */
STRUCT(loop_info,
       TYPE_INT,                 /* lo_number */
       TYPE_OLDDEVT,             /* lo_device */
       TYPE_ULONG,               /* lo_inode */
       TYPE_OLDDEVT,             /* lo_rdevice */
       TYPE_INT,                 /* lo_offset */
       TYPE_INT,                 /* lo_encrypt_type */
       TYPE_INT,                 /* lo_encrypt_key_size */
       TYPE_INT,                 /* lo_flags */
       MK_ARRAY(TYPE_CHAR, 64),  /* lo_name */
       MK_ARRAY(TYPE_CHAR, 32),  /* lo_encrypt_key */
       MK_ARRAY(TYPE_ULONG, 2),  /* lo_init */
       MK_ARRAY(TYPE_CHAR, 4))   /* reserved */

STRUCT(loop_info64,
       TYPE_ULONGLONG,           /* lo_device */
       TYPE_ULONGLONG,           /* lo_inode */
       TYPE_ULONGLONG,           /* lo_rdevice */
       TYPE_ULONGLONG,           /* lo_offset */
       TYPE_ULONGLONG,           /* lo_sizelimit */
       TYPE_INT,                 /* lo_number */
       TYPE_INT,                 /* lo_encrypt_type */
       TYPE_INT,                 /* lo_encrypt_key_size */
       TYPE_INT,                 /* lo_flags */
       MK_ARRAY(TYPE_CHAR, 64),  /* lo_name */
       MK_ARRAY(TYPE_CHAR, 64),  /* lo_crypt_name */
       MK_ARRAY(TYPE_CHAR, 32),  /* lo_encrypt_key */
       MK_ARRAY(TYPE_ULONGLONG, 2))  /* lo_init */

/* mag tape ioctls */
STRUCT(mtop, TYPE_SHORT, TYPE_INT)
STRUCT(mtget, TYPE_LONG, TYPE_LONG, TYPE_LONG, TYPE_LONG, TYPE_LONG,
       TYPE_INT, TYPE_INT)
STRUCT(mtpos, TYPE_LONG)

STRUCT(fb_fix_screeninfo,
       MK_ARRAY(TYPE_CHAR, 16), /* id */
       TYPE_ULONG, /* smem_start */
       TYPE_INT, /* smem_len */
       TYPE_INT, /* type */
       TYPE_INT, /* type_aux */
       TYPE_INT, /* visual */
       TYPE_SHORT, /* xpanstep */
       TYPE_SHORT, /* ypanstep */
       TYPE_SHORT, /* ywrapstep */
       TYPE_INT, /* line_length */
       TYPE_ULONG, /* mmio_start */
       TYPE_INT, /* mmio_len */
       TYPE_INT, /* accel */
       MK_ARRAY(TYPE_CHAR, 3)) /* reserved */

STRUCT(fb_var_screeninfo,
       TYPE_INT, /* xres */
       TYPE_INT, /* yres */
       TYPE_INT, /* xres_virtual */
       TYPE_INT, /* yres_virtual */
       TYPE_INT, /* xoffset */
       TYPE_INT, /* yoffset */
       TYPE_INT, /* bits_per_pixel */
       TYPE_INT, /* grayscale */
       MK_ARRAY(TYPE_INT, 3), /* red */
       MK_ARRAY(TYPE_INT, 3), /* green */
       MK_ARRAY(TYPE_INT, 3), /* blue */
       MK_ARRAY(TYPE_INT, 3), /* transp */
       TYPE_INT, /* nonstd */
       TYPE_INT, /* activate */
       TYPE_INT, /* height */
       TYPE_INT, /* width */
       TYPE_INT, /* accel_flags */
       TYPE_INT, /* pixclock */
       TYPE_INT, /* left_margin */
       TYPE_INT, /* right_margin */
       TYPE_INT, /* upper_margin */
       TYPE_INT, /* lower_margin */
       TYPE_INT, /* hsync_len */
       TYPE_INT, /* vsync_len */
       TYPE_INT, /* sync */
       TYPE_INT, /* vmode */
       TYPE_INT, /* rotate */
       MK_ARRAY(TYPE_INT, 5)) /* reserved */

STRUCT(fb_cmap,
       TYPE_INT, /* start  */
       TYPE_INT, /* len    */
       TYPE_PTRVOID, /* red    */
       TYPE_PTRVOID, /* green  */
       TYPE_PTRVOID, /* blue   */
       TYPE_PTRVOID) /* transp */

STRUCT(fb_con2fbmap,
       TYPE_INT, /* console     */
       TYPE_INT) /* framebuffer */


STRUCT(vt_stat,
       TYPE_SHORT, /* v_active */
       TYPE_SHORT, /* v_signal */
       TYPE_SHORT) /* v_state */

STRUCT(vt_mode,
       TYPE_CHAR,  /* mode   */
       TYPE_CHAR,  /* waitv  */
       TYPE_SHORT, /* relsig */
       TYPE_SHORT, /* acqsig */
       TYPE_SHORT) /* frsig  */

STRUCT(dm_ioctl,
       MK_ARRAY(TYPE_INT, 3), /* version */
       TYPE_INT, /* data_size */
       TYPE_INT, /* data_start */
       TYPE_INT, /* target_count*/
       TYPE_INT, /* open_count */
       TYPE_INT, /* flags */
       TYPE_INT, /* event_nr */
       TYPE_INT, /* padding */
       TYPE_ULONGLONG, /* dev */
       MK_ARRAY(TYPE_CHAR, 128), /* name */
       MK_ARRAY(TYPE_CHAR, 129), /* uuid */
       MK_ARRAY(TYPE_CHAR, 7)) /* data */

STRUCT(dm_target_spec,
       TYPE_ULONGLONG, /* sector_start */
       TYPE_ULONGLONG, /* length */
       TYPE_INT, /* status */
       TYPE_INT, /* next */
       MK_ARRAY(TYPE_CHAR, 16)) /* target_type */

STRUCT(dm_target_deps,
       TYPE_INT, /* count */
       TYPE_INT) /* padding */

STRUCT(dm_name_list,
       TYPE_ULONGLONG, /* dev */
       TYPE_INT) /* next */

STRUCT(dm_target_versions,
       TYPE_INT, /* next */
       MK_ARRAY(TYPE_INT, 3)) /* version*/

STRUCT(dm_target_msg,
       TYPE_ULONGLONG) /* sector */

STRUCT(drm_version,
       TYPE_INT, /* version_major */
       TYPE_INT, /* version_minor */
       TYPE_INT, /* version_patchlevel */
       TYPE_ULONG, /* name_len */
       TYPE_PTRVOID, /* name */
       TYPE_ULONG, /* date_len */
       TYPE_PTRVOID, /* date */
       TYPE_ULONG, /* desc_len */
       TYPE_PTRVOID) /* desc */

STRUCT(file_clone_range,
       TYPE_LONGLONG, /* src_fd */
       TYPE_ULONGLONG, /* src_offset */
       TYPE_ULONGLONG, /* src_length */
       TYPE_ULONGLONG) /* dest_offset */

STRUCT(fiemap_extent,
       TYPE_ULONGLONG, /* fe_logical */
       TYPE_ULONGLONG, /* fe_physical */
       TYPE_ULONGLONG, /* fe_length */
       MK_ARRAY(TYPE_ULONGLONG, 2), /* fe_reserved64[2] */
       TYPE_INT, /* fe_flags */
       MK_ARRAY(TYPE_INT, 3)) /* fe_reserved[3] */

STRUCT(fiemap,
       TYPE_ULONGLONG, /* fm_start */
       TYPE_ULONGLONG, /* fm_length */
       TYPE_INT, /* fm_flags */
       TYPE_INT, /* fm_mapped_extents */
       TYPE_INT, /* fm_extent_count */
       TYPE_INT) /* fm_reserved */

STRUCT(blkpg_partition,
       TYPE_LONGLONG, /* start */
       TYPE_LONGLONG, /* length */
       TYPE_INT, /* pno */
       MK_ARRAY(TYPE_CHAR, BLKPG_DEVNAMELTH), /* devname */
       MK_ARRAY(TYPE_CHAR, BLKPG_VOLNAMELTH)) /* volname */

STRUCT(rtc_time,
       TYPE_INT, /* tm_sec */
       TYPE_INT, /* tm_min */
       TYPE_INT, /* tm_hour */
       TYPE_INT, /* tm_mday */
       TYPE_INT, /* tm_mon */
       TYPE_INT, /* tm_year */
       TYPE_INT, /* tm_wday */
       TYPE_INT, /* tm_yday */
       TYPE_INT) /* tm_isdst */

STRUCT(rtc_wkalrm,
       TYPE_CHAR, /* enabled */
       TYPE_CHAR, /* pending */
       MK_STRUCT(STRUCT_rtc_time)) /* time */

STRUCT(rtc_pll_info,
       TYPE_INT, /* pll_ctrl */
       TYPE_INT, /* pll_value */
       TYPE_INT, /* pll_max */
       TYPE_INT, /* pll_min */
       TYPE_INT, /* pll_posmult */
       TYPE_INT, /* pll_negmult */
       TYPE_LONG) /* pll_clock */

STRUCT(blkpg_ioctl_arg,
       TYPE_INT, /* op */
       TYPE_INT, /* flags */
       TYPE_INT, /* datalen */
       TYPE_PTRVOID) /* data */

STRUCT(format_descr,
       TYPE_INT,     /* device */
       TYPE_INT,     /* head */
       TYPE_INT)     /* track */

STRUCT(floppy_max_errors,
       TYPE_INT, /* abort */
       TYPE_INT, /* read_track */
       TYPE_INT, /* reset */
       TYPE_INT, /* recal */
       TYPE_INT) /* reporting */

#if defined(CONFIG_USBFS)
/* usb device ioctls */
STRUCT(usbdevfs_ctrltransfer,
        TYPE_CHAR, /* bRequestType */
        TYPE_CHAR, /* bRequest */
        TYPE_SHORT, /* wValue */
        TYPE_SHORT, /* wIndex */
        TYPE_SHORT, /* wLength */
        TYPE_INT, /* timeout */
        TYPE_PTRVOID) /* data */

STRUCT(usbdevfs_bulktransfer,
        TYPE_INT, /* ep */
        TYPE_INT, /* len */
        TYPE_INT, /* timeout */
        TYPE_PTRVOID) /* data */

STRUCT(usbdevfs_setinterface,
        TYPE_INT, /* interface */
        TYPE_INT) /* altsetting */

STRUCT(usbdevfs_disconnectsignal,
        TYPE_INT, /* signr */
        TYPE_PTRVOID) /* context */

STRUCT(usbdevfs_getdriver,
        TYPE_INT, /* interface */
        MK_ARRAY(TYPE_CHAR, USBDEVFS_MAXDRIVERNAME + 1)) /* driver */

STRUCT(usbdevfs_connectinfo,
        TYPE_INT, /* devnum */
        TYPE_CHAR) /* slow */

STRUCT(usbdevfs_iso_packet_desc,
        TYPE_INT, /* length */
        TYPE_INT, /* actual_length */
        TYPE_INT) /* status */

STRUCT(usbdevfs_urb,
        TYPE_CHAR, /* type */
        TYPE_CHAR, /* endpoint */
        TYPE_INT, /* status */
        TYPE_INT, /* flags */
        TYPE_PTRVOID, /* buffer */
        TYPE_INT, /* buffer_length */
        TYPE_INT, /* actual_length */
        TYPE_INT, /* start_frame */
        TYPE_INT, /* union number_of_packets stream_id */
        TYPE_INT, /* error_count */
        TYPE_INT, /* signr */
        TYPE_PTRVOID, /* usercontext */
        MK_ARRAY(MK_STRUCT(STRUCT_usbdevfs_iso_packet_desc), 0)) /* desc */

STRUCT(usbdevfs_ioctl,
        TYPE_INT, /* ifno */
        TYPE_INT, /* ioctl_code */
        TYPE_PTRVOID) /* data */

STRUCT(usbdevfs_hub_portinfo,
        TYPE_CHAR, /* nports */
        MK_ARRAY(TYPE_CHAR, 127)) /* port */

STRUCT(usbdevfs_disconnect_claim,
        TYPE_INT, /* interface */
        TYPE_INT, /* flags */
        MK_ARRAY(TYPE_CHAR, USBDEVFS_MAXDRIVERNAME + 1)) /* driver */
#endif /* CONFIG_USBFS */

/* ethtool ioctls */
STRUCT(ethtool_cmd,
       TYPE_INT,   /* cmd */
       TYPE_INT,   /* supported */
       TYPE_INT,   /* advertising */
       TYPE_SHORT, /* speed */
       TYPE_CHAR,  /* duplex */
       TYPE_CHAR,  /* port */
       TYPE_CHAR,  /* phy_address */
       TYPE_CHAR,  /* transceiver */
       TYPE_CHAR,  /* autoneg */
       TYPE_CHAR,  /* mdio_support */
       TYPE_INT,   /* maxtxpkt */
       TYPE_INT,   /* maxrxpkt */
       TYPE_SHORT, /* speed_hi */
       TYPE_CHAR,  /* eth_tp_mdix */
       TYPE_CHAR,  /* eth_tp_mdix_ctrl */
       TYPE_INT,   /* lp_advertising */
       MK_ARRAY(TYPE_INT, 2)) /* reserved */

STRUCT(ethtool_drvinfo,
       TYPE_INT, /* cmd */
       MK_ARRAY(TYPE_CHAR, 32), /* driver */
       MK_ARRAY(TYPE_CHAR, 32), /* version */
       MK_ARRAY(TYPE_CHAR, 32), /* fw_version[ETHTOOL_FWVERS_LEN] */
       MK_ARRAY(TYPE_CHAR, 32), /* bus_info[ETHTOOL_BUSINFO_LEN] */
       MK_ARRAY(TYPE_CHAR, 32), /* erom_version[ETHTOOL_EROMVERS_LEN] */
       MK_ARRAY(TYPE_CHAR, 12), /* reserved2 */
       TYPE_INT, /* n_priv_flags */
       TYPE_INT, /* n_stats */
       TYPE_INT, /* testinfo_len */
       TYPE_INT, /* eedump_len */
       TYPE_INT) /* regdump_len */

STRUCT(ethtool_regs,
       TYPE_INT, /* cmd */
       TYPE_INT, /* version */
       TYPE_INT, /* len */
       MK_FLEXIBLE_ARRAY(TYPE_CHAR, 2)) /* data[0]: len */

STRUCT(ethtool_wolinfo,
       TYPE_INT, /* cmd */
       TYPE_INT, /* supported */
       TYPE_INT, /* wolopts */
       MK_ARRAY(TYPE_CHAR,  6)) /* sopass[SOPASS_MAX] */

STRUCT(ethtool_value,
       TYPE_INT, /* cmd */
       TYPE_INT) /* data */

STRUCT(ethtool_eee,
       TYPE_INT, /* cmd */
       TYPE_INT, /* supported */
       TYPE_INT, /* advertised */
       TYPE_INT, /* lp_advertised */
       TYPE_INT, /* eee_active */
       TYPE_INT, /* eee_enabled */
       TYPE_INT, /* tx_lpi_enabled */
       TYPE_INT, /* tx_lpi_timer */
       MK_ARRAY(TYPE_INT, 2)) /* reserved */

STRUCT(ethtool_eeprom,
       TYPE_INT, /* cmd */
       TYPE_INT, /* magic */
       TYPE_INT, /* offset */
       TYPE_INT, /* len */
       MK_FLEXIBLE_ARRAY(TYPE_CHAR, 3)) /* data[0]: len */

STRUCT(ethtool_coalesce,
       TYPE_INT, /* cmd */
       TYPE_INT, /* rx_coalesce_usecs */
       TYPE_INT, /* rx_max_coalesced_frames */
       TYPE_INT, /* rx_coalesce_usecs_irq */
       TYPE_INT, /* rx_max_coalesced_frames_irq */
       TYPE_INT, /* tx_coalesce_usecs */
       TYPE_INT, /* tx_max_coalesced_frames */
       TYPE_INT, /* tx_coalesce_usecs_irq */
       TYPE_INT, /* tx_max_coalesced_frames_irq */
       TYPE_INT, /* stats_block_coalesce_usecs */
       TYPE_INT, /* use_adaptive_rx_coalesce */
       TYPE_INT, /* use_adaptive_tx_coalesce */
       TYPE_INT, /* pkt_rate_low */
       TYPE_INT, /* rx_coalesce_usecs_low */
       TYPE_INT, /* rx_max_coalesced_frames_low */
       TYPE_INT, /* tx_coalesce_usecs_low */
       TYPE_INT, /* tx_max_coalesced_frames_low */
       TYPE_INT, /* pkt_rate_high */
       TYPE_INT, /* rx_coalesce_usecs_high */
       TYPE_INT, /* rx_max_coalesced_frames_high */
       TYPE_INT, /* tx_coalesce_usecs_high */
       TYPE_INT, /* tx_max_coalesced_frames_high */
       TYPE_INT) /* rate_sample_interval */

STRUCT(ethtool_ringparam,
       TYPE_INT, /* cmd */
       TYPE_INT, /* rx_max_pending */
       TYPE_INT, /* rx_mini_max_pending */
       TYPE_INT, /* rx_jumbo_max_pending */
       TYPE_INT, /* tx_max_pending */
       TYPE_INT, /* rx_pending */
       TYPE_INT, /* rx_mini_pending */
       TYPE_INT, /* rx_jumbo_pending */
       TYPE_INT) /* tx_pending */

STRUCT(ethtool_pauseparam,
       TYPE_INT, /* cmd */
       TYPE_INT, /* autoneg */
       TYPE_INT, /* rx_pause */
       TYPE_INT) /* tx_pause */

STRUCT(ethtool_test,
       TYPE_INT, /* cmd */
       TYPE_INT, /* flags */
       TYPE_INT, /* reserved */
       TYPE_INT, /* len */
       MK_FLEXIBLE_ARRAY(TYPE_LONGLONG, 3)) /* data[0]: len */

STRUCT(ethtool_gstrings,
       TYPE_INT, /* cmd */
       TYPE_INT, /* string_set */
       TYPE_INT, /* len */
       MK_FLEXIBLE_ARRAY(MK_ARRAY(TYPE_CHAR, 32), 2))
       /* data[0]: len * ETH_GSTRING_LEN */

STRUCT(ethtool_stats,
       TYPE_INT, /* cmd */
       TYPE_INT, /* n_stats */
       MK_FLEXIBLE_ARRAY(TYPE_LONGLONG, 1)) /* data[0]: n_stats */

STRUCT(ethtool_perm_addr,
       TYPE_INT, /* cmd */
       TYPE_INT, /* size */
       MK_FLEXIBLE_ARRAY(TYPE_CHAR, 1)) /* data[0]: size */

STRUCT(ethtool_flow_ext,
       MK_ARRAY(TYPE_CHAR, 2), /* padding */
       MK_ARRAY(TYPE_CHAR, 6), /* h_dest[ETH_ALEN] */
       MK_ARRAY(TYPE_CHAR, 2), /* __be16 vlan_etype */
       MK_ARRAY(TYPE_CHAR, 2), /* __be16 vlan_tci */
       MK_ARRAY(TYPE_CHAR, 8)) /* __be32 data[2] */

/*
 * Union ethtool_flow_union contains alternatives that are either struct that
 * only uses __be* types or char/__u8, or "__u8 hdata[52]". We can treat it as
 * byte array in all cases.
 */
STRUCT(ethtool_rx_flow_spec,
       TYPE_INT,                           /* flow_type */
       MK_ARRAY(TYPE_CHAR, 52),            /* union ethtool_flow_union h_u */
       MK_STRUCT(STRUCT_ethtool_flow_ext), /* h_ext */
       MK_ARRAY(TYPE_CHAR, 52),            /* union ethtool_flow_union m_u */
       MK_STRUCT(STRUCT_ethtool_flow_ext), /* m_ext */
       TYPE_LONGLONG,                      /* ring_cookie */
       TYPE_INT)                           /* location */

STRUCT(ethtool_rxnfc_rss_context,
       TYPE_INT, /* cmd */
       TYPE_INT, /* flow_type */
       TYPE_LONGLONG, /* data */
       MK_STRUCT(STRUCT_ethtool_rx_flow_spec), /* fs */
       TYPE_INT) /* rss_context */

STRUCT(ethtool_rxnfc_rule_cnt,
       TYPE_INT, /* cmd */
       TYPE_INT, /* flow_type */
       TYPE_LONGLONG, /* data */
       MK_STRUCT(STRUCT_ethtool_rx_flow_spec), /* fs */
       TYPE_INT) /* rss_cnt */

STRUCT(ethtool_rxnfc_rule_locs,
       TYPE_INT, /* cmd */
       TYPE_INT, /* flow_type */
       TYPE_LONGLONG, /* data */
       MK_STRUCT(STRUCT_ethtool_rx_flow_spec), /* fs */
       TYPE_INT, /* rss_cnt */
       MK_FLEXIBLE_ARRAY(TYPE_INT, 4)) /* rule_locs[0]: rss_cnt */

/*
 * For ETHTOOL_{G,S}RXFH, originally only the first three fields are defined,
 * but with certain options, more fields are used.
 */
STRUCT_SPECIAL(ethtool_rxnfc_get_set_rxfh)

STRUCT(ethtool_flash,
       TYPE_INT, /* cmd */
       TYPE_INT, /* region */
       MK_ARRAY(TYPE_CHAR, 128)) /* data[ETHTOOL_FLASH_MAX_FILENAME] */

STRUCT_SPECIAL(ethtool_sset_info)

STRUCT(ethtool_rxfh_indir,
       TYPE_INT, /* cmd */
       TYPE_INT, /* size */
       MK_FLEXIBLE_ARRAY(TYPE_INT, 1)) /* ring_index[0]: size */

STRUCT_SPECIAL(ethtool_rxfh)

STRUCT(ethtool_get_features_block,
       TYPE_INT, /* available */
       TYPE_INT, /* requested */
       TYPE_INT, /* active */
       TYPE_INT) /* never_changed */

STRUCT(ethtool_gfeatures,
       TYPE_INT, /* cmd */
       TYPE_INT, /* size */
       MK_FLEXIBLE_ARRAY(MK_STRUCT(STRUCT_ethtool_get_features_block), 1))
       /* features[0]: size */

STRUCT(ethtool_set_features_block,
       TYPE_INT, /* valid */
       TYPE_INT) /* requested */

STRUCT(ethtool_sfeatures,
       TYPE_INT, /* cmd */
       TYPE_INT, /* size */
       MK_FLEXIBLE_ARRAY(MK_STRUCT(STRUCT_ethtool_set_features_block), 1))
       /* features[0]: size */

STRUCT(ethtool_channels,
       TYPE_INT, /* cmd */
       TYPE_INT, /* max_rx */
       TYPE_INT, /* max_tx */
       TYPE_INT, /* max_other */
       TYPE_INT, /* max_combined */
       TYPE_INT, /* rx_count */
       TYPE_INT, /* tx_count */
       TYPE_INT, /* other_count */
       TYPE_INT) /* combined_count */

/*
 * For ETHTOOL_SET_DUMP and ETHTOOL_GET_DUMP_FLAG, the flexible array `data` is
 * not used.
 */
STRUCT(ethtool_dump_no_data,
       TYPE_INT, /* cmd */
       TYPE_INT, /* version */
       TYPE_INT, /* flag */
       TYPE_INT) /* len */

STRUCT(ethtool_dump,
       TYPE_INT, /* cmd */
       TYPE_INT, /* version */
       TYPE_INT, /* flag */
       TYPE_INT, /* len */
       MK_FLEXIBLE_ARRAY(TYPE_CHAR, 3)) /* data[0]: len */

STRUCT(ethtool_ts_info,
       TYPE_INT, /* cmd */
       TYPE_INT, /* so_timestamping */
       TYPE_INT, /* phc_index */
       TYPE_INT, /* tx_types */
       MK_ARRAY(TYPE_INT, 3), /* tx_reserved */
       TYPE_INT, /* rx_filters */
       MK_ARRAY(TYPE_INT, 3)) /* rx_reserved */

STRUCT(ethtool_modinfo,
       TYPE_INT, /* cmd */
       TYPE_INT, /* type */
       TYPE_INT, /* eeprom_len */
       MK_ARRAY(TYPE_INT, 8)) /* reserved */

STRUCT(ethtool_tunable,
       TYPE_INT, /* cmd */
       TYPE_INT, /* id */
       TYPE_INT, /* type_id */
       TYPE_INT, /* len */
       MK_FLEXIBLE_ARRAY(TYPE_PTRVOID, 3)) /* data[0]: len */

STRUCT_SPECIAL(ethtool_link_settings)

STRUCT(ethtool_fecparam,
       TYPE_INT, /* cmd */
       TYPE_INT, /* active_fec */
       TYPE_INT, /* fec */
       TYPE_INT) /* reserved */

STRUCT_SPECIAL(ethtool_per_queue_op)
