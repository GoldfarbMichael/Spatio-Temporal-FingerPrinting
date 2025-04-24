#define _GNU_SOURCE
#include <sched.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bits/time.h>
#include <mastik/l3.h>
#include "utils.h"
#include "map-preprocess.h"

#define INTERVAL_NORMALIZER 1000000 // 1 -> 1sec | 1000 -> 1ms | 1000000 -> microSec
#define PROBE_TIME_SEC 5 // probe time in seconds
#define INTERVAL_PROBE_MS 2 // the interval time in ms
#define NUM_OF_SAMPLES 64
#define NORMALIZED_INTERVAL (2600000000 / INTERVAL_NORMALIZER)

void pin_to_core(int core_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) != 0) {
        perror("sched_setaffinity");
    }
}

int open_website(const char* url) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return 0;
    }

    if (pid == 0) {
        setpgid(0, 0);
        // Child process: open browser in incognito mode
        pin_to_core(2);
        execlp("google-chrome", "google-chrome",
              "--new-window",
              url, NULL);
        perror("execlp failed"); // if execlp returns
        exit(EXIT_FAILURE);
    }

    // Parent continues
    return pid; // return child PID
}

void probe_group(const LinkedList *groupLinkedList, uint **sampleMatrix ,uint64_t Interval, int groupNum, int sampleNum) {
    const uint64_t limit = rdtscp64() + Interval;
    Node *current = groupLinkedList->head;
    int counter = 0;
    while (rdtscp64() < limit) {
        current = current->next;
        counter++;
    }
    sampleMatrix[groupNum][sampleNum] = counter;
}

void fill_matrix(SpatialInfo *si, uint **sampleMatrix) {
    for (int sampleNum = 0; sampleNum < NUM_OF_SAMPLES; sampleNum++) {
        for (int groupNum = 0; groupNum < si->numOfGroups; groupNum++) {
            probe_group(si->groupLinkedList[groupNum], sampleMatrix, NORMALIZED_INTERVAL*50, groupNum, sampleNum);
        }
    }
}

void collect_data(SpatialInfo *si, uint **sampleMatrix, const char* url) {
    char site_name[128];
    parse_site_name(url, site_name, sizeof(site_name));
    //print the site name
    printf("Probing site: %s\n", site_name);
    pid_t browser_pid = open_website(url);
    if (browser_pid == 0) {
        free2DArray(sampleMatrix);
        return;
    }

    fill_matrix(si, sampleMatrix);
    printf("Done.\n");

    kill(-browser_pid, SIGKILL);
    waitpid(browser_pid, NULL, 0);

    // Write results to CSV
    char csv_path[256];
    snprintf(csv_path, sizeof(csv_path), "%s.csv", site_name);
    write_matrix_to_csv(sampleMatrix, si->numOfGroups, NUM_OF_SAMPLES, csv_path);

}

int main(void) {
    pin_to_core(0);
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    const char* urlWiki = "https://www.wikipedia.org";
    const char* urlBBC = "https://www.bbc.com/";
    SpatialInfo *si = (SpatialInfo *) malloc(sizeof(SpatialInfo));
    LinkedList **groupLinkedList = (LinkedList **) calloc(NUM_OF_GROUPS, sizeof(LinkedList *));

    build_SpatialInfo(si, groupLinkedList);
    create_groups_list(*si, groupLinkedList);
    write_linkedList_to_csv(groupLinkedList, NUM_OF_GROUPS, "linked_list_output.csv");

    uint **sampleMatrix = allocate2DArray(si->numOfGroups, NUM_OF_SAMPLES);
    printf("collecting data...\n");
    collect_data(si, sampleMatrix, urlWiki);
    collect_data(si, sampleMatrix, urlBBC);




    clock_gettime(CLOCK_MONOTONIC, &end);
    // Calculate elapsed time in seconds and nanoseconds
    double elapsed_time = (end.tv_sec - start.tv_sec) +
                          (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Execution time: %.9f seconds\n", elapsed_time);
    for (int i = 0; i < NUM_OF_GROUPS; i++) {
        if (groupLinkedList[i] != NULL) {
            free_linked_list(groupLinkedList[i]);
            groupLinkedList[i] = NULL; // Avoid double free
        }
    }
    free(groupLinkedList);
    l3_release(si->l3);
    // free2DArray(sampleMatrix);
    return 0;
}
