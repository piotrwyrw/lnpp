#include "netcfg.h"
#include "state.h"

#include <_time.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/syslog.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

struct lnppd_handler_ctx {
	int sockd;
	int handler_id;
};

// Find the next free ID for a handler, or -1 if not available
static int _lnppd_next_id()
{
	for (int i = 0; i < JOBS_MAX_THREADS; i ++) {
		if (glob_state.active_handlers[i])
			continue;
		return i;
	}
	return -1;
}

// Close the handled connection and set the handler to inactive
static void _lnppd_handler_exit(struct lnppd_handler_ctx *ctx)
{
	pthread_mutex_lock(&glob_state.handler_mutex);
	close(ctx->sockd);
	glob_state.active_handlers[ctx->handler_id] = false;
	pthread_mutex_unlock(&glob_state.handler_mutex);
}

static void *_lnppd_job_handler(struct lnppd_handler_ctx *ctx)
{
	// Wait until the handler thread is done working on the mutex data,
	// the unlock immediately.
	pthread_mutex_lock(&glob_state.handler_mutex);
	pthread_mutex_unlock(&glob_state.handler_mutex);

	uint64_t size;
	if (recv(ctx->sockd, &size, sizeof(uint64_t), 0) != sizeof(uint64_t)) {
		syslog(LOG_WARNING, "Received invalid job packet.");
		goto exit_handler;
	}

	syslog(LOG_INFO, "Received Job: Handler ID: %d, Size: %lld.", ctx->handler_id, size);

exit_handler:
	_lnppd_handler_exit(ctx);
	return NULL;
}

static int _lnppd_start_job_handler(struct lnppd_handler_ctx *ctx, pthread_t *thread_id)
{
	if (pthread_create(thread_id, NULL, (void *) _lnppd_job_handler, ctx) != 0) {
		syslog(LOG_WARNING, "Could not start thread for job handler.");
		return -1;
	}
	return 0;
}

int lnppd_start_job_server()
{
	int sockd;
	struct sockaddr_in *addr = &glob_state.jobs_addr;
	_Bool err = false;

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
		err = true;
		goto end;
	}

	if (fcntl(sockd, F_SETFL, O_NONBLOCK) < 0) {
		syslog(LOG_ERR, "Could not set O_NONBLOCK on server socket: %s\n", strerror(errno));
		err = true;
		goto end;
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
		err = false;
		goto end;
	}

	syslog(LOG_INFO, "Job server is ready to accept connections on port %d.\n", LNPPD_STREAM_PORT);

	while (glob_state.running) {
		inb_sockd = accept(sockd, (struct sockaddr *) &inb_addr, &inb_len);

		if (inb_sockd < 0 && errno != EWOULDBLOCK) {
			syslog(LOG_ERR, "Could not accept() incoming connection: %s\n", strerror(errno));
			err = true;
			break;
		}

		if (inb_sockd) {
			int id;
			if (pthread_mutex_lock(&glob_state.handler_mutex)) {
				syslog(LOG_WARNING, "Could not lock handler mutex. This should not happen. Rejecting job.");
				close(inb_sockd);
				goto loop;
			}
			if ((id = _lnppd_next_id()) < 0) {
				syslog(LOG_WARNING, "Unable to allocate a job handler for an incoming request.");
				close(inb_sockd);
				goto loop;
			}
			struct lnppd_handler_ctx *ctx = malloc(sizeof(struct lnppd_handler_ctx));

			if (!ctx) {
				syslog(LOG_WARNING, "Could not malloc() a handler context.");
				goto loop;
			}

			ctx->handler_id = id;
			glob_state.active_handlers[id] = true;

			if (_lnppd_start_job_handler(ctx, &glob_state.job_handlers[id]) < 0) {
				syslog(LOG_WARNING, "Could not start handler thread for an incoming job request. Rejecting.");
				close(inb_sockd);
				glob_state.active_handlers[id] = false;
			}

			if (pthread_mutex_unlock(&glob_state.handler_mutex)) {
				syslog(LOG_WARNING, "Failed to unlock handler setup mutex. This should absolutely not happen. Rejecting job.");

				// We don't really care about clenaup at this point. If we failed to unlock the mutex
				// for whatever reason, the thing is stuck indefinitely; We're fucked regardless.
				// ... but let's do it anyway.
				pthread_cancel(glob_state.job_handlers[id]);
				pthread_join(glob_state.job_handlers[id], NULL);
				glob_state.active_handlers[id] = false;
				close(inb_sockd);
				goto loop;
			}
		}

loop:
		nanosleep(&delay, NULL);
	}

end:

	if (err) {
		syslog(LOG_INFO, "Job server shutting down due to an error ...");
	} else {
		syslog(LOG_INFO, "Job server shutting down ...");
	}

	// Wait for all remaining job handlers to exit
	for (unsigned int i = 0; i < JOBS_MAX_THREADS; i ++) {
		if (glob_state.active_handlers[i]) {
			pthread_join(glob_state.job_handlers[i], NULL);
			glob_state.active_handlers[i] = false;
		}
	}

	if (!err) {
		return 0;
	}

	close(sockd);
	return -1;
}
