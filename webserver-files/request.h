#ifndef __REQUEST_H__
#include "worker.h"

void requestHandle(int fd, WorkerThread* worker, struct timeval  arrival_time, struct timeval dispatch_time); 

#endif
