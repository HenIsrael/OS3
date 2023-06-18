#ifndef _REQUEST_HANDLER_H
#define _REQUEST_HANDLER_H

#include "queue.h"
#include "request_obj.h"

typedef struct request_manager_t{
    List runningRequests;
    List waitingRequestsQueue;

    int maxAcceptedRequests;

}*RequestsHandler;

RequestsHandler createRequestsHandler(int maxAcceptedRequests);
int countWaitingQueue(RequestsHandler RequestsHandler);
int waitingQueueIsEmpty(RequestsHandler RequestsHandler);
int runningQueueIsEmpty(RequestsHandler RequestsHandler);
int requestsHandlerCanAcceptRequests(RequestsHandler RequestsHandler);
int requestsHandlerHasReachedItMaxRequests(RequestsHandler RequestsHandler, int max_size);


void addReadyRequest(RequestsHandler RequestsHandler, Request request);
Request getReadyRequest(RequestsHandler RequestsHandler);
void addPendingRequest(RequestsHandler RequestsHandler, Request request);
void enlargeMaxAcceptedRequests(RequestsHandler RequestsHandler);


void removeFinishedRequest(RequestsHandler RequestsHandler, Request request);
Request removeWaitingRequestAt(RequestsHandler RequestsHandler, int index);
Request removeRandWaitingRequest(RequestsHandler RequestsHandler);
int removeOldestWaitingRequest(RequestsHandler RequestsHandler);

void requestHandlerDelete(RequestsHandler RequestsHandler);

#endif //_REQUEST_HANDLER_H
