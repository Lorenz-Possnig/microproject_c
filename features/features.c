//
// Created by develop on 11.07.21.
//

#include "features.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


void putStrLnIndent(char *s) {
    printf("\t%s\n", s);
}

void help() {
    putStrLnIndent("quit | exit ... quits the program");
    putStrLnIndent("help ... shows this message");
    putStrLnIndent("payers ... prints a sorted list of all payers");
    putStrLnIndent("recipients ... prints a sorted list of all recipients");
    putStrLnIndent("quarters ... prints a list of loaded quarters");
    putStrLnIndent("top n <payers|recipients> <§2|§4|§31> ... prints the top n payers/recipients for given paragraph");
    putStrLnIndent("search <payers|recipients> searchTerm ... prints a list of payers/recipients containing the given search term");
    putStrLnIndent("details <payers|recipients> organization ... prints a list of all payments payed or received by the given payer/recipient");
}

char *getRechtstraeger(TransactionListNode *node) {
    return node->transaction.rechtstraeger;
}

char *getMedieninhaber(TransactionListNode *node) {
    return node->transaction.medieninhaber;
}

void payersOrRecipients(TransactionList *list, bool (contains)(TransactionList*, char*),
                        char *(extractor)(TransactionListNode*), void (sortBy)(TransactionList*), char *por) {
    TransactionList *withoutDuplicates = new__TransactionList();
    ForEach(node, list) {
        if (!contains(withoutDuplicates, extractor(node))) {
            appendTransaction(withoutDuplicates, node->transaction);
        }
    }
    sortBy(withoutDuplicates);
    printf("Printing %d %s:\n", withoutDuplicates->length, por);
    ForEach(node, withoutDuplicates) {
        printf("\t%s\n", extractor(node));
    }
    free(withoutDuplicates);
}

void payers(TransactionList *list) {
    payersOrRecipients(list, containsRechtstraeger, getRechtstraeger, sortByRechtstraeger, PAYERS);
}

void recipients(TransactionList *list) {
    payersOrRecipients(list, containsMedieninhaber, getMedieninhaber, sortByMedieninhaber, RECIPIENTS);
}

void quarters(TransactionList *list) {
    TransactionList *uniqueQuarters = new__TransactionList();
    ForEach(node, list) {
        if (!containsQuarter(uniqueQuarters, node->transaction.quartal))
            appendTransaction(uniqueQuarters, node->transaction);
    }
    sortByQuarter(uniqueQuarters);
    printf("%d quarters loaded:\n", uniqueQuarters->length);
    ForEach(node, uniqueQuarters) {
        printf("\t%d\n", node->transaction.quartal);
    }
    free(uniqueQuarters);
}

void top(TransactionList *list, int amount, int paragraph, char *por, char *(extractor)(TransactionListNode*),
         bool (contains)(TransactionList*, char*), Transaction *(findBy)(TransactionList*, char*)) {
    TransactionList *filteredByParagraph = new__TransactionList();
    ForEach(node, list) {
        if (node->transaction.bekanntgabe == paragraph) {
            appendTransaction(filteredByParagraph, node->transaction);
        }
    }
    TransactionList *summed = new__TransactionList();
    ForEach(node, filteredByParagraph) {
        if (!contains(summed, extractor(node))) {
            appendTransaction(summed, node->transaction);
        } else {
            Transaction *ptr = findBy(summed, extractor(node));
            if (ptr == NULL) {continue;}
            ptr->euro += node->transaction.euro;
        }
    }
    sortByEuro(summed);
    int i = 0;
    if (summed->length < amount) {
        amount = summed->length;
    }
    printf("Listing top %d %s of paragraph §%d\n", amount, por, paragraph);
    ForEach(node, summed) {
        if (i == amount) {break;}
        printf("\t%s - %.2f€\n", node->transaction.rechtstraeger, node->transaction.euro);
        i++;
    }
    free(summed);
    free(filteredByParagraph);
}

void top_payers(TransactionList *list, int amount, int paragraph) {
    top(list, amount, paragraph, PAYERS, getRechtstraeger, containsRechtstraeger, findByRechtstraeger);
}

void top_recipients(TransactionList *list, int amount, int paragraph) {
    top(list, amount, paragraph, RECIPIENTS, getMedieninhaber, containsMedieninhaber, findByMedieninhaber);
}

