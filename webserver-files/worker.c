#include "worker.h"

WorkerThread* create_thread(int thread_id)
{
    WorkerThread* thread = (WorkerThread*) malloc(sizeof (WorkerThread));
    thread->thread_id = thread_id;
    thread->thread_count = 0;
    thread->thread_static_count = 0;
    thread->thread_dynamic_count = 0;
    return thread;
}

void delete_thread(WorkerThread* thread)
{
    free(thread);
}