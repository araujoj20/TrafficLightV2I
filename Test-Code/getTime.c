#include <stdio.h>
#include <sys/time.h>
#include <time.h>

int main() {
    struct timeval tmVal;
    gettimeofday(&tmVal, NULL);

    struct tm *tmInfo;
    tmInfo = localtime(&tmVal.tv_sec);

    int minutesYear = (tmInfo->tm_yday * 24 * 60) + (tmInfo->tm_hour * 60) + tmInfo->tm_min;

    int secondsHour = tmInfo->tm_min * 60 + tmInfo->tm_sec;

    int tenthsSecond = (tmVal.tv_usec / 100000) % 10;

    printf("Minutes of the Year : %d\n", minutesYear);
    printf("Seconds of the Hour : %d\n", secondsHour);
    //printf("Tenths of a Second: %d\n", tenthsSecond);
    printf("Tenths of a second in relation to the Hour: %d\n", tenthsSecond + secondsHour * 10);

    return 0;
}
