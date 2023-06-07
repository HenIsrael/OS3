#ifndef REQUEST_OBJECT_H
#define REQUEST_OBJECT_H

#include "segel.h"

typedef struct request_object_t{
    int val;
    struct timeval time_arrive;
    struct timeval disp;
}*RequestObject;

RequestObject createRequestObject(int val);
RequestObject copyRequestObject(RequestObject requestObject);

int compareRequestObject(RequestObject requestObject1, RequestObject requestObject2);
void deleteRequestObject(RequestObject requestObject);
void requestObjectUpdateDispatchTime(RequestObject requestObject);

#endif //REQUEST_OBJECT_H
