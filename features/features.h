//
// Created by develop on 11.07.21.
//

#ifndef MICROPROJECT_FEATURES_H
#define MICROPROJECT_FEATURES_H

#include "../transaction/transaction.h"
#define PAYERS "payers"
#define RECIPIENTS "recipients"

void help();

void payers(TransactionList*);

void recipients(TransactionList*);

void quarters(TransactionList*);

void top_payers(TransactionList*, int, int);

void top_recipients(TransactionList*, int, int);

void search_payers(TransactionList*, char*);

void search_recipients(TransactionList*, char*);

void details_payers(TransactionList*, char*);

void details_recipients(TransactionList*, char*);

#endif //MICROPROJECT_FEATURES_H
