#include "state.h"

struct lnppd_state glob_state = {
	.running = true,
	.jobs_ok = false,
	.advert_ok = false
};
