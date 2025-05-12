#include "dutil.h"
#include "state.h"
#include "network.h"

#include <sys/syslog.h>
#include <syslog.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

int lnpp_daemonize()
{
	pid_t pid = fork();

	if (pid == 0) {
		if (setsid() < 0) {
			printf("Could not create a new session: %s\n", strerror(errno));
			return -1;
		}

		if (chdir("/") < 0) {
			printf("Could not change directory to root: %s\n", strerror(errno));
			return -1;
		}

		close(STDERR_FILENO);
		close(STDIN_FILENO);
		close(STDOUT_FILENO);

		signal(SIGTERM, lnpp_sigterm);
		openlog("LNPP", LOG_PID | LOG_NDELAY | LOG_CONS, LOG_DAEMON);

		return 0;
	}

	if (pid < 0) {
		printf("Could not create LNPP daemon: %s\n", strerror(errno));
		return -1;
	}

	printf("LNPP Server daemon is up and running PID %d\n", pid);
	return -1;
}

void lnpp_sigterm(int sig)
{
	syslog(LOG_INFO, "Initiating graceful daemon shutdown ...");
	glob_state.running = false;
	lnppd_terminate_network_services();
	closelog();
}
