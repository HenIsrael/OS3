#include "request_obj.h"
#include "stdlib.h"
#include <sys/time.h>

Request createRequest(int val){
    Request request = malloc(sizeof (*request));
    if(request == NULL)
        return request;
    request->val = val;
    gettimeofday(&(request->time_arrive),NULL);
    return request;
}

Request copyRequest(Request request){
    Request request_copy = createRequest(request->val);
    request_copy->time_arrive.tv_sec = request->time_arrive.tv_sec;
    request_copy->time_arrive.tv_usec = request->time_arrive.tv_usec;

    request_copy->disp.tv_sec = request->disp.tv_sec;
    request_copy->disp.tv_usec = request->disp.tv_usec;
    return request_copy;
}

int compareRequests(Request request1, Request request2){
    if(request1->val > request2->val)
        return 1;
    if(request1->val == request2->val)
        return 0;
    return -1;
}

void deleteRequest(Request request){
    free(request);
}

void dispTime( struct timeval *start, struct timeval *finish,struct timeval *ans)
{
    timersub(finish, start, ans);

}

void updateDispatchTime(Request request){
    struct timeval time_now;
    gettimeofday(&time_now,NULL);
    dispTime(&(request->time_arrive),&time_now,&(request->disp));
}