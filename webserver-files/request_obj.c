#include "request_obj.h"
#include "stdlib.h"

RequestObject createRequestObject(int val){
    RequestObject requestObject = malloc(sizeof (*requestObject));
    if(requestObject == NULL)
        return requestObject;
    requestObject->val = val;
    gettimeofday(&(requestObject->time_arrive),NULL);
    return requestObject;
}

RequestObject copyRequestObject(RequestObject requestObject){
    RequestObject requestObject1 = createRequestObject(requestObject->val);
    requestObject1->time_arrive.tv_sec = requestObject->time_arrive.tv_sec;
    requestObject1->time_arrive.tv_usec = requestObject->time_arrive.tv_usec;

    requestObject1->disp.tv_sec = requestObject->disp.tv_sec;
    requestObject1->disp.tv_usec = requestObject->disp.tv_usec;
    return requestObject1;
}

int compareRequestObject(RequestObject requestObject1, RequestObject requestObject2){
    if(requestObject1->val > requestObject2->val) return 1;
    if(requestObject1->val == requestObject2->val) return 0;
    return -1;
}

void deleteRequestObject(RequestObject requestObject){
    free(requestObject);
}

void dispTime( struct timeval *start, struct timeval *finish,struct timeval *ans)
{

    if (finish->tv_usec > 999999)
    {
        finish->tv_sec += finish->tv_usec / 1000000;
        finish->tv_usec %= 1000000;
    }

    if (start->tv_usec > 999999)
    {
        start->tv_sec += start->tv_usec / 1000000;
        start->tv_usec %= 1000000;
    }

    ans->tv_sec = finish->tv_sec - start->tv_sec;
    ans->tv_usec = finish->tv_usec - start->tv_usec;

    if (ans->tv_usec< 0)
    {
        ans->tv_usec += 1000000;
        ans->tv_sec--;
    }
}

void requestObjectUpdateDispatchTime(RequestObject requestObject){
    struct timeval time_now;
    gettimeofday(&time_now,NULL);
    dispTime(&(requestObject->time_arrive),&time_now,&(requestObject->disp));
}