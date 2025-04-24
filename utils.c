#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <mastik/impl.h>


#define OFFSET_MONITOREDHEAD 0
#define OFFSET_NMONITORED 8

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

void add_addresses_to_arr(void **head_ptr, void **arr, int baseIndex) {
    if (!head_ptr) {
        printf("The linked list is empty.\n");
        return;
    }
    int i = 0 + baseIndex;
    void *head = *head_ptr;
    void *current = head;
    do {
        arr[i] = current;
        i++;
        current = LNEXT(current);
    } while (current && current != head); // Continue until we loop back to the head
}
