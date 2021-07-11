#include <stdio.h>
#include <stdlib.h>
#include "transaction/transaction.h"
#include <curl/curl.h>
#include <string.h>
#include "cJSON/cJSON.h"
#include <stdbool.h>
#include <ctype.h>
#include "stringList/stringList.h"
#include "features/features.h"

#define ForEach(element, list) for(TransactionListNode* element = list->start;element;element = element->next)

#define URL_LENGTH 92

const char *DELIMITER = " ";

typedef struct {
    char* ptr;
    size_t len;
} string;

void initString(string* s) {
    s->len = 0;
    s->ptr = malloc(s->len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t gotData(void* ptr, size_t itemsize, size_t nItems, string* s) {
    size_t bytes = itemsize * nItems;
    size_t newStringSize = s->len + bytes;
    s->ptr = realloc(s->ptr, newStringSize + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr+s->len, ptr, itemsize * nItems);
    s->ptr[newStringSize] = '\0';
    s->len = newStringSize;

    return bytes;
}

TransactionList* loadData(char* quarter) {
    TransactionList* list = new__TransactionList();
    if (strlen(quarter) != 5) {
        printf("Entered quarter has wrong length\n");
        return list;
    }
    CURL* curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "curl init failed\n");
        exit(EXIT_FAILURE);
    }
    string s;
    initString(&s);

    char* url[URL_LENGTH];
    strcpy(url, "https://data.rtr.at/api/v1/tables/MedKFTGBekanntgabe.json?quartal=");
    strcat(url, quarter);
    strcat(url, "&leermeldung=0&size=0");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, gotData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        fprintf(stderr, "download problem: %s\n", curl_easy_strerror(result));
        exit(EXIT_FAILURE);
    }
    cJSON* data = NULL;
    cJSON* wholeJson = cJSON_Parse(s.ptr);
    if (wholeJson == NULL) {
        const char* error = cJSON_GetErrorPtr();
        if (error != NULL) {
            fprintf(stderr, "Error before: %s\n", error);
            exit(EXIT_FAILURE);
        }
    }
    data = cJSON_GetObjectItemCaseSensitive(wholeJson, "data");
    const cJSON* transaction = NULL;
    cJSON_ArrayForEach(transaction, data) {
        cJSON* rechtstraeger = cJSON_GetObjectItemCaseSensitive(transaction, "rechtstraeger");
        cJSON* quartal = cJSON_GetObjectItemCaseSensitive(transaction, "quartal");
        cJSON* bekanntgabe = cJSON_GetObjectItemCaseSensitive(transaction, "bekanntgabe");
        cJSON* medieninhaber = cJSON_GetObjectItemCaseSensitive(transaction, "mediumMedieninhaber");
        cJSON* euro = cJSON_GetObjectItemCaseSensitive(transaction, "euro");
        if (!cJSON_IsString(rechtstraeger) ||
            !cJSON_IsString(quartal)||
            !cJSON_IsNumber(bekanntgabe) ||
            !cJSON_IsString(medieninhaber) ||
            !cJSON_IsNumber(euro)) {
            const char* error = cJSON_GetErrorPtr();
            if (error != NULL) {
                fprintf(stderr, "Failed parsing entries at: %s", error);
                continue;
            }
        } else {
            Transaction transactionToAdd = {.quartal = atoi(quartal->valuestring),
                                            .bekanntgabe = bekanntgabe->valueint,
                                            .euro = euro->valuedouble};
            char* rechtstraegerCopy = malloc(strlen(rechtstraeger->valuestring) + 1);
            strcpy(rechtstraegerCopy, rechtstraeger->valuestring);
            transactionToAdd.rechtstraeger = rechtstraegerCopy;
            char* medieninhaberCopy = malloc(strlen(medieninhaber->valuestring) + 1);
            strcpy(medieninhaberCopy, medieninhaber->valuestring);
            transactionToAdd.medieninhaber = medieninhaberCopy;
            appendTransaction(list, transactionToAdd);
        }
    }
    s.len = 0;
    free(s.ptr);
    cJSON_Delete(wholeJson);
    curl_easy_cleanup(curl);
    return list;
}

