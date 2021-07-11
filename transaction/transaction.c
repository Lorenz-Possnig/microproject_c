#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "transaction.h"

TransactionListNode *new__Transaction_Node(Transaction transaction) {
    TransactionListNode *result = malloc(sizeof(TransactionListNode));
    result->next = NULL;
    result->transaction = transaction;
    return result;
}

void printTransaction(Transaction transaction) {
    printf("Transaction(Rechtstraeger: %s, Quartal: %d, Bekanntgabe: %d, Medieninhaber: %s, Euro: %.2f)",
           transaction.rechtstraeger, transaction.quartal, transaction.bekanntgabe,
           transaction.medieninhaber, transaction.euro);
}

void printTransactionList(TransactionList *list) {
    printf("Transactions {");
    printTransaction(list->start->transaction);
    ForEach(node, list) {
        printf(", ");
        printTransaction(node->transaction);
    }
    printf("}");
}

TransactionList *new__TransactionList() {
    TransactionList *result = malloc(sizeof(TransactionList));
    result->start = NULL;
    result->end = NULL;
    result->length = 0;
    return result;
}

void appendTransaction(TransactionList *list, Transaction transaction) {
    TransactionListNode *node = new__Transaction_Node(transaction);
    if (list->length == 0) {
        list->start = node;
        list->end = node;
        list->length ++;
    } else {
        list->end->next = node;
        list->end = node;
        list->length++;
    }
}

void prependTransaction(TransactionList *list, Transaction transaction) {
    TransactionListNode *node = new__Transaction_Node(transaction);
    if (list->length == 0) {
        list->start = node;
        list->end = node;
        list->length++;
    } else {
        node->next = list->start;
        list->start = node;
    }
}

bool containsRechtstraeger(TransactionList *list, char* rechtstraeger) {
    ForEach(node, list) {
        if (strcmp(node->transaction.rechtstraeger, rechtstraeger) == 0) {
            return true;
        }
    }
    return false;
}

bool containsMedieninhaber(TransactionList *list, char* medieninhaber) {
    ForEach(node, list) {
        if (strcmp(node->transaction.medieninhaber, medieninhaber) == 0) {
            return true;
        }
    }
    return false;
}

bool containsQuarter(TransactionList *list, int quarter) {
    ForEach(node, list) {
        if (node->transaction.quartal == quarter)
            return true;
    }
    return false;
}

Transaction *findByRechtstraeger(TransactionList *list, char* rechtstraeger) {
    ForEach(node, list) {
        if (strcmp(node->transaction.rechtstraeger, rechtstraeger) == 0) {
            return &node->transaction;
        }
    }
    return NULL; // should not happen if you check with contains first
}

Transaction *findByMedieninhaber(TransactionList *list, char* medieninhaber) {
    ForEach(node, list) {
        if (strcmp(node->transaction.medieninhaber, medieninhaber) == 0) {
            return &node->transaction;
        }
    }
    return NULL; // should not happen if you check with contains first
}

void swap(TransactionListNode *a, TransactionListNode *b) {
    Transaction tmp = a->transaction;
    a->transaction = b->transaction;
    b->transaction = tmp;
}

void sortByRechtstraeger(TransactionList *list) {
    // bubble sort because fuck lists
    if (list->start == NULL) { return; }
    bool swapped = false;
    TransactionListNode *ptr1;

    do {
        swapped = false;
        ptr1 = list->start;
        while (ptr1->next != NULL) {
            // make lowercase copies of the strings
            int n = strlen(ptr1->transaction.rechtstraeger);
            char* transactionCopy = malloc(n + 1);
            int n2 = strlen(ptr1->next->transaction.rechtstraeger);
            char* transactionNextCopy = malloc(n2 + 1);

            strcpy(transactionCopy, ptr1->transaction.rechtstraeger);
            strcpy(transactionNextCopy, ptr1->next->transaction.rechtstraeger);
            for (int i = 0; i < n; i++)
                transactionCopy[i] = tolower((unsigned char) transactionCopy[i]);
            for (int i = 0; i < n2; i++)
                transactionNextCopy[i] = tolower((unsigned char) transactionNextCopy[i]);

            // compare copies and swap
            if (strcmp(transactionCopy, transactionNextCopy) > 0) {
                swap(ptr1, ptr1->next);
                swapped = true;
            }
            // free strings
            free(transactionCopy);
            free(transactionNextCopy);

            // advance pointer
            ptr1 = ptr1->next;
        }
    } while (swapped);
}

void sortByMedieninhaber(TransactionList *list) {
    if (list->start == NULL) { return; }
    bool swapped = false;
    TransactionListNode *ptr1;

    do {
        swapped = false;
        ptr1 = list->start;
        while (ptr1->next != NULL) {
            // make lowercase copies of the strings
            int n = strlen(ptr1->transaction.medieninhaber);
            char* transactionCopy = malloc(n + 1);
            int n2 = strlen(ptr1->next->transaction.medieninhaber);
            char* transactionNextCopy = malloc(n2 + 1);

            strcpy(transactionCopy, ptr1->transaction.medieninhaber);
            strcpy(transactionNextCopy, ptr1->next->transaction.medieninhaber);
            for (int i = 0; i < n; i++)
                transactionCopy[i] = tolower((unsigned char) transactionCopy[i]);
            for (int i = 0; i < n2; i++)
                transactionNextCopy[i] = tolower((unsigned char) transactionNextCopy[i]);

            // compare copies and swap
            if (strcmp(transactionCopy, transactionNextCopy) > 0) {
                swap(ptr1, ptr1->next);
                swapped = true;
            }
            // free strings
            free(transactionCopy);
            free(transactionNextCopy);

            // advance pointer
            ptr1 = ptr1->next;
        }
    } while (swapped);
}

void sortByQuarter(TransactionList *list) {
    if (list->start == NULL) { return; }
    bool swapped = false;
    TransactionListNode *ptr1;

    do {
        swapped = false;
        ptr1 = list->start;
        while (ptr1->next != NULL) {
            // compare copies and swap
            if (ptr1->transaction.quartal > ptr1->next->transaction.quartal) {
                swap(ptr1, ptr1->next);
                swapped = true;
            }
            // advance pointer
            ptr1 = ptr1->next;
        }
    } while (swapped);
}

void sortByEuro(TransactionList *list) {
    if (list->start == NULL) { return; }
    bool swapped = false;
    TransactionListNode *ptr1;

    do {
        swapped = false;
        ptr1 = list->start;
        while (ptr1->next != NULL) {
            // compare copies and swap
            if (ptr1->transaction.euro < ptr1->next->transaction.euro) {
                swap(ptr1, ptr1->next);
                swapped = true;
            }
            // advance pointer
            ptr1 = ptr1->next;
        }
    } while (swapped);
}