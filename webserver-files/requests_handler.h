#ifndef _REQUEST_HANDLER_H
#define _REQUEST_HANDLER_H

#include "queue.h"
#include "request_obj.h"

typedef struct request_manager_t{
    List runningRequests;
    List waitingRequestsQueue;

    int maxAcceptedRequests;

}*RequestManager;

RequestManager createRequestsHandler(int maxAcceptedRequests);
int countWaitingQueue(RequestManager requestManager);
int waitingQueueIsEmpty(RequestManager requestManager);
int runningQueueIsEmpty(RequestManager requestManager);
int requestsHandlerCanAcceptRequests(RequestManager requestManager);
int requestsHandlerHasReachedItMaxRequests(RequestManager requestManager, int max_size);

void addReadyRequest(RequestManager requestManager, Request requestObject);
Request getReadyRequest(RequestManager requestManager);
void addPendingRequest(RequestManager requestManager, Request requestObject);
void enlargeMaxAcceptedRequests(RequestManager requestManager);


void removeFinishedRequest(RequestManager requestManager, Request requestObject);
Request removeWaitingRequestAt(RequestManager requestManager, int index);
Request removeRandWaitingRequest(RequestManager requestManager);
int removeOldestWaitingRequest(RequestManager requestManager);

void requestHandlerDelete(RequestManager requestManager);

#endif //_REQUEST_HANDLER_H
