#ifndef _WORKERTHREAD_H
#define _WORKERTHREAD_H

#include "segel.h"

typedef struct WorkerThread
{
    int thread_id;
    int thread_count;
    int thread_static_count;
    int thread_dynamic_count;
}WorkerThread;

WorkerThread* create_thread(int thread_id);
void delete_thread(WorkerThread* thread);

#endif //_WORKERTHREAD_H