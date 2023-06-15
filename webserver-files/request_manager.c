#include "request_manager.h"
#include "stdlib.h"

void *copyData(void *data)
{
    return copyRequestObject((RequestObject)data);
}

int compareData(Data data1, Data data2){
    return compareRequestObject((RequestObject)data1, (RequestObject)data2);
}

void freeData(void *data)
{
    deleteRequestObject((RequestObject)data);
}

void printData(void*data){
    RequestObject requestObject = (RequestObject)data;
    printf("|request id = %d| --> ",requestObject->val);
}

RequestManager requestManagerCreate(int threadsNum, int maxAcceptedRequests){
    RequestManager requestManager = malloc(sizeof (*requestManager));
    requestManager->runningRequests = listCreate(copyData, compareData, freeData, printData);
    if(requestManager->runningRequests == NULL){
        free(requestManager);
        return NULL;
    }
    requestManager->waitingRequestsQueue = listCreate(copyData, compareData, freeData, printData);
    if(requestManager->waitingRequestsQueue == NULL){
        listDelete(requestManager->runningRequests);
        free(requestManager);
        return NULL;
    }
    requestManager->threadsNum = threadsNum;
    requestManager->maxAcceptedRequests = maxAcceptedRequests;
    return requestManager;
}

int requestManagerGetWaitingQueueSize(RequestManager requestManager){
    int size = listGetSize(requestManager->waitingRequestsQueue);
    return size;
}

int requestManagerHasWaitingRequests(RequestManager requestManager){
    int size = listGetSize(requestManager->waitingRequestsQueue);
    if(size >= 1) return 1;
    return 0;
}

int requestManagerCanAcceptRequests(RequestManager requestManager){
    int s1 = listGetSize(requestManager->waitingRequestsQueue);
    int s2 = listGetSize(requestManager->runningRequests);
    if(requestManager->maxAcceptedRequests > s1 + s2)
        return 1;
    return 0;
}

int requestManagerHasReachedItMaxRequests(RequestManager requestManager, int max_size){
    int runningQueueSize = listGetSize(requestManager->runningRequests);
    int waitingQueueSize = listGetSize(requestManager->waitingRequestsQueue);
    if(runningQueueSize + waitingQueueSize < max_size)
        return 0;
    return 1;
}

RequestObject requestManagerGetReadyRequest(RequestManager requestManager){
    int size = listGetSize(requestManager->waitingRequestsQueue);
    if(size == 0)
        return NULL;//todo: return status instead
    RequestObject requestObject = listPopFront(requestManager->waitingRequestsQueue);
    return requestObject;
}

void requestManagerAddPendingRequest(RequestManager requestManager, RequestObject requestObject) {
    //todo: add only if there is enough room
    listPushBack(requestManager->waitingRequestsQueue, requestObject);
    deleteRequestObject(requestObject);///we are deleting the current r.o because the list copies it
}

void requestManagerAddReadyRequest(RequestManager requestManager, RequestObject requestObject){
    //todo: add only if there is enough room
    requestObjectUpdateDispatchTime(requestObject);
    listPushBack(requestManager->runningRequests, requestObject);
//    deleteRequestObject(requestObject);///we are deleting the current r.o because the list copies it
}

void requestManagerRemoveFinishedRequest(RequestManager requestManager, RequestObject requestObject){
    RequestObject requestObject1 = createRequestObject(-2);
    listRemoveAtData(requestManager->runningRequests,requestObject,(void**)(&requestObject1));
    deleteRequestObject(requestObject1);
    deleteRequestObject(requestObject);
}

void requestManagerRemoveRequestFromWaitingQueue(RequestManager requestManager, RequestObject requestObject){
    RequestObject requestObject1 = createRequestObject(-2);
    listRemoveAtData(requestManager->waitingRequestsQueue,requestObject,(void**)(&requestObject1));
    deleteRequestObject(requestObject1);
    deleteRequestObject(requestObject);
}

RequestObject requestManagerRemoveRequestFromWaitingQueueAtIndex(RequestManager requestManager, int index) {
    RequestObject requestObject1 = createRequestObject(-2);
    listRemoveAtIndex(requestManager->waitingRequestsQueue, index, (void**)(&requestObject1));
    return requestObject1;
}

RequestObject requestManagerRemovRandRequestFromWaitingQueue(RequestManager requestManager){
   int rand_index_from_waiting = rand() % requestManagerGetWaitingQueueSize(requestManager);
   return requestManagerRemoveRequestFromWaitingQueueAtIndex(requestManager, rand_index_from_waiting);
}

int requestManagerRemoveOldestRequestFromWaitingQueue(RequestManager requestManager) {
    RequestObject requestObject = listPopFront(requestManager->waitingRequestsQueue);
    int c = requestObject->val;
    deleteRequestObject(requestObject);
    return c;
}

void requestManagerEnlargeMaxAcceptedRequests(RequestManager requestManager){
    requestManager->maxAcceptedRequests = requestManager->maxAcceptedRequests + 1;
}

void requestManagerDelete(RequestManager requestManager){
    listDelete(requestManager->waitingRequestsQueue);
    listDelete(requestManager->runningRequests);
    free(requestManager);
}


void requestManagerPrint(RequestManager requestManager){
    printf("\n------------------------------------------------\n");
    printf("requestManager->maxAcceptedRequests: %d\n", requestManager->maxAcceptedRequests);
    int ws = listGetSize(requestManager->waitingRequestsQueue);
    int rs = listGetSize(requestManager->runningRequests);
    printf("waiting requests: %d,running requests: %d\n", ws, rs);
    printf("waiting list:  ");
    listPrint(requestManager->waitingRequestsQueue);
    printf("running list:  ");
    listPrint(requestManager->runningRequests);
    printf("\n------------------------------------------------\n");
}