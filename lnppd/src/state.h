#ifndef LNPP_STATE
#define LNPP_STATE

#include <stdbool.h>

struct lnpp_state {
	_Bool running;
};

extern struct lnpp_state glob_state;

#endif
