#ifndef UTILS_H
#define UTILS_H

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

void add_addresses_to_arr(void **head_ptr, void **arr, int baseIndex);

#endif //UTILS_H
