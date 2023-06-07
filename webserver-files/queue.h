#ifndef QUENE_DATA_STRUCTURE
#define QUENE_DATA_STRUCTURE

typedef enum listResult_t {
    LS_SUCCESS,
    LS_OUT_OF_MEMORY,
    LS_NULL_ARGUMENT,
    LS_OUT_OF_INDEX,
    LS_EMPTY,
    LS_NOT_FOUND,
    LS_ERROR
} ListResult;

typedef void * Data;

typedef Data(*CopyDataFunction)(Data data);
typedef void(*FreeDataFunction)(Data data);
typedef int(*CompareDataFunction)(Data data1,Data data2);
typedef void(*Print)(Data data);

typedef struct node_t {
    struct node_t *next;
    struct node_t *prev;
    CopyDataFunction copyDataFunction;
    CompareDataFunction compareDataFunction;
    FreeDataFunction freeDataFunction;
    Data data;
} *Node;

typedef struct list_t{
    Node head;
    Node tail;

    CopyDataFunction copyDataFunction;
    CompareDataFunction compareDataFunction;
    FreeDataFunction freeDataFunction;
    Print print;

    int size;
}*List;

List listCreate(CopyDataFunction copyDataFunction, CompareDataFunction compareDataFunction, FreeDataFunction freeDataFunction, Print print);

ListResult listPushBack(List list, Data data);
ListResult listPushFront(List list, Data data);

ListResult listInsertAtIndex(List list, int index, Data data);

Data listPopFront(List list);
Data listPopBack(List list);

ListResult listRemoveAtIndex(List list, int index, Data* removedData);
ListResult listRemoveAtData(List list, Data data, Data* removedData);

int listGetSize(List list);

void listPrint(List list);

void listDelete(List list);
#endif //QUENE_DATA_STRUCTURE
