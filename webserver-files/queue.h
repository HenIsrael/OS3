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

List list_create(CopyDataFunction copyDataFunction, CompareDataFunction compareDataFunction, FreeDataFunction freeDataFunction, Print print);

ListResult list_push_back(List list, Data data);
ListResult list_push_front(List list, Data data);

ListResult list_insert_at_index(List list, int index, Data data);

Data list_pop_front(List list);
Data list_pop_back(List list);

ListResult list_remove_at_index(List list, int index, Data* removedData);
ListResult list_remove_at_data(List list, Data data, Data* removedData);

int list_get_size(List list);

void list_print(List list);

void list_delete(List list);
#endif //QUENE_DATA_STRUCTURE
