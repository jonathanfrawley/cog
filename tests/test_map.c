#include "cog_log.h"
#include "cog_map.h"

//#include <linux/time.h>
#include <sys/time.h>
//#include <time.h>

typedef struct test_data {
    int32_t number;
} test_data;

int main(int argc, char* argv[]) {
    cog_map map;
    cog_map_init(&map);
    test_data test;

    int32_t n_elems = 10000;

    struct timeval t1, t2;
    double elapsed_time;

    gettimeofday(&t1, 0);
    for(int i=0;i<n_elems;i++) {
        cog_map_put(&map, i, (void*)&test);
    }
    gettimeofday(&t2, 0);

    // compute and print the elapsed time in millisec
    elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    cog_debugf("time taken to insert %d elements : %lf ms ", n_elems, elapsed_time);

    int32_t get_cnts = 15846905;
    gettimeofday(&t1, 0);
    for(int i=0;i<get_cnts;i++) {
        cog_map_get(&map, i%n_elems);
    }
    gettimeofday(&t2, 0);
    elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    cog_debugf("time taken to get %d elements : %lf ms ", get_cnts, elapsed_time);

}
