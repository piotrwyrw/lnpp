#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/_types/_pid_t.h>
#include <sys/syslog.h>
#include <unistd.h>
#include <syslog.h>
#include "state.h"

static int _lnppd_daemonize() {
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

		return 0;
	}

	if (pid < 0) {
		printf("Could not create LNPP daemon: %s\n", strerror(errno));
		return -1;
	}

	printf("LNPP Server daemon is up and running PID %d\n", pid);
	return -1;
}

static void _lnppd_close(int sig)
{
	syslog(LOG_INFO, "LNPPD Daemon is gracefully terminating in response to SIGTERM.");
	closelog();
	glob_state.running = false;
}

int main(void)
{
	if (_lnppd_daemonize() < 0)
		return 1;

	close(STDERR_FILENO);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);

	signal(SIGTERM, _lnppd_close);
	openlog("LNPP", LOG_PID | LOG_NDELAY | LOG_CONS, LOG_DAEMON);

	syslog(LOG_INFO, "LNPPD Daemon is up and running");

	while (glob_state.running) {
	}

	return 0;
}
