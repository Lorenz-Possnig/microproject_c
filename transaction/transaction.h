//
// Created by develop on 10.07.21.
//

#ifndef MICROPROJECT_TRANSACTION_H
#define MICROPROJECT_TRANSACTION_H


#include <stdbool.h>
#define ForEach(element, list) for(TransactionListNode* element = list->start;element;element = element->next)

typedef struct Transaction {
    char* rechtstraeger;
    int quartal;
    int bekanntgabe;
    char* medieninhaber;
    double euro;
} Transaction;

typedef struct TransactionListNode {
    Transaction transaction;
    struct TransactionListNode *next;
} TransactionListNode;

typedef struct TransactionList {
    TransactionListNode *start;
    TransactionListNode *end;
    unsigned int length;
} TransactionList;

void printTransaction(Transaction);

void printTransactionList(TransactionList*);

TransactionList *new__TransactionList();

void freeTransactionNode(TransactionListNode*);

void freeTransactionList(TransactionList*);

void appendTransaction(TransactionList*, Transaction);

void prependTransaction(TransactionList*, Transaction);

bool containsRechtstraeger(TransactionList*, char*);

bool containsMedieninhaber(TransactionList*, char*);

bool containsQuarter(TransactionList*, int);

Transaction *findByRechtstraeger(TransactionList*, char*);

Transaction *findByMedieninhaber(TransactionList*, char*);

void sortByRechtstraeger(TransactionList*);

void sortByMedieninhaber(TransactionList*);

void sortByQuarter(TransactionList*);

void sortByEuro(TransactionList*);

#endif //MICROPROJECT_TRANSACTION_NEW_H
