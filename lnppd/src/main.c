#include "dutil.h"
#include "server.h"

int main(void)
{
	if (lnpp_daemonize() < 0)
		return 0;

	lnppd_start_advertiser();

	return 0;
}
