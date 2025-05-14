#ifndef LNPPD_STATE
#define LNPPD_STATE

#include <netinet/in.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/_pthread/_pthread_mutex_t.h>

#define JOBS_MAX_THREADS 20

struct lnppd_state {
	_Bool running;

	int advert_sockd;
	struct sockaddr_in advert_addr;
	pthread_t advert_thread;
	_Bool advert_ok;

	int jobs_sockd;
	struct sockaddr_in jobs_addr;
	_Bool jobs_ok;

	pthread_t job_handlers[JOBS_MAX_THREADS];
	_Bool active_handlers[JOBS_MAX_THREADS];
	pthread_mutex_t handler_mutex;
};

extern struct lnppd_state glob_state;

#endif
