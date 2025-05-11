#ifndef LNPPD_STATE
#define LNPPD_STATE

#include <netinet/in.h>
#include <stdbool.h>
#include <pthread.h>

struct lnppd_state {
	_Bool running;

	int advert_sockd;
	struct sockaddr_in advert_addr;
	pthread_t advert_thread;
};

extern struct lnppd_state glob_state;

#endif
