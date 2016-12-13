#include <stdio.h>
#include <time.h>
#include <sys/time.h>

int main(void) {
    double time_now;
    struct timespec time_tmp;
    while(1){
        clock_gettime(CLOCK_REALTIME_COARSE, &time_tmp);
        time_now = (int)time_tmp.tv_sec + (double)time_tmp.tv_nsec * 0.000000001;
        printf("%f\n",time_now);
    }
    return 0;
}
