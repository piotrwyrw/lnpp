#ifndef LNPP_PACKET
#define LNPP_PACKET

#include <stdint.h>
#include <time.h>

#define PACKED(id) __attribute__((packed)) id

#define PKT_ADV_LEN 100

enum lnpp_pktype {
	LNPP_PING = 0,
	LNPP_ADVERTISEMENT
};

struct PACKED(lnpp_advert) {
	char name[PKT_ADV_LEN];
};

struct PACKED(lnpp_ping) {
	time_t time;
};

struct PACKED(lnpp_packet) {
	enum lnpp_pktype type;
	union {
		struct lnpp_advert advert;
		struct lnpp_ping ping;
	};
};

void lnpp_packet_advert(struct lnpp_packet *dst, char *name);
void lnpp_packet_ping(struct lnpp_packet *dst, time_t time);

#endif
