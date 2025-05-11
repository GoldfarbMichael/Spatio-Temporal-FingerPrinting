#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>
#include <stdlib.h>

// Node struct for the linked list
typedef struct Node {
    void *data;
    struct Node *next;
} Node;

// LinkedList struct with head and tail pointers
typedef struct LinkedList {
    Node *head;
    Node *tail;
} LinkedList;


/*
 * creates a linked list assigns head and tail to be NULL
 */
LinkedList *create_linked_list();

/*
 * Function to add a node to the linked list
 * Adds the node to the tail of the linked list
 */
void add_to_list(LinkedList *list, void *data);

/*
 * Function to free the linked list
 * Frees all nodes in the linked list
 */
void free_linked_list(LinkedList *list);

/*
 *returns the head of the linked list at setIndex
 *linked list of addresses
 *the linked list is a circular linked list
*/
void *getHead(void *l3, int setIndex);


/*
 * Traverse the linked list and print the addresses
 * Assumes the linked list is circular
 */
void traverse_and_print(void **head_ptr);

u_int **allocate2DArray(size_t rows, size_t cols);
void free2DArray(u_int **array);

void write_linkedList_to_csv(LinkedList **groupLinkedList, int numGroups, const char *filename);
void write_matrix_to_csv(unsigned int **matrix, size_t rows, size_t cols, const char *filename);
void parse_site_name(const char* url, char* site_name, size_t size);
void log_timings(char *csv_path, const uint64_t start_time, const uint64_t end_time);
uint64_t read_config_long(const char* path, const char* key);

static inline int read_config_int(const char* path, const char* key) {
    return (int) read_config_long(path, key);
}


#endif //UTILS_H
