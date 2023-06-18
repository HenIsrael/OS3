#include "requests_handler.h"
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

RequestManager createRequestsHandler(int threadsNum, int maxAcceptedRequests){
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

RequestObject getReadyRequest(RequestManager requestManager){
    int waiting = listGetSize(requestManager->waitingRequestsQueue);
    if(waiting == 0)
        return NULL;
    RequestObject ready = listPopFront(requestManager->waitingRequestsQueue);
    return ready;
}

void addPendingRequest(RequestManager requestManager, RequestObject requestObject) {

    listPushBack(requestManager->waitingRequestsQueue, requestObject);
    // a request copy pushed for waiting queue
    // need to delete the original
    deleteRequestObject(requestObject); 
}

void addReadyRequest(RequestManager requestManager, RequestObject requestObject){
    requestObjectUpdateDispatchTime(requestObject);
    listPushBack(requestManager->runningRequests, requestObject);
}

void removeFinishedRequest(RequestManager requestManager, RequestObject requestObject){
    RequestObject request_tmp = createRequestObject(-2);
    listRemoveAtData(requestManager->runningRequests,requestObject,(void**)(&request_tmp));
    deleteRequestObject(request_tmp);
    deleteRequestObject(requestObject);
}

RequestObject removeWaitingRequestAt(RequestManager requestManager, int index) {
    RequestObject request_tmp = createRequestObject(-2);
    listRemoveAtIndex(requestManager->waitingRequestsQueue, index, (void**)(&request_tmp));
    return request_tmp;
}

RequestObject removeRandWaitingRequest(RequestManager requestManager){
   int rand_index_from_waiting = rand() % countWaitingQueue(requestManager);
   return removeWaitingRequestAt(requestManager, rand_index_from_waiting);
}

int removeOldestWaitingRequest(RequestManager requestManager) {
    RequestObject oldest = listPopFront(requestManager->waitingRequestsQueue);
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
