#include "requests_handler.h"
#include "stdlib.h"

void *copyData(void *data)
{
    return copyRequest((Request)data);
}

int compareData(Data data1, Data data2){
    return compareRequests((Request)data1, (Request)data2);
}

void freeData(void *data)
{
    deleteRequest((Request)data);
}

void printData(void*data){
    Request requestObject = (Request)data;
    printf("|request id = %d| --> ",requestObject->val);
}

RequestsHandler createRequestsHandler(int maxAcceptedRequests){
    RequestsHandler requestsHandler = malloc(sizeof (*requestsHandler));
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

int countWaitingQueue(RequestsHandler requestsHandler){
    int size = listGetSize(requestsHandler->waitingRequestsQueue);
    return size;
}

int waitingQueueIsEmpty(RequestsHandler requestsHandler){
    int size = listGetSize(requestsHandler->waitingRequestsQueue);
    if(size >= 1) return 0;
    return 1;
}

int runningQueueIsEmpty(RequestsHandler requestsHandler){
    int size = listGetSize(requestsHandler->runningRequests);
    if(size >= 1)
        return 0;
    return 1;
}

int requestsHandlerCanAcceptRequests(RequestsHandler requestsHandler){

    int running = listGetSize(requestsHandler->runningRequests);
    int waiting = listGetSize(requestsHandler->waitingRequestsQueue);
    
    if(requestsHandler->maxAcceptedRequests > waiting + running)
        return 1;
    return 0;
}

int requestsHandlerHasReachedItMaxRequests(RequestsHandler requestsHandler, int max_size){

    int running = listGetSize(requestsHandler->runningRequests);
    int waiting = listGetSize(requestsHandler->waitingRequestsQueue);

    if(running + waiting < max_size)
        return 0;
    return 1;
}

Request getReadyRequest(RequestsHandler requestsHandler){
    int waiting = listGetSize(requestsHandler->waitingRequestsQueue);
    if(waiting == 0)
        return NULL;
    Request ready = listPopFront(requestsHandler->waitingRequestsQueue);
    return ready;
}

void addPendingRequest(RequestsHandler requestsHandler, Request request) {

    listPushBack(requestsHandler->waitingRequestsQueue, request);
    // a request copy pushed for waiting queue
    // need to delete the original
    deleteRequest(request); 
}

void addReadyRequest(RequestsHandler requestsHandler, Request request){
    updateDispatchTime(request);
    listPushBack(requestsHandler->runningRequests, request);
}

void removeFinishedRequest(RequestsHandler requestsHandler, Request request){
    Request request_tmp = createRequest(-2);
    listRemoveAtData(requestsHandler->runningRequests,request,(void**)(&request_tmp));
    deleteRequest(request_tmp);
    deleteRequest(request);
}

Request removeWaitingRequestAt(RequestsHandler requestsHandler, int index) {
    Request request_tmp = createRequest(-2);
    listRemoveAtIndex(requestsHandler->waitingRequestsQueue, index, (void**)(&request_tmp));
    return request_tmp;
}

Request removeRandWaitingRequest(RequestsHandler requestsHandler){
   int rand_index_from_waiting = rand() % countWaitingQueue(requestsHandler);
   return removeWaitingRequestAt(requestsHandler, rand_index_from_waiting);
}

int removeOldestWaitingRequest(RequestsHandler requestsHandler) {
    Request oldest = listPopFront(requestsHandler->waitingRequestsQueue);
    int fd = oldest->val;
    deleteRequest(oldest);
    return fd;
}

void enlargeMaxAcceptedRequests(RequestsHandler requestsHandler){
    requestsHandler->maxAcceptedRequests = requestsHandler->maxAcceptedRequests + 1;
}

void requestHandlerDelete(RequestsHandler requestsHandler){
    listDelete(requestsHandler->waitingRequestsQueue);
    listDelete(requestsHandler->runningRequests);
    free(requestsHandler);
}
