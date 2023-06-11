#include "segel.h"
#include "request.h"
#include "request_manager.h"
#include "worker.h"
#include "request_obj.h"

pthread_mutex_t Lock;
pthread_cond_t FullPool;
pthread_cond_t EmptyPool;
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

void* thread_routine(void* worker){

    WorkerThread* worker_act= (WorkerThread*)worker;

    while(1){
        pthread_mutex_lock(&Lock);
        while(!requestManagerHasWaitingRequests(requests_control)){
            pthread_cond_wait(&EmptyPool, &Lock);
        }

        RequestObject request_ready = requestManagerGetReadyRequest(requests_control);
        requestManagerAddReadyRequest(requests_control, request_ready);

        int fd = request_ready->val;
        pthread_mutex_unlock(&Lock);

        requestHandle(fd);
	    Close(fd);

        pthread_mutex_lock(&Lock);
        requestManagerRemoveFinishedRequest(requests_control, request_ready);
        pthread_cond_signal(&FullPool);
        pthread_mutex_unlock(&Lock);
    }
}

void pool_initialization(int threads){

    
    pthread_t *threads_pool = (pthread_t*)malloc(threads * sizeof(pthread_t));
    memset(threads_pool, 0, threads * sizeof(threads_pool[0]));

    for (int i = 0; i < threads; i++){

        WorkerThread* worker = create_thread(i);
        int ans = pthread_create(&(threads_pool[i]), NULL, &thread_routine, (void*)worker);
        if (ans != 0){
            fprinff(stderr, "pthread_create failed\n"); // TODO: check if message is OK 
            // TODO: maybe free worker?free pool? free locks?
            exit(1);
        }
    }
}

// -------------------- end of cool functions -------------------- //


void getargs(int *port, int *threads, int *queue_size,  int argc, char *argv[]) //TODO : need to add more parms 
{
    if (argc < 4) { // TODO : maybe 5 or 6 depend the amount of params
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *threads = atoi(argv[2]);
    *queue_size = atoi(argv[3]);
    //TODO: *schedalg = 
    //      *max_size = 

}


int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, threads, queue_size;
    // TODO: [1] add policy param : 'schedalg' char*
    //       [2] max_size?

    struct sockaddr_in clientaddr;

    getargs(&port, &threads, &queue_size, argc, argv);

    pthread_cond_init(&FullPool, NULL);
    pthread_cond_init(&EmptyPool, NULL);
    pthread_mutex_init(&Lock, NULL);

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
            // TODO:part 2 code
        
        }

    }

    pthread_cond_destroy(&FullPool);
    pthread_cond_destroy(&EmptyPool);
    pthread_mutex_destroy(&Lock);
    // TODO : free pool of threads 
}


    


 
