#include "packet.h"

#include <string.h>

static void _lnpp_packet_prime(struct lnpp_packet *dst, enum lnpp_pktype type)
{
	memset(dst, 0, sizeof(struct lnpp_packet));
	dst->type = type;
}

void lnpp_packet_advert(struct lnpp_packet *dst, char *name)
{
	_lnpp_packet_prime(dst, LNPP_ADVERTISEMENT);
	strlcpy(dst->advert.name, name, PKT_ADV_LEN - 1);
}

void lnpp_packet_ping(struct lnpp_packet *dst, time_t time)
{
	_lnpp_packet_prime(dst, LNPP_PING);
	dst->ping.time = time;
}
