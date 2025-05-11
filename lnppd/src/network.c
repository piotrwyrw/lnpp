#include "network.h"
#include "server.h"
#include <sys/syslog.h>

int lnppd_start_network_services()
{
	if (lnppd_start_advertiser() < 0) {
		syslog(LOG_ERR, "Failed to start LNPPD avertiser.");
		return -1;
	}

	return 0;
}
