#include "network.h"
#include "state.h"

#include <sys/syslog.h>
#include <pthread.h>

int lnppd_start_network_services()
{
	if (lnppd_start_advertiser() < 0) {
		syslog(LOG_ERR, "Failed to start LNPPD avertiser.");
		return -1;
	}

	if (lnppd_start_job_server() < 0) {
		syslog(LOG_ERR, "Failed to start LNPPD job server.");
		return -1;
	}

	return 0;
}

void lnppd_terminate_network_services()
{
	syslog(LOG_INFO, "Terminating all network services ...");

	if (glob_state.advert_ok) {
		pthread_join(glob_state.advert_thread, NULL);
		glob_state.advert_ok = false;
	}
}
