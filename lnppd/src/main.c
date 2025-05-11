#include "dutil.h"
#include "network.h"
#include <sys/syslog.h>

int main(void)
{
	if (lnpp_daemonize() < 0)
		return 0;

	if (lnppd_start_network_services() < 0) {
		syslog(LOG_ERR, "Failed to start LNPPD network services. Shutting down ...");
		return 1;
	}

	lnpp_halt_while_running();

	return 0;
}
