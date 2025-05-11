#include "server.h"
#include "netcfg.h"
#include "state.h"
#include "packet.h"

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <syslog.h>
#include <arpa/inet.h>
#include <pthread.h>

struct lnppd_adv_data {
	struct sockaddr_in addr;
	int sockd;
};

static void *_lnppd_advert_loop(struct lnppd_adv_data *data)
{
	time_t last_adv = time(NULL);
	time_t now;

	struct lnpp_packet packet;
	lnpp_packet_advert(&packet, "Dona eis requiem");

	while (glob_state.running) {
		now = time(NULL);

		if (now - last_adv < 5) {
			usleep(100000);
			continue;
		}

		sendto(data->sockd, &packet, sizeof(struct lnpp_packet), 0, (struct sockaddr *) &data->addr,
				(socklen_t) sizeof(struct sockaddr_in));
	}

	syslog(LOG_INFO, "LNPPD Advertiser thread received shutdown request.");
	close(data->sockd);
	return NULL;
}

int lnppd_start_advertiser()
{
	int sockd;
	struct sockaddr_in addr = {0};
	int broadcast = 1;

	if ((sockd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		syslog(LOG_ERR, "Could not create LNPP UDP socket: %s\n", strerror(errno));
		return -1;
	}

	if (setsockopt(sockd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
		syslog(LOG_ERR, "Could not enable broadcasts on UDP socket: %s\n", strerror(errno));
		close(sockd);
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(LNPPD_ADVERT_PORT);
	addr.sin_addr.s_addr = inet_addr("255.255.255.255");

	pthread_t t;

	struct lnppd_adv_data data  = {
		.addr = addr,
		.sockd = sockd
	};

	if (pthread_create(&t, NULL, (void *) _lnppd_advert_loop, &data) != 0) {
		syslog(LOG_ERR, "Could not create advertiser thread");
		close(sockd);
		return -1;
	}

	while (glob_state.running);

	pthread_join(t, NULL);

	syslog(LOG_INFO, "LNPPD Server shutting down");

	return 0;
}
