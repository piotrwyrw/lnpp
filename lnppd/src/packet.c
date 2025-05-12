#include "packet.h"
#include "netutil.h"

#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

static void _lnpp_packet_prime(struct lnpp_packet *dst, enum lnpp_pktype type)
{
	memset(dst, 0, sizeof(struct lnpp_packet));
	dst->type = type;
}

void lnpp_packet_advert(struct lnpp_packet *dst, char *name, uint16_t udp_port, uint16_t tcp_port)
{
	_lnpp_packet_prime(dst, LNPP_ADVERTISEMENT);
	dst->advert.udp_port = udp_port;
	dst->advert.tcp_port = tcp_port;
	strlcpy(dst->advert.name, name, PKT_STR_LEN - 1);
}

void lnpp_packet_job(struct lnpp_packet *dst, uint64_t size, char *user, void *data)
{
	_lnpp_packet_prime(dst, LNPP_JOB);
	dst->job.size = size;
	strlcpy(dst->job.username, user, PKT_STR_LEN - 1);
	dst->job.data = data;
}

void lnpp_packet_hton(struct lnpp_packet *packet)
{
	switch (packet->type) {
		case LNPP_ADVERTISEMENT:
			packet->advert.tcp_port = htons(packet->advert.tcp_port);
			packet->advert.udp_port = htons(packet->advert.udp_port);
			break;

		case LNPP_JOB:
			packet->job.size = htonll(packet->job.size);
			break;
	}
}
