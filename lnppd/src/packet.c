#include "packet.h"

#include <_string.h>
#include <string.h>

void lnpp_packet_advert(struct lnpp_packet *dst, char *name)
{
	dst->type = LNPP_ADVERTISEMENT;
	strlcpy(dst->advert.name, name, PKT_ADV_LEN - 1);
}

void lnpp_packet_ping(struct lnpp_packet *dst, time_t time)
{
	dst->type = LNPP_PING;
	dst->ping.time = time;
}
