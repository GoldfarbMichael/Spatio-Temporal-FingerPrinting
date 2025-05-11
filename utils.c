#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <mastik/impl.h>


#define OFFSET_MONITOREDHEAD 0
#define OFFSET_NMONITORED 8

LinkedList *create_linked_list() {
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    if (list == NULL) {
        fprintf(stderr, "Memory allocation failed for linked list\n");
        exit(EXIT_FAILURE);
    }
    list->head = nullptr;
    list->tail = nullptr;
    return list;
}

void add_to_list(LinkedList *list, void *data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed for linked list node\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->next = nullptr;

    if (list->head == NULL) {
        // If the list is empty, set both head and tail to the new node
        list->head = newNode;
        list->tail = newNode;
        newNode->next = newNode; // Make it circular

    } else {
        // Add the new node to the tail and update the tail pointer
        list->tail->next = newNode;
        list->tail = newNode;
        newNode->next = list->head;

    }
}


void free_linked_list(LinkedList *list) {
    if (!list || !list->head) {
        free(list);
        return;
    }

    Node *current = list->head;
    Node *nextNode;

    do {
        nextNode = current->next;
        free(current);
        current = nextNode;
    } while (current != list->head); // Stop when we loop back to the head

    free(list);
}

void *getHead(void *l3, int setIndex) {
    // Validate input
    if (!l3) {
        fprintf(stderr, "Error: l3 pointer is NULL.\n");
        return NULL;
    }

    // Access `nmonitored` using its offset
    int *nmonitored_ptr = (int *)((uintptr_t)l3 + OFFSET_NMONITORED);
    int nmonitored = *nmonitored_ptr;

    // Check if setIndex is within bounds
    if (setIndex < 0 || setIndex >= nmonitored) {
        fprintf(stderr, "Error: setIndex is out of bounds. Valid range is 0 to %d.\n", nmonitored - 1);
        return NULL;
    }

    // Access `monitoredhead` using its offset
    void **monitoredhead_ptr = (void **)((uintptr_t)l3 + OFFSET_MONITOREDHEAD);

    // Return the pointer to the head of the linked list at setIndex
    return monitoredhead_ptr[0] + setIndex * sizeof(void *);

}


void traverse_and_print(void **head_ptr) {
    if (!head_ptr) {
        printf("The linked list is empty.\n");
        return;
    }
    void *head = *head_ptr;
    void *current = head;
    do {
        printf("Address: %p\n", current);
        current = LNEXT(current);
    } while (current && current != head); // Continue until we loop back to the head
}


u_int **allocate2DArray(size_t rows, size_t cols) {
    // Allocate memory for an array of pointers to rows
    u_int **array = malloc(rows * sizeof(u_int *));
    if (!array) {
        fprintf(stderr, "Failed to allocate memory for row pointers.\n");
        return NULL;
    }

    // Allocate a contiguous block of memory for all elements
    u_int *data = malloc(rows * cols * sizeof(u_int));
    if (!data) {
        fprintf(stderr, "Failed to allocate memory for data block.\n");
        free(array);
        return nullptr;
    }

    // Assign row pointers
    for (size_t i = 0; i < rows; ++i) {
        array[i] = data + i * cols;
    }

    return array;
}

void free2DArray(u_int **array) {
    if (array) {
        free(array[0]); // free the contiguous block
        free(array);    // free the array of row pointers
    }
}


void write_linkedList_to_csv(LinkedList **groupLinkedList, int numGroups, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return;
    }

    for (int i = 0; i < numGroups; i++) {
        LinkedList *list = groupLinkedList[i];
        if (!list || !list->head) {
            fprintf(file, "Group %d is empty\n", i);
            continue;
        }

        Node *current = list->head;
        do {
            fprintf(file, "%p,", current->data);
            current = current->next;
        } while (current && current != list->head); // Stop when we loop back to the head

        fprintf(file, "\n");
    }

    fclose(file);
}


void write_matrix_to_csv(unsigned int **matrix, size_t rows, size_t cols, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return;
    }

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            fprintf(file, "%u", matrix[i][j]);
            if (j < cols - 1) {
                fprintf(file, ","); // Add a comma between columns
            }
        }
        fprintf(file, "\n"); // Add a newline at the end of each row
    }

    fclose(file);
}

void parse_site_name(const char* url, char* site_name, size_t size) {
    const char* www_ptr = strstr(url, "www.");
    if (!www_ptr) {
        fprintf(stderr, "Invalid URL: 'www.' not found\n");
        return;
    }

    www_ptr += 4; // Move past "www."
    const char* dot_ptr = strchr(www_ptr, '.');
    if (!dot_ptr) {
        fprintf(stderr, "Invalid URL: '.' not found after 'www.'\n");
        return;
    }

    size_t length = dot_ptr - www_ptr;
    if (length >= size) {
        fprintf(stderr, "Site name buffer too small\n");
        return;
    }

    strncpy(site_name, www_ptr, length);
    site_name[length] = '\0'; // Null-terminate the string
}

void log_timings(char *csv_path, const uint64_t start_time, const uint64_t end_time) {

    uint64_t duration = end_time - start_time;
    //append the elapsed time to the CSV file
    FILE *file = fopen(csv_path, "a");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", csv_path);
        return;
    }
    fprintf(file, "%lu\n", duration);

}


uint64_t read_config_long(const char* path, const char* key) {
    FILE *file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Config file not found: %s\n", path);
        return 0; //default value
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char k[128];
        uint64_t v;
        if (sscanf(line, "%127[^=]=%lu", k, &v) == 2) {
            if (strcmp(k, key) == 0) {
                fclose(file);
                return v;
            }
        }
    }

    fclose(file);
    return 0; //default value
}

