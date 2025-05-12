#ifndef LNPPD_NET
#define LNPPD_NET

int lnppd_start_network_services();

int lnppd_start_advertiser();

/**
 * Always start job server after the advertiser.
 **/
int lnppd_start_job_server();

void lnppd_terminate_network_services();

#endif
