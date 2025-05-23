#include "dutil.h"
#include "network.h"
#include <sys/syslog.h>

int main(void)
{
	if (lnpp_daemonize() < 0)
		return 0;

	if (lnppd_start_network_services() < 0) {
		syslog(LOG_ERR, "A network service error occurred.");
		lnppd_terminate_network_services();
		return 1;
	}

	syslog(LOG_INFO, "Good night.");

	closelog();

	return 0;
}
