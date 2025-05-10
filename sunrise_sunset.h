#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <time.h>
#include <stdbool.h>

typedef struct SunriseSunset {
    time_t sunrise;
    time_t sunset;
} SunriseSunset;

SunriseSunset sunrise_sunset(double latitude, double longitude, int year, int month, int day);
bool is_daytime(double latitude, double longitude, time_t currentTime);

#endif
