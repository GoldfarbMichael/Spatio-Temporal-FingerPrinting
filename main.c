//add main function
#include <stdio.h>
#include <stdlib.h>
#include <mastik/l3.h>
#include <mastik/impl.h>
#include <mastik/util.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#define SETS_PER_SLICE 1024

#define NUM_OF_GROUPS 1024

void write_groupList_to_csv(void **groupList, int numGroups, int groupSize, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    for (int row = 0; row < groupSize; row++) {
        for (int col = 0; col < numGroups; col++) {
            if (col > 0) {
                fprintf(file, ",");
            }
            fprintf(file, "%p", ((void **)groupList[col])[row]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
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

void create_groups_list(l3pp_t l3, int groupSize, int subgroupSize, void** groupList, int associativity) {
    for (int groupNum = 0; groupNum < NUM_OF_GROUPS; groupNum++) {
        monitor_sets(l3, groupSize, groupNum, l3_getSlices(l3));
        for (int j = 0; j < groupSize; j++ ) {
            add_addresses_to_arr(getHead(l3, j),groupList[groupNum], j* subgroupSize* associativity);
        }
    }
}


int main(void) {

    delayloop(3000000000U);
    l3info_t l3i = (l3info_t)malloc(sizeof(struct l3info));
    printf("preparing...\n");
    l3pp_t l3 = l3_prepare(l3i, NULL);
    printf("Num of slices: %d\n", l3_getSlices(l3));
    int setsGoal = l3_getSlices(l3) * SETS_PER_SLICE;
    printf("Num of sets Goal: %d\n", setsGoal);

    while (l3_getSets(l3) != setsGoal) { // continue until all the cache is mapped
        l3 = l3_prepare(l3i, NULL);
    }
    int associativity = l3_getAssociativity(l3);

    int groupSize = l3_getSets(l3) / NUM_OF_GROUPS;
    int numOfSets = l3_getSets(l3);
    int subgroupSize = groupSize / l3_getSlices(l3);
    printf("Number of sets: %d, sub Size: %d\n", numOfSets, subgroupSize);
    printf("%d Way cache\n", associativity);
    void **groupList = (void**) calloc(NUM_OF_GROUPS, sizeof(void*)); //allocate array of pointers of groups

    // Allocate memory for each cell in groupList
    for (int i = 0; i < NUM_OF_GROUPS; i++) {
        groupList[i] = (void *)calloc(groupSize * associativity, sizeof(void *));
        if (groupList[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for groupList[%d]\n", i);
            exit(EXIT_FAILURE);
        }
    }

    create_groups_list(l3, groupSize, subgroupSize, groupList, associativity);

    write_groupList_to_csv(groupList, NUM_OF_GROUPS, groupSize * associativity, "output.csv");
    // l3_monitor(l3, 0);
    // l3_monitor(l3, 1024);

    // traverse_and_print(getHead(l3, 0));
    //
    // traverse_and_print(getHead(l3, 1));
    // printf("monitoring...\n");
    // monitor_sets(&l3, 24, 0, l3_getSlices(l3));
    // printf("\n");
    // monitor_sets(&l3, 24, 1, l3_getSlices(l3));
    // printf("\n");
    // monitor_sets(&l3, 24, 2, l3_getSlices(l3));

    for (int i = 0; i < NUM_OF_GROUPS; i++) {
        if (groupList[i] != NULL) {
            free(groupList[i]);
            groupList[i] = NULL; // Avoid double free
        }
    }
    free(groupList);
    free(l3i);
    l3_release(l3);
    return 0;
}
