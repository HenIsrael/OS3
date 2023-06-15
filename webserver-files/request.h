#ifndef __REQUEST_H__

void requestHandle(int fd, WorkerThread* worker, struct timeval  arrival_time, struct timeval dispatch_time); 

#endif
