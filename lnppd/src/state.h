#ifndef LNPPD_STATE
#define LNPPD_STATE

#include <stdbool.h>
#include <pthread.h>

struct lnppd_state {
	_Bool running;
};

extern struct lnppd_state glob_state;

#endif
