#include <stdio.h>
#include <sys/time.h>
#include <time.h>

int daysMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void convertMinutesToCalendar(int minutesOfYear, int* months, int* days, int* hours, int* minutes) {

    int totalDays = minutesOfYear / (60 * 24);
    *hours = (minutesOfYear % (60 * 24)) / 60;
    *minutes = minutesOfYear % 60;

    int dayOfYear = 0;
    *months = 1;

    for (int i = 0; i < 12; i++) {
        if (dayOfYear + daysMonth[i] >= totalDays) {
            *days = totalDays - dayOfYear;
            break;
        }
        dayOfYear += daysMonth[i];
        (*months)++;
    }
}


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


    int months, days, hours, minutes;
    convertMinutesToCalendar(minutesYear, &months, &days, &hours, &minutes);
    printf("%d months\n", months);
    printf("%d days\n", days);
    printf("%d hours\n", hours);
    printf("%d minutes\n", minutes);
    return 0;
}
