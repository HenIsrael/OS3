#include "segel.h"
#include "request.h"
#include "request_manager.h"
#include "worker.h"

#include <math.h>

#define MAXSCHEDULINGLEN 7

pthread_mutex_t Lock;
pthread_cond_t FullPool;  // for both waiting and running queues
pthread_cond_t EmptyPool; // only for waiting requests queue
pthread_cond_t NoFish;
RequestManager requests_control;


// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// HW3: Parse the new arguments too

// -------------------- our cool functions -------------------- //


void* thread_routineHen(void* worker){

    WorkerThread* worker_act= (WorkerThread*)worker;

    while(1){
        pthread_mutex_lock(&Lock);
        while(!requestManagerHasWaitingRequests(requests_control)){
            pthread_cond_wait(&EmptyPool, &Lock);
        }

        RequestObject request_ready = requestManagerGetReadyRequest(requests_control);
        requestManagerAddReadyRequest(requests_control, request_ready);

        int fd = request_ready->val;
        struct timeval arrive = request_ready->time_arrive;
        struct timeval disp = request_ready->disp;
        pthread_mutex_unlock(&Lock);

        requestHandle(fd, worker_act, arrive, disp);
	    Close(fd);

        pthread_mutex_lock(&Lock);
        requestManagerRemoveFinishedRequest(requests_control, request_ready);
        pthread_cond_signal(&FullPool);
        pthread_mutex_unlock(&Lock);
    }
}



void thread_routine (void* worker )
{
    WorkerThread* worker_act= (WorkerThread*)worker;
    while (1)
    {
        pthread_mutex_lock(&Lock);
        while(!requestManagerHasWaitingRequests(requests_control)){
        pthread_cond_wait(&EmptyPool , &Lock);
        }
        // קמתי לתחיה, אני רוצה למשוך משימה מהתור 
        RequestObject current_task = requestManagerGetReadyRequest(requests_control);
        requestManagerAddReadyRequest(requests_control, current_task);

        struct timeval arrive = current_task->time_arrive;
        struct timeval disp = current_task->disp;

        int soc_fd= current_task->val;
        pthread_mutex_unlock(&Lock);

        requestHandle(soc_fd, worker_act, arrive, disp);
        Close(soc_fd);

        pthread_mutex_lock(&Lock);
        requestManagerRemoveFinishedRequest(requests_control, current_task);
        if(listGetSize(requests_control->runningRequests)== 0)
        {
            pthread_cond_signal(&NoFish);
        }
        pthread_cond_signal(&FullPool);
        pthread_mutex_unlock(&Lock);
    }
}

void pool_initialization(int threads){
    
    pthread_t *threads_pool = (pthread_t*)malloc(threads * sizeof(pthread_t));
    memset(threads_pool, 0, threads * sizeof(threads_pool[0]));

    for (int i = 0; i < threads; i++){

        WorkerThread* worker = create_thread(i);
        int ans = pthread_create(&(threads_pool[i]), NULL, &thread_routineHen, (void*)worker);
        if (ans != 0){
            fprintf(stderr, "pthread_create failed\n"); // TODO: check if message is OK 
            // TODO: maybe free worker?free pool? free locks?
            exit(1);
        }
    }
}





// -------------------- end of cool functions -------------------- //


