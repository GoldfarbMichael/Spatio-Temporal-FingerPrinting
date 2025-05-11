#ifndef MAP_PREPROCESS_H
#define MAP_PREPROCESS_H

#define SETS_PER_SLICE 1024
#define NUM_OF_GROUPS 128
#include "utils.h"
#include <stddef.h>
#include <mastik/l3.h>

typedef struct SpatialInfo{
    int numOfGroups;
    int groupSize;
    l3pp_t l3;
    LinkedList **groupLinkedList;
}SpatialInfo;

void allocate_lists(LinkedList **linkedList);
void build_SpatialInfo(SpatialInfo *spatialInfo, LinkedList **linkedList);
void monitor_sets(l3pp_t l3, int groupSize, int groupNum, int numOfSlices);
void add_eviction_set_to_list(LinkedList *list, void **evictionHead);
void create_groups_list(SpatialInfo si, LinkedList **linkedList);
void log_prepare_time(char *csv_path);

#endif //MAP_PREPROCESS_H
