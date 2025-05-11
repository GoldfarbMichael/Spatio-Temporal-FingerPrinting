#ifndef MAP_PREPROCESS_H
#define MAP_PREPROCESS_H


#include "utils.h"
#include <stddef.h>
#include <mastik/l3.h>

typedef struct SpatialInfo{
    int numOfGroups;
    int groupSize;
    l3pp_t l3;
    LinkedList **groupLinkedList;
}SpatialInfo;

void allocate_lists(LinkedList **linkedList, const int numOfGroups);
void build_SpatialInfo(SpatialInfo *spatialInfo, LinkedList **linkedList, const int setsPerSlice, const int numOfGroups);
void monitor_sets(l3pp_t l3, int groupSize, int groupNum, int numOfSlices, int setsPerSlice);
void add_eviction_set_to_list(LinkedList *list, void **evictionHead);
void create_groups_list(SpatialInfo si, LinkedList **linkedList, const int numOfGroups, const int setsPerSlice);
void log_prepare_time(char *csv_path, const int setsPerSlice);
#endif //MAP_PREPROCESS_H
