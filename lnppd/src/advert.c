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

static void *_lnppd_advert_loop(void *junk)
{
	int sockd = glob_state.advert_sockd;
	struct sockaddr_in *addr = &glob_state.advert_addr;

	struct timespec t;
	t.tv_nsec = 0;
	t.tv_sec = 5;

	struct lnpp_packet packet;
	lnpp_packet_advert(&packet, "Dona eis requiem", LNPPD_ADVERT_PORT, LNPPD_STREAM_PORT);

	while (glob_state.running) {
		nanosleep(&t, NULL);
		sendto(sockd, &packet, sizeof(struct lnpp_packet), 0, (struct sockaddr *) addr,
				(socklen_t) sizeof(struct sockaddr_in));
	}

	syslog(LOG_INFO, "LNPPD Advertiser thread received shutdown request.");
	close(sockd);
	return NULL;
}

int lnppd_start_advertiser()
{
	int sockd;
	int broadcast = 1;
	struct sockaddr_in *addr = &glob_state.advert_addr;

	if ((sockd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		syslog(LOG_ERR, "Could not create LNPP UDP socket: %s\n", strerror(errno));
		return -1;
	}

	if (setsockopt(sockd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
		syslog(LOG_ERR, "Could not enable broadcasts on UDP socket: %s\n", strerror(errno));
		close(sockd);
		return -1;
	}

	addr->sin_family = AF_INET;
	addr->sin_port = htons(LNPPD_ADVERT_PORT);
	addr->sin_addr.s_addr = inet_addr("255.255.255.255");

	glob_state.advert_sockd = sockd;

	if (pthread_create(&glob_state.advert_thread, NULL, (void *) _lnppd_advert_loop, NULL) != 0) {
		syslog(LOG_ERR, "Could not create advertiser thread");
		close(sockd);
		return -1;
	}

	return 0;
}
