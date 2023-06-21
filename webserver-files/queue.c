#include "queue.h"
#include "stdlib.h"
#include <stdio.h>

/**************************************************************
 * Description: Node Structure Implementation
 **************************************************************/

Node nodeCreate(Data data,CopyDataFunction copyDataFunction, CompareDataFunction compareDataFunction, FreeDataFunction freeDataFunction) {
    Node newNode = malloc(sizeof(*newNode));
    if (newNode == NULL) {
        return NULL;
    }
    if(data != NULL){
        newNode->data = copyDataFunction(data);
        if (newNode->data == NULL) {
            free(newNode);
            return NULL;
        }
    }
    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->copyDataFunction = copyDataFunction;
    newNode->compareDataFunction = compareDataFunction;
    newNode->freeDataFunction = freeDataFunction;
    return newNode;
}

int isNodeEdge(Node node){
    if(node->data == NULL)
        return 1;
    return 0;
}

void nodeSetNext(Node currentNode, Node nextNode){
    Node tempNext = currentNode->next;

    currentNode->next = nextNode;
    nextNode->next = tempNext;

    tempNext->prev = nextNode;
    nextNode->prev = currentNode;
}

void nodeSetPrev(Node currentNode, Node prevNode){
    Node tempPrev = currentNode->prev;

    currentNode->prev = prevNode;
    prevNode->prev = tempPrev;

    tempPrev->next = prevNode;
    prevNode->next = currentNode;
}

void nodeFree(Node node){
    node->freeDataFunction(node->data);
    free(node);
}

void nodeDelete(Node node){
    Node next = node->next;
    nodeFree(node);
    if(next != NULL) nodeDelete(next);
}

// End of node section

/**************************************************************
 * Description: Queue Structure Implementation
 **************************************************************/

List list_create(CopyDataFunction copyDataFunction, CompareDataFunction compareDataFunction,FreeDataFunction freeDataFunction, Print print){
    List list = malloc(sizeof(*list));
    if(list == NULL){
        return NULL;
    }
    list->copyDataFunction = copyDataFunction;
    list->compareDataFunction = compareDataFunction;
    list->freeDataFunction = freeDataFunction;
    list->print = print;

    list->head = nodeCreate(NULL, copyDataFunction, compareDataFunction,freeDataFunction);
    if(list->head == NULL){
        free(list);
        return NULL;
    }

    list->tail = nodeCreate(NULL, copyDataFunction, compareDataFunction, freeDataFunction);
    if(list->tail == NULL){
        nodeFree(list->head);
        free(list);
        return NULL;
    }

    list->head->next = list->tail;
    list->tail->prev = list->head;

    list->size = 0;

    return list;
}


ListResult list_push_back(List list, Data data){
    if(list == NULL || data == NULL){
        return LS_NULL_ARGUMENT;
    }
    Node node = nodeCreate(data, list->copyDataFunction, list->compareDataFunction,list->freeDataFunction);
    if(node == NULL){
        return LS_OUT_OF_MEMORY;
    }
    list->size++;
    nodeSetPrev(list->tail, node);
    return LS_SUCCESS;
}

ListResult list_push_front(List list, Data data){
    if(list == NULL || data == NULL){
        return LS_NULL_ARGUMENT;
    }
    Node node = nodeCreate(data, list->copyDataFunction, list->compareDataFunction,list->freeDataFunction);
    if(node == NULL){
        return LS_OUT_OF_MEMORY;
    }
    list->size++;
    nodeSetNext(list->head, node);
    return LS_SUCCESS;
}

ListResult list_insert_at_index(List list, int index, Data data){
    if(list == NULL || data == NULL){
        return LS_NULL_ARGUMENT;
    }
    if(index > list->size){
        return  LS_OUT_OF_INDEX;
    }
    Node node = nodeCreate(data, list->copyDataFunction, list->compareDataFunction,list->freeDataFunction);
    if(node == NULL){
        return LS_OUT_OF_MEMORY;
    }
    Node currentNode = list->head;
    for (int i = 0; i < index; ++i) {
        currentNode = currentNode->next;
    }
    nodeSetNext(currentNode, node);
    list->size++;
    return LS_SUCCESS;
}

Data list_pop_front(List list){
    if(list->size == 0){
        return NULL;
    }
    Node temp = list->head->next;
    Data data = list->copyDataFunction(temp->data);
    if(data == NULL){
        return NULL;
    }
    list->head->next = temp->next;
    temp->next->prev = list->head;

    nodeFree(temp);
    list->size--;
    return data;
}

Data list_pop_back(List list){
    if(list->size == 0){
        return NULL;
    }
    Node temp = list->tail->prev;
    Data data = list->copyDataFunction(temp->data);
    if(data == NULL){
        return NULL;
    }
    list->tail->prev = temp->prev;
    temp->prev->next = list->tail;

    nodeFree(temp);
    list->size--;
    return data;
}

Data _removeNode(List list, Node currentNode){
    Data removedData;

    Node prev = currentNode->prev;
    Node next = currentNode->next;

    prev->next = next;
    next->prev = prev;

    removedData = list->copyDataFunction(currentNode->data);

    nodeFree(currentNode);
    return removedData;
}

ListResult list_remove_at_index(List list, int index, Data* removedData){
    if(list == NULL){
        return LS_NULL_ARGUMENT;
    }
    if(list->size == 0){
        return LS_EMPTY;
    }
    if(index > list->size || index < 0){
        return  LS_OUT_OF_INDEX;
    }
    Node currentNode = list->head->next;
    for (int i = 0; i < index; ++i) {
        currentNode = currentNode->next;
    }
    *removedData = _removeNode(list, currentNode);
    list->size--;

    return LS_SUCCESS;
}

ListResult list_remove_at_data(List list, Data data, Data* removedData){
    if(list == NULL){
        return LS_NULL_ARGUMENT;
    }
    if(list->size == 0){
        return LS_EMPTY;
    }
    int founded = 0;
    Node currentNode = list->head->next;
    while (!isNodeEdge(currentNode)){
        if(list->compareDataFunction(currentNode->data, data) == 0){
            founded = 1;
            break;
        }
        currentNode = currentNode->next;
    }
    if(!founded){
        return LS_NOT_FOUND;
    }
    *removedData = _removeNode(list, currentNode);
    list->size--;

    return LS_SUCCESS;
}

int list_get_size(List list){
    return  list->size;
}

void list_print(List list){
    Node node = list->head;
    int size = list_get_size(list);
    if(size == 0)
        return;
    for (int i = 0; i < size; ++i) {
        node = node->next;
        list->print(node->data);
    }
}

void list_delete(List list){
    nodeDelete(list->head);
    free(list);
}

// End of queue section