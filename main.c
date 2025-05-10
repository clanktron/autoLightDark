#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <gio/gio.h>
#include "sunrise_sunset.h"

typedef struct Coordinates {
    double latitude;
    double longitude;
} Coordinates;

Coordinates estimate_coordinates_from_timezone(const char *tz) {
    if (strcmp(tz, "America/Los_Angeles") == 0)
        return (Coordinates){34.0522, -118.2437};
    if (strcmp(tz, "America/New_York") == 0)
        return (Coordinates){40.7128, -74.0060};
    if (strcmp(tz, "Europe/London") == 0)
        return (Coordinates){51.5074, -0.1278};
    if (strcmp(tz, "Asia/Tokyo") == 0)
        return (Coordinates){35.6895, 139.6917};
    if (strcmp(tz, "Australia/Sydney") == 0)
        return (Coordinates){-33.8688, 151.2093};
    // Default
    return (Coordinates){0.0, 0.0};
}

void set_os_color_mode(const char *mode) {
    GSettings *settings = g_settings_new("org.gnome.desktop.interface");

    if (strcmp(mode, "Dark") == 0) {
        g_settings_set_string(settings, "color-scheme", "prefer-dark");
        printf("[TRIGGER] Set to prefer-dark\n");
    } else if (strcmp(mode, "Light") == 0) {
        g_settings_set_string(settings, "color-scheme", "prefer-light");
        printf("[TRIGGER] Set to prefer-light\n");
    } else {
        printf("Unknown time of day.\n");
    }

    g_object_unref(settings);
}

int get_timezone(char* tz_buffer, size_t buf_size) {
    const char *link = "/etc/localtime";
    char zone[PATH_MAX];
    ssize_t len;

    len = readlink(link, zone, sizeof(zone) - 1);
    if (len == -1) {
        perror("readlink");
        return 1;
    }
    zone[len] = '\0';

    char *file_separator = "/";
    char *token = strtok(zone, file_separator);
    char *last_token = token;
    char *s_last_token;
    while (token != NULL) {
        s_last_token = last_token;
        last_token = token;
        token = strtok(NULL, file_separator);
    }
    if (s_last_token == NULL || last_token == NULL) {
       printf("timezone path found is too short to be valid");
       return 1;
    }

    int result = snprintf(tz_buffer, buf_size, "%s%s%s", s_last_token, file_separator, last_token);
    if (result <= 0 || (size_t)result >= buf_size) {
       return 1;
    }
    return 0;
}

int main() {
    char tz[256];
    if (get_timezone(tz, sizeof(tz))) {
       printf("failed to get timezone, exiting...");
       exit(EXIT_FAILURE);
    }

    Coordinates coords = estimate_coordinates_from_timezone(tz);
    printf("Estimated coordinates for TZ %s: Lat %.4f, Lon %.4f\n", tz, coords.latitude, coords.longitude);

    while (1) {
        time_t now = time(NULL);
        const char *color_mode = is_daytime(coords.latitude, coords.longitude, now) ? "Light" : "Dark";
        set_os_color_mode(color_mode);
        sleep(15);
    }

    return 0;
}
