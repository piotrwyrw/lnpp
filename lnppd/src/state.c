#include "state.h"
#include <pthread.h>

struct lnppd_state glob_state = {
	.running = true,
	.jobs_ok = false,
	.advert_ok = false,
	.active_handlers = {false},
	.handler_mutex = PTHREAD_MUTEX_INITIALIZER
};
