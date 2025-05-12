#ifndef LNPP_PACKET
#define LNPP_PACKET

#include <stdint.h>
#include <time.h>

#define PACKED(id) __attribute__((packed)) id

/**
 * Pre-determined length for all fixed-size strings passed along in the packets,
 * e.g. Advert node names, job authors, etc. -- This lets us avoid malloc()-ing
 * memory all the time and instead rely on the stack, which is usually faster.
 **/
#define PKT_STR_LEN 100

enum lnpp_pktype {
	LNPP_ADVERTISEMENT = 0,
	LNPP_JOB
};

struct PACKED(lnpp_advert) {
	uint16_t udp_port;
	uint16_t tcp_port;
	char name[PKT_STR_LEN];
	char description[PKT_STR_LEN];
};

struct PACKED(lnpp_job) {
	uint64_t size;
	char username[PKT_STR_LEN];
	void *data;
};

struct PACKED(lnpp_packet) {
	enum lnpp_pktype type;
	union {
		struct lnpp_advert advert;
		struct lnpp_job job;
	};
};

void lnpp_packet_advert(struct lnpp_packet *dst, char *name, uint16_t udp_port, uint16_t tcp_port);
void lnpp_packet_ping(struct lnpp_packet *dst, time_t time);
void lnpp_packet_hton(struct lnpp_packet *packet);

#endif