void search(TransactionList *list, char *searchTerm, char *(extractor)(TransactionListNode*),
            bool contains(TransactionList*, char*), char* por) {
    TransactionList *deduplicated = new__TransactionList();
    ForEach(node, list) {
        if (!contains(deduplicated, extractor(node))) {
            appendTransaction(deduplicated, node->transaction);
        }
    }
    TransactionList *matches = new__TransactionList();
    ForEach(node, deduplicated) {
        char *extracted = extractor(node);
        size_t len = strlen(extracted);
        char *extractedCopy = malloc(sizeof(char) * len);
        for (size_t i = 0; i < len; i++) {
            extractedCopy[i] = tolower((unsigned char) extracted[i]);
        }
        char *matched = strstr(extractedCopy, searchTerm);
        if (matched) {
            appendTransaction(matches, node->transaction);
        }
        free(extractedCopy);
    }
    printf("Found %d %s for %s:\n", matches->length, por, searchTerm);
    ForEach(node, matches) {
        printf("\t%s\n", extractor(node));
    }
    free(matches);
    free(deduplicated);
}

void search_payers(TransactionList *list, char *searchTerm) {
    search(list, searchTerm, getRechtstraeger, containsRechtstraeger, PAYERS);
}

void search_recipients(TransactionList *list, char *searchTerm) {
    search(list, searchTerm, getMedieninhaber, containsMedieninhaber, RECIPIENTS);
}

void sumUp(TransactionList *toSumUp, TransactionList *resultList,
           char *extractor(TransactionListNode*), Transaction *findBy(TransactionList*, char*)) {
    ForEach(node, toSumUp) {
        Transaction *ptr = findBy(resultList, extractor(node));
        if (ptr)
            ptr->euro += node->transaction.euro;
        else
            appendTransaction(resultList, node->transaction);
    }
}

void printDetails(TransactionList *list, char *(extractor)(TransactionListNode*)) {
    ForEach(node, list) {
        printf("\t%-25s - %8.2f€\n", extractor(node), node->transaction.euro);
    }
}

void details(TransactionList *list, char* organization, char *(extractor)(TransactionListNode*),
             char *(reverseExtractor)(TransactionListNode*), Transaction *(findBy)(TransactionList*, char*)) {
    // get all transactions of organization
    TransactionList *paymentsOfOrg = new__TransactionList();
    ForEach(node, list) {
        // make lowercase copy of organization to match lowercase user input
        char *extracted = extractor(node);
        size_t len = strlen(extracted);
        char *extractedCopy = malloc(sizeof(char) * len);
        for (size_t i = 0; i < len; i++)
            extractedCopy[i] = tolower((unsigned char) extracted[i]);
        if (strcmp(organization, extractedCopy) == 0) {
            appendTransaction(paymentsOfOrg, node->transaction);
        }
        //free(extractedCopy);
    }
    // group them by bekanntgabe
    TransactionList *p2 = new__TransactionList();
    TransactionList *p4 = new__TransactionList();
    TransactionList *p31 = new__TransactionList();

    ForEach(node, paymentsOfOrg) {
        switch (node->transaction.bekanntgabe) {
            case 2: { appendTransaction(p2, node->transaction); break; }
            case 4: { appendTransaction(p4, node->transaction); break; }
            case 31: { appendTransaction(p31, node->transaction); break; }
            default: { printf("Found wrong value for bekanntgabe: %d\n", node->transaction.bekanntgabe); }
        }
    }
    
    /* sum up the reverse of extractor
     * if we want payers sum up recipients
     * else sum up payers
     */
    TransactionList *p2Summed = new__TransactionList();
    TransactionList *p4Summed = new__TransactionList();
    TransactionList *p31Summed = new__TransactionList();
    sumUp(p2, p2Summed, reverseExtractor, findBy);
    sumUp(p4, p4Summed, reverseExtractor, findBy);
    sumUp(p31, p31Summed, reverseExtractor, findBy);

    // sort by euro
    sortByEuro(p2Summed);
    sortByEuro(p4Summed);
    sortByEuro(p31Summed);
    
    printf("Payments according to §2:\n");
    printDetails(p2Summed, reverseExtractor);
    printf("Payments according to §4:\n");
    printDetails(p4Summed, reverseExtractor);
    printf("Payments according to §31:\n");
    printDetails(p31Summed, reverseExtractor);
    
    free(paymentsOfOrg);
    free(p2);
    free(p4);
    free(p31);
    free(p2Summed);
    free(p4Summed);
    free(p31Summed);
}

void details_payers(TransactionList *list, char *organization) {
    details(list, organization, getRechtstraeger, getMedieninhaber, findByMedieninhaber);
}

void details_recipients(TransactionList *list, char *organization) {
    details(list, organization, getMedieninhaber, getRechtstraeger, findByRechtstraeger);
}