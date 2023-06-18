#ifndef _REQUEST_MANAGER_H
#define _REQUEST_MANAGER_H

#include "queue.h"
#include "request_obj.h"

typedef struct request_manager_t{
    List runningRequests;
    List waitingRequestsQueue;

    int maxAcceptedRequests;
    int threadsNum;//todo: remove

}*RequestManager;

RequestManager requestManagerCreate(int threadsNum, int maxAcceptedRequests);
int requestManagerGetWaitingQueueSize(RequestManager requestManager);
int requestManagerHasWaitingRequests(RequestManager requestManager);
int requestManagerHasRunningRequests(RequestManager requestManager);
int requestManagerCanAcceptRequests(RequestManager requestManager);
int requestManagerHasReachedItMaxRequests(RequestManager requestManager, int max_size);

RequestObject requestManagerGetReadyRequest(RequestManager requestManager);//todo: return status instead

void requestManagerAddPendingRequest(RequestManager requestManager, RequestObject requestObject);//todo: return status instead
void requestManagerAddReadyRequest(RequestManager requestManager, RequestObject requestObject);//todo: return status instead
void requestManagerRemoveFinishedRequest(RequestManager requestManager, RequestObject requestObject);//todo: return status instead


RequestObject requestManagerRemoveRequestFromWaitingQueueAtIndex(RequestManager requestManager, int index);//todo: return status instead
RequestObject requestManagerRemovRandRequestFromWaitingQueue(RequestManager requestManager);

int requestManagerRemoveOldestRequestFromWaitingQueue(RequestManager requestManager);//todo: return status instead

void requestManagerEnlargeMaxAcceptedRequests(RequestManager requestManager);
void requestManagerDelete(RequestManager requestManager);

#endif //_REQUEST_MANAGER_H