void getargs(int *port, int *threads, int *queue_size, char *schedalg, int *max_size, int argc, char *argv[])
{
    if (argc < 5) { 
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *threads = atoi(argv[2]);
    *queue_size = atoi(argv[3]);
    strcpy(schedalg, argv[4]);
    *max_size = atoi(argv[5]);
}


int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, threads, queue_size, max_size;
    char *schedalg = (char*)malloc(MAXSCHEDULINGLEN);

    struct sockaddr_in clientaddr;

    getargs(&port, &threads, &queue_size, schedalg, &max_size,  argc, argv);

    pthread_mutex_init(&Lock, NULL);
    pthread_cond_init(&FullPool, NULL);
    pthread_cond_init(&EmptyPool, NULL);
    pthread_cond_init(&NoFish, NULL);
    

    requests_control = requestManagerCreate(0, queue_size);
    pool_initialization(threads);

    listenfd = Open_listenfd(port);
    while (1) {

        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

        pthread_mutex_lock(&Lock);
        if(requestManagerCanAcceptRequests(requests_control)){
            RequestObject fish_request = createRequestObject(connfd);
            requestManagerAddPendingRequest(requests_control, fish_request);
            pthread_cond_signal(&EmptyPool);
            pthread_mutex_unlock(&Lock);
        }
        else{
            if (strcmp(schedalg, "block")) 
            {

                while (!requestManagerCanAcceptRequests(requests_control)){
                
                    pthread_cond_wait(&FullPool, &Lock);
                }

                RequestObject fish_request = createRequestObject(connfd);
                requestManagerAddPendingRequest(requests_control, fish_request);
                pthread_cond_signal(&EmptyPool);
                pthread_mutex_unlock(&Lock);
            }
            else if (strcmp(schedalg, "dt")) // TODO: TALI THE QUEEN
            {
                close(connfd);
                pthread_mutex_unlock(&Lock);
            }
            else if (strcmp(schedalg, "dh")) 
            {
                if (!requestManagerCanAcceptRequests(requests_control)){
                
                    if (!requestManagerHasWaitingRequests(requests_control)){
                        // case no waiting requests
                        // do nothing
                    
                        Close(connfd);
                        pthread_mutex_unlock(&Lock);
                        continue;
                    }

                    int old_req_fd = requestManagerRemoveOldestRequestFromWaitingQueue(requests_control);
                    Close(old_req_fd);
                    RequestObject fish_request = createRequestObject(connfd);
                    requestManagerAddPendingRequest(requests_control, fish_request);
                    pthread_cond_signal(&EmptyPool);
                    pthread_mutex_unlock(&Lock);

                }
            }
            else if (strcmp(schedalg, "bf")) // TODO: TALI THE QUEEN
            {
                close(connfd);
                while ( listGetSize(requests_control->runningRequests)!= 0 )
                {
                    pthread_cond_wait( &NoFish , &Lock);
                }
                pthread_mutex_unlock(&Lock);   
            }
            else if (strcmp(schedalg, "dynamic")) 
            {
                if(requestManagerHasReachedItMaxRequests(requests_control, max_size)){
                    // conduct like it is 'drop tail' policy
                    Close(connfd);
                    pthread_mutex_unlock(&Lock);
                    continue;
                }
                else{
                    Close(connfd);
                    requestManagerEnlargeMaxAcceptedRequests(requests_control);
                    pthread_mutex_unlock(&Lock);
                }
            }
            /*
            else if (strcmp(schedalg, "random"))
            {
                if (!requestManagerCanAcceptRequests(requests_control))
                {
                    if(!requestManagerHasWaitingRequests(requests_control)){
                        // case no waiting requests
                        // do nothing
                    
                        Close(connfd);
                        pthread_mutex_unlock(&Lock);
                        continue;
                    }

                    double num_to_delete = ceil(((((double) requestManagerGetWaitingQueueSize(requests_control)) / 2)));
                    for (int i = 0; i < num_to_delete; i++) {
                        //remove randomly half of the waiting requests
                        RequestObject rand_fish_request = requestManagerRemovRandRequestFromWaitingQueue(requests_control);
                        Close(rand_fish_request->val);
                    }

                    RequestObject fish_request = createRequestObject(connfd);
                    requestManagerAddPendingRequest(requests_control, fish_request);
                    pthread_cond_signal(&EmptyPool);
                    pthread_mutex_unlock(&Lock);
                }
            
            }
            */
            
        }

    }
    pthread_mutex_destroy(&Lock);
    pthread_cond_destroy(&FullPool);
    pthread_cond_destroy(&EmptyPool);
    pthread_cond_destroy(&NoFish);
    requestManagerDelete(requests_control);
    
    // TODO : free pool of threads 
}


    


 
