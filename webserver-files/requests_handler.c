#include "requests_handler.h"
#include "stdlib.h"

void *copyData(void *data)
{
    return copyRequestObject((Request)data);
}

int compareData(Data data1, Data data2){
    return compareRequestObject((Request)data1, (Request)data2);
}

void freeData(void *data)
{
    deleteRequestObject((Request)data);
}

void printData(void*data){
    Request requestObject = (Request)data;
    printf("|request id = %d| --> ",requestObject->val);
}

RequestManager createRequestsHandler(int maxAcceptedRequests){
    RequestManager requestsHandler = malloc(sizeof (*requestsHandler));
    requestsHandler->runningRequests = listCreate(copyData, compareData, freeData, printData);

    if(requestsHandler->runningRequests == NULL){
        free(requestsHandler);
        return NULL;
    }
    requestsHandler->waitingRequestsQueue = listCreate(copyData, compareData, freeData, printData);

    if(requestsHandler->waitingRequestsQueue == NULL){
        listDelete(requestsHandler->runningRequests);
        free(requestsHandler);
        return NULL;
    }

    requestsHandler->maxAcceptedRequests = maxAcceptedRequests;
    return requestsHandler;
}

int countWaitingQueue(RequestManager requestManager){
    int size = listGetSize(requestManager->waitingRequestsQueue);
    return size;
}

int waitingQueueIsEmpty(RequestManager requestManager){
    int size = listGetSize(requestManager->waitingRequestsQueue);
    if(size >= 1) return 0;
    return 1;
}

int runningQueueIsEmpty(RequestManager requestManager){
    int size = listGetSize(requestManager->runningRequests);
    if(size >= 1)
        return 0;
    return 1;
}

int requestsHandlerCanAcceptRequests(RequestManager requestManager){

    int running = listGetSize(requestManager->runningRequests);
    int waiting = listGetSize(requestManager->waitingRequestsQueue);
    
    if(requestManager->maxAcceptedRequests > waiting + running)
        return 1;
    return 0;
}

int requestsHandlerHasReachedItMaxRequests(RequestManager requestManager, int max_size){

    int running = listGetSize(requestManager->runningRequests);
    int waiting = listGetSize(requestManager->waitingRequestsQueue);

    if(running + waiting < max_size)
        return 0;
    return 1;
}

Request getReadyRequest(RequestManager requestManager){
    int waiting = listGetSize(requestManager->waitingRequestsQueue);
    if(waiting == 0)
        return NULL;
    Request ready = listPopFront(requestManager->waitingRequestsQueue);
    return ready;
}

void addPendingRequest(RequestManager requestManager, Request requestObject) {

    listPushBack(requestManager->waitingRequestsQueue, requestObject);
    // a request copy pushed for waiting queue
    // need to delete the original
    deleteRequestObject(requestObject); 
}

void addReadyRequest(RequestManager requestManager, Request requestObject){
    requestObjectUpdateDispatchTime(requestObject);
    listPushBack(requestManager->runningRequests, requestObject);
}

void removeFinishedRequest(RequestManager requestManager, Request requestObject){
    Request request_tmp = createRequestObject(-2);
    listRemoveAtData(requestManager->runningRequests,requestObject,(void**)(&request_tmp));
    deleteRequestObject(request_tmp);
    deleteRequestObject(requestObject);
}

Request removeWaitingRequestAt(RequestManager requestManager, int index) {
    Request request_tmp = createRequestObject(-2);
    listRemoveAtIndex(requestManager->waitingRequestsQueue, index, (void**)(&request_tmp));
    return request_tmp;
}

Request removeRandWaitingRequest(RequestManager requestManager){
   int rand_index_from_waiting = rand() % countWaitingQueue(requestManager);
   return removeWaitingRequestAt(requestManager, rand_index_from_waiting);
}

int removeOldestWaitingRequest(RequestManager requestManager) {
    Request oldest = listPopFront(requestManager->waitingRequestsQueue);
    int fd = oldest->val;
    deleteRequestObject(oldest);
    return fd;
}

void enlargeMaxAcceptedRequests(RequestManager requestManager){
    requestManager->maxAcceptedRequests = requestManager->maxAcceptedRequests + 1;
}

void requestHandlerDelete(RequestManager requestManager){
    listDelete(requestManager->waitingRequestsQueue);
    listDelete(requestManager->runningRequests);
    free(requestManager);
}
