#if !(defined(__APPLE__) && defined(__MACH__))
	#define htonll(ll) (((uint64_t) htonl((ll) >> 32)) | ((uint64_t) htonl((ll) & 0xFFFFFFFF)))
#endif
