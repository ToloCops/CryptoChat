#ifndef SIGNAL_UTILS_H
#define SIGNAL_UTILS_H

#include <signal.h>

sigset_t get_blocking_signal_set();  // Returns a signal set that blocks SIGINT, SIGTERM, SIGUSR1, SIGUSR2, and SIGALRM

#endif