int main() {

    char *line = NULL;
    size_t len = 0;
    ssize_t lineSize = 0;


    printf("Please enter the quarter to load\n");
    lineSize = getline(&line, &len, stdin);
    char *quarterToLoad = malloc(sizeof(char) * 6);
    for (int i = 0; i < 5; i++)
        quarterToLoad[i] = line[i];
    quarterToLoad[5] = '\0';
    TransactionList* data = loadData(quarterToLoad);
    free(quarterToLoad);

    bool isRunning = true;

    while (isRunning) {
        // get command
        printf("Please enter a command or type 'help' for more information\n");
        lineSize = getline(&line, &len, stdin);

        // set to lowercase
        for (size_t i = 0; i < strlen(line); i++) {
            line[i] = tolower((unsigned char) line[i]);
        }

        // split on space
        char *commandPtr;
        StringList *commandList = new__StringList();
        commandPtr = strtok(line, DELIMITER);
        while (commandPtr != NULL) {
            appendString(commandList, commandPtr);
            commandPtr = strtok(NULL, DELIMITER);
        }

        // parse command
        char *commandHead = get(commandList, 0);
        if (strncmp(commandHead, "quit", 4) == 0 ||
            strncmp(commandHead, "exit", 4) == 0) {
            isRunning = false;
            printf("Bye!\n");
        } else if (strncmp(commandHead, "help", 4) == 0) {
            help();
        } else if (strncmp(commandHead, "payers", 6) == 0) {
            payers(data);
        } else if (strncmp(commandHead, "recipients", 10) == 0) {
            recipients(data);
        } else if (strncmp(commandHead, "load", 4) == 0) {
            if (commandList->length < 2) {
                printf("Please enter a quarter you want to load\n");
                continue;
            }
            for (StringListNode *current = getNode(commandList, 1); current; current = current->next) {
                char *quarterStr = current->string;
                char *cpy = malloc(sizeof(char) * 6);
                for (int i = 0; i < 5; i++)
                    cpy[i] = quarterStr[i];
                cpy[5] = '\0';
                TransactionList *newData = loadData(cpy);
                data->length += newData->length;
                data->end->next = newData->start;
                data->end = newData->end;
                newData->length = 0;
                free(quarterStr);
                free(cpy);
                free(newData);
            }
        } else if (strncmp(commandHead, "quarters", 8) == 0) {
            quarters(data);
        } else if (strncmp(commandHead, "top", 3) == 0) {
            if (commandList->length != 4) {
                printf("Wrong syntax for command top\n");
                continue;
            }
            char *amountStr = get(commandList, 1);
            char *end;
            int amount = (int) strtol(amountStr, end, 10);
            char *por = get(commandList, 2);
            char *paragraphStr = get(commandList, 3);
            char paragraphFiltered[2];
            int i = 0;
            for (int j = 0; j < 4; j++) {
                if (paragraphStr[j] > '0' && paragraphStr[j] < '9') {
                    paragraphFiltered[i] = paragraphStr[j];
                    i++;
                }
            }
            int paragraph = (int) strtol(paragraphFiltered, end, 10);
            if (strcmp(por, PAYERS) == 0) {
                top_payers(data, amount, paragraph);
            } else if (strcmp(por, RECIPIENTS) == 0) {
                top_recipients(data, amount, paragraph);
            } else {
                printf("Wrong syntax for command top\n");
            }
        } else if (strcmp(commandHead, "search") == 0) {
            // check minimum length of commands
            if (commandList->length < 3) {
                printf("Wrong syntax for command search\n");
                continue;
            }
            // build search term string
            // find out length of string
            size_t searchTermLength = commandList->length;
            for (StringListNode* current = getNode(commandList, 2); current; current = current->next) {
                searchTermLength += strlen(current->string);
            }
            // concat list to string
            char *searchTerm = malloc(sizeof(char) * searchTermLength);
            strcpy(searchTerm, getNode(commandList, 2)->string);
            for (StringListNode *current = getNode(commandList, 3); current; current = current->next) {
                strcat(searchTerm, " ");
                strcat(searchTerm, current->string);
            }
            // set the newline char to null terminator
            for (int i = 0; i < searchTermLength; i++) {
                if (searchTerm[i] == '\n') {
                    searchTerm[i] = '\0';
                }
            }

            // check weather to search payers or recipients
            char *por = get(commandList, 1);
            if (strcmp(por, PAYERS) == 0) {
                search_payers(data, searchTerm);
            } else if (strcmp(por, RECIPIENTS) == 0) {
                search_recipients(data, searchTerm);
            } else {
                printf("Wrong syntax for command search\n");
            }
            // free the allocated search string
            free(searchTerm);
        } else if (strncmp(commandHead, "details", 7) == 0) {
            if (commandList->length < 3) {
                printf("Wrong syntax for command details\n");
                continue;
            }
            size_t orgLength = commandList->length;
            for (StringListNode* current = getNode(commandList, 2); current; current = current->next) {
                orgLength += strlen(current->string);
            }
            char *organization = malloc(sizeof(char) * orgLength);
            strcpy(organization, getNode(commandList, 2)->string);
            for (StringListNode *current = getNode(commandList, 3); current; current = current->next) {
                strcat(organization, " ");
                strcat(organization, current->string);
            }
            // set the newline char to null terminator
            for (int i = 0; i < orgLength; i++) {
                if (organization[i] == '\n') {
                    organization[i] = '\0';
                }
            }
            char *por = get(commandList, 1);
            if (strcmp(por, PAYERS) == 0) {
                details_payers(data, organization);
            } else if (strcmp(por, RECIPIENTS) == 0) {
                details_recipients(data, organization);
            } else {
                printf("Wrong syntax for command search\n");
            }
        } else {
            printf("Unrecognized command\n");
        }
    }
    return EXIT_SUCCESS;
}
