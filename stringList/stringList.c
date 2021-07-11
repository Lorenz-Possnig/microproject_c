//
// Created by develop on 10.07.21.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stringList.h"

StringList *new__StringList() {
    StringList *result = malloc(sizeof(StringList));
    result->start = NULL;
    result->end = NULL;
    result->length = 0;
    return result;
}

StringListNode *newNode(char* s) {
    StringListNode *node = malloc(sizeof(StringListNode));
    node->string = s;
    node->next = NULL;
    return node;
}

void appendString(StringList *list, char* s) {
    char *strCopy = malloc(strlen(s));
    strcpy(strCopy, s);
    StringListNode *node = newNode(strCopy);
    if (list->length == 0) {
        list->start = node;
        list->end = node;
        list->length++;
    } else {
        list->end->next = node;
        list->end = node;
        list->length++;
    }
}

void printStrings(StringList *list) {
    if (list->length < 1) {
        printf("[]");
    }
    for (StringListNode *current = list->start; current; current = current->next) {
        printf("%s\n", current->string);
    }
}

char *get(StringList* list, int index) {
    if (index > list->length) {
        return NULL;
    } else {
        StringListNode *current = list->start;
        for (int i = 0; i != index; i++) {
            if (i == index) {
                break;
            } else {
                current = current->next;
            }
        }
        return current->string;
    }
}

void freeStringNode(StringListNode *node) {
    free(node->string);
    free(node);
}

void freeStringList(StringList* list) {
    if (list->start == NULL) { free(list); return; }
    StringListNode* toDelete = list->start;
    for (StringListNode *current = list->start; current; current = current->next) {
        freeStringNode(toDelete);
        toDelete = current;
    }
    freeStringNode(toDelete);
    free(list);
}

StringListNode *getNode(StringList *list, int index) {
    if (list->length < index) { return NULL; }
    int i = 0;
    for (StringListNode *current = list->start; current; current = current->next) {
        if (i == index) { return current; }
        i++;
    }
}
