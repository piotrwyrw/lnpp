#include "netcfg.h"
#include "state.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/syslog.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

int lnppd_start_job_server()
{
	int sockd;
	struct sockaddr_in *addr;

	if ((sockd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		syslog(LOG_ERR, "Could not create socket for job server: %s\n", strerror(errno));
		return -1;
	}

	addr->sin_family = AF_INET;
	addr->sin_port = htons(LNPPD_STREAM_PORT);
	addr->sin_addr.s_addr = inet_addr("127.0.0.1");

	glob_state.jobs_sockd = sockd;

	if (bind(sockd, (struct sockaddr *) addr, (socklen_t) sizeof(struct sockaddr_in))) {
		syslog(LOG_ERR, "Could not bind() job server socket: %s\n", strerror(errno));
		goto fail;
	}

	if (fcntl(sockd, F_SETFL, O_NONBLOCK) < 0) {
		syslog(LOG_ERR, "Could not set O_NONBLOCK on server socket: %s\n", strerror(errno));
		goto fail;
	}

	struct sockaddr_in inb_addr;
	socklen_t inb_len;
	int inb_sockd;

	// Suspend execution for 200ms every iteration to reduce CPU usage
	struct timespec delay;
	delay.tv_nsec = 200000000;
	delay.tv_sec = 0;

	if (listen(sockd, LNPPD_BACKLOG) < 0) {
		syslog(LOG_ERR, "The job server could not listen() for incomming connections: %s\n", strerror(errno));
		goto fail;
	}

	syslog(LOG_INFO, "LNPPD Job server is ready to accept connections on port %d.\n", LNPPD_STREAM_PORT);


	while (glob_state.running) {
		inb_sockd = accept(sockd, (struct sockaddr *) &inb_addr, &inb_len);
		if (inb_sockd < 0 && errno != EWOULDBLOCK) {
			syslog(LOG_ERR, "Could not accept() incoming connection: %s\n", strerror(errno));
			goto fail;
		}

		nanosleep(&delay, NULL);
	}

	syslog(LOG_INFO, "LNPPD Job server shutting down ...");

	return 0;

fail:
	close(sockd);
	return -1;
}
