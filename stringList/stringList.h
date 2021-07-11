//
// Created by develop on 10.07.21.
//

#ifndef MICROPROJECT_STRINGLIST_H
#define MICROPROJECT_STRINGLIST_H

typedef struct StringListNode {
    char* string;
    struct StringListNode *next;
} StringListNode;

typedef struct StringList {
    StringListNode *start;
    StringListNode *end;
    unsigned int length;
} StringList;

StringList *new__StringList();

void appendString(StringList*, char*);

void printStrings(StringList*);

char *get(StringList*, int);

void freeStringList(StringList*);

StringListNode *getNode(StringList*, int);

#endif //MICROPROJECT_STRINGLIST_H
