#ifndef REQUEST_OBJECT_H
#define REQUEST_OBJECT_H

#include "segel.h"

typedef struct request_object_t{
    int val;
    struct timeval time_arrive;
    struct timeval disp;
}*Request;

Request createRequest(int val);
Request copyRequest(Request request);

int compareRequests(Request request1, Request request2);
void deleteRequest(Request request);
void updateDispatchTime(Request request);

#endif //REQUEST_OBJECT_H
