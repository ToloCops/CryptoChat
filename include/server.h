#ifndef SERVER_H
#define SERVER_H

#define MAX_BLOCKS 8192  // max 64KB

int run_server(int num_threads, const char* file_prefix, int port);

#endif
