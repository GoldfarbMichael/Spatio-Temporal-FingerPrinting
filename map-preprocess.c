#include "map-preprocess.h"
#include <stdio.h>
#include <stdlib.h>
#include <mastik/impl.h>
#include <mastik/util.h>
#include "utils.h"

void allocate_lists(LinkedList **linkedList) {
    for (int i = 0; i < NUM_OF_GROUPS; i++) {
        linkedList[i] = create_linked_list();
        if (linkedList[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for groupList[%d]\n", i);
            exit(EXIT_FAILURE);
        }
    }
}

void build_SpatialInfo(SpatialInfo *spatialInfo, LinkedList **linkedList) {
    delayloop(3000000000U);
    l3info_t l3i = (l3info_t)malloc(sizeof(struct l3info));
    printf("preparing...\n");
    l3pp_t l3 = l3_prepare(l3i, nullptr);
    printf("Num of slices: %d\n", l3_getSlices(l3));
    int setsGoal = l3_getSlices(l3) * SETS_PER_SLICE;
    printf("Num of sets Goal: %d\n", setsGoal);

    while (l3_getSets(l3) != setsGoal) { // continue until all the cache is mapped
        l3 = l3_prepare(l3i, nullptr);
    }
    spatialInfo->numOfGroups = NUM_OF_GROUPS;
    spatialInfo->groupSize = l3_getSets(l3) / NUM_OF_GROUPS;
    spatialInfo->groupLinkedList = linkedList;
    spatialInfo->l3 = l3;
    printf("Number of sets: %d, sub Size: %d\n", l3_getSets(l3), spatialInfo->groupSize/l3_getSlices(l3));
    printf("%d Way cache\n", l3_getAssociativity(l3));
    allocate_lists(linkedList);
    free(l3i);
}

void monitor_sets(l3pp_t l3, int groupSize, int groupNum, int numOfSlices) {
    if (groupSize < numOfSlices) {
        printf("Group size is too small, must be at least as the num of slices\n");
        return;
    }
    l3_unmonitorall(l3);
    int groupPerSlice = groupSize / numOfSlices;
    int baseSetNum = groupNum * groupPerSlice;
    for (int sliceNum = 0; sliceNum < numOfSlices; sliceNum++) {
        for (int i = 0; i < groupPerSlice; i++) {
            int setIndex = baseSetNum + i + sliceNum*SETS_PER_SLICE;
            l3_monitor(l3, setIndex);
            // printf("Monitoring set %d in slice %d\n", setIndex, sliceNum);
        }
    }
}

void add_eviction_set_to_list(LinkedList *list, void **evictionHead) {
    if (!evictionHead) {
        printf("The eviction set is empty.\n");
        return;
    }
    void *head = *evictionHead;
    void *current = head;
    do {
        add_to_list(list, current);
        current = LNEXT(current);
    } while (current && current != head); // Continue until we loop back to the head
}

/**
 *  Creates Linked List of groups of addresses
 */
void create_groups_list(SpatialInfo si, LinkedList **linkedList) {
    for (int groupNum = 0; groupNum < NUM_OF_GROUPS; groupNum++) {
        monitor_sets(si.l3, si.groupSize, groupNum, l3_getSlices(si.l3));
        for (int j = 0; j < si.groupSize; j++ ) {
            add_eviction_set_to_list(linkedList[groupNum], getHead(si.l3, j));
        }
    }
}
