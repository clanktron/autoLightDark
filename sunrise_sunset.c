#include <math.h>
#include <time.h>
#include <stdbool.h>
#include "sunrise_sunset.h"

#define DEG_TO_RAD (M_PI / 180.0)
#define RAD_TO_DEG (180.0 / M_PI)
#define J2000 2451545.0

double julian_day(int year, int month, int day) {
    if (month <= 2) {
        year--;
        month += 12;
    }
    int A = year / 100;
    int B = 2 - A + A / 4;
    return floor(365.25 * (year + 4716)) +
           floor(30.6001 * (month + 1)) +
           day + B - 1524.5;
}

double days_since_j2000(int year, int month, int day) {
    return julian_day(year, month, day) - J2000;
}

double mean_solar_noon(double longitude, int year, int month, int day) {
    double d = days_since_j2000(year, month, day);
    return d - (longitude / 360.0);
}

double solar_mean_anomaly(double d) {
    return fmod((357.5291 + 0.98560028 * d), 360.0);
}

double equation_of_center(double M) {
    double Mrad = M * DEG_TO_RAD;
    return (1.9148 * sin(Mrad)) +
           (0.0200 * sin(2 * Mrad)) +
           (0.0003 * sin(3 * Mrad));
}

double ecliptic_longitude(double M, double C) {
    return fmod((M + C + 180 + 102.9372), 360.0);
}

double solar_transit(double d, double M, double L) {
    double Mrad = M * DEG_TO_RAD;
    double Lrad = L * DEG_TO_RAD;
    return J2000 + d + 0.0053 * sin(Mrad) - 0.0069 * sin(2 * Lrad);
}

double declination(double L) {
    double Lrad = L * DEG_TO_RAD;
    return asin(sin(Lrad) * sin(23.44 * DEG_TO_RAD)) * RAD_TO_DEG;
}

double hour_angle(double latitude, double declination) {
    // Atmospheric correction: solar elevation angle when the sun is just at the visible horizon
    const double solarElevation = -0.833; // degrees
    double latRad = latitude * DEG_TO_RAD;
    double decRad = declination * DEG_TO_RAD;
    double elevationRad = solarElevation * DEG_TO_RAD;
    return acos((sin(elevationRad) - sin(latRad) * sin(decRad)) /
                (cos(latRad) * cos(decRad))) * RAD_TO_DEG;
}

time_t julian_to_unix(double jd) {
    return (time_t)((jd - 2440587.5) * 86400.0);
}

bool is_daytime(double latitude, double longitude, time_t currentTime) {
    struct tm *utc = gmtime(&currentTime);
    int year = utc->tm_year + 1900;
    int month = utc->tm_mon + 1;
    int day = utc->tm_mday;

    SunriseSunset sun = sunrise_sunset(latitude, longitude, year, month, day);

    return currentTime >= sun.sunrise && currentTime < sun.sunset;
}

SunriseSunset sunrise_sunset(double latitude, double longitude, int year, int month, int day) {
    double d = days_since_j2000(year, month, day);
    double M = solar_mean_anomaly(d);
    double C = equation_of_center(M);
    double L = ecliptic_longitude(M, C);
    double dec = declination(L);
    double transitJ = solar_transit(mean_solar_noon(longitude, year, month, day), M, L);
    double H = hour_angle(latitude, dec);

    double sunriseJ = transitJ - H / 360.0;
    double sunsetJ = transitJ + H / 360.0;

    return (SunriseSunset){
        .sunrise = julian_to_unix(sunriseJ),
        .sunset = julian_to_unix(sunsetJ)
    };
}
