
#ifndef __SCO_H
#define __SCO_H

#ifdef __cplusplus
extern "C" {
#endif

/* SCO defaults */
#define SCO_DEFAULT_MTU		500
#define SCO_DEFAULT_FLUSH_TO	0xFFFF

#define SCO_CONN_TIMEOUT	(HZ * 40)
#define SCO_DISCONN_TIMEOUT	(HZ * 2)
#define SCO_CONN_IDLE_TIMEOUT	(HZ * 60)

/* SCO socket address */
struct sockaddr_sco {
	sa_family_t	sco_family;
	bdaddr_t	sco_bdaddr;
	/* for use with eSCO masks such as SCO_ESCO_MASK */
	uint16_t	sco_pkt_type;
};

/* set/get sockopt defines */
#define SCO_OPTIONS	0x01
struct sco_options {
	uint16_t	mtu;
};

#define SCO_CONNINFO	0x02
struct sco_conninfo {
	uint16_t	hci_handle;
	uint8_t		dev_class[3];
};

#ifdef __cplusplus
}
#endif

#endif /* __SCO_H */