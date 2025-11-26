#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LINE 4096

#define BLUE   "\033[1;34m"
#define CYAN   "\033[1;36m"
#define GREEN  "\033[1;32m"
#define YELLOW "\033[1;33m"
#define RED    "\033[1;31m"
#define MAG    "\033[1;35m"
#define RESET  "\033[0m"

// All earthquakes worldwide for the past ~30 days (USGS)
#define CSV_URL  "https://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/all_month.csv"
#define CSV_FILE "all_month.csv"

double calc_cost(double m) { return m * 1000000.0; }

void print_mag_color(double m) {
    if (m >= 5)      printf(RED   "%.2f" RESET, m);
    else if (m >= 4) printf(YELLOW"%.2f" RESET, m);
    else if (m >= 3) printf(GREEN "%.2f" RESET, m);
    else             printf(CYAN  "%.2f" RESET, m);
}

// parser CSV sencillo que respeta comillas
char *col(char *dest, const char *src, int colWanted) {
    int col = 0, i = 0, j = 0, inside = 0;

    while (src[i]) {
        char c = src[i];
        if (c == '"') {
            inside = !inside;
            i++;
            continue;
        }
        if (c == ',' && !inside) {
            if (col == colWanted) {
                dest[j] = 0;
                return dest;
            }
            col++;
            j = 0;
            i++;
            continue;
        }
        if (col == colWanted) dest[j++] = c;
        i++;
    }
    dest[j] = 0;
    return dest;
}

// devuelve solo la parte después del último " of "
char *short_place(char *place) {
    char *last = NULL;
    char *p = place;

    while ((p = strstr(p, " of ")) != NULL) {
        last = p;
        p += 4; // saltar " of "
    }

    if (last)
        return last + 4; // después del último " of "
    return place;
}

void tolower_str(char *s) {
    for (; *s; s++) *s = (char)tolower((unsigned char)*s);
}

int main(void) {
    printf("Program developed by team3\n");

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "curl -s \"%s\" -o %s", CSV_URL, CSV_FILE);
    system(cmd);  // descarga / actualiza earthquakes.csv

    FILE *f = fopen(CSV_FILE, "r");
    if (!f) {
        perror("fopen");
        return 1;
    }

    char line[LINE];
    if (!fgets(line, LINE, f)) {
        fclose(f);
        printf("Empty CSV.\n");
        return 1;
    }

    char c_time[256], c_mag[32], c_place[512];
    long   count_total = 0;
    double sumMag_total  = 0.0;
    double sumCost_total = 0.0;

    printf(BLUE "#  %-25s %-30s %-12s %-15s\n" RESET,
           "Time", "Place", "Magnitude", "Damage Cost");
    printf(BLUE "---------------------------------------------------------------------------------------------\n" RESET);

    int idx = 1;
    int shown = 0;
    const int MAX_SHOW = 20;

    while (fgets(line, LINE, f)) {
        col(c_time,  line, 0);   // time
        col(c_mag,   line, 4);   // magnitude
        col(c_place, line, 13);  // place

        double m = atof(c_mag);
        if (m <= 0) continue;

        double cost = calc_cost(m);

        // estadísticas sobre TODOS (mag > 0)
        count_total++;
        sumMag_total  += m;
        sumCost_total += cost;

        // solo imprimir las primeras 20 filas
        if (shown < MAX_SHOW) {
            printf(GREEN "%-2d " RESET, idx++);
            printf("%-25s %-30s ", c_time, short_place(c_place));
            print_mag_color(m);
            printf("   " MAG "$%.0f" RESET "\n", cost);
            shown++;
        }
    }

    fclose(f);

    printf(BLUE "---------------------------------------------------------------------------------------------\n" RESET);
    if (count_total > 0) {
        printf("Count (mag > 0): %ld\n", count_total);
        printf("Sum Magnitude   : %.2f\n", sumMag_total);
        printf("Avg Magnitude   : %.2f\n", sumMag_total / count_total);
        printf("Avg Damage Cost : $%.0f\n", sumCost_total / count_total);
    } else {
        printf("No earthquakes with magnitude > 0.\n");
    }

    // búsqueda por ciudad
    while (1) {
        char query[128];

        printf("\nsearch for a city: ");
        if (!fgets(query, sizeof(query), stdin))
            break;

        // quitar \n
        size_t len = strlen(query);
        if (len > 0 && query[len - 1] == '\n')
            query[len - 1] = '\0';

        if (strcmp(query, "quit") == 0)
            break;
        if (query[0] == '\0')
            continue;

        FILE *fs = fopen(CSV_FILE, "r");
        if (!fs) {
            perror("fopen");
            break;
        }

        fgets(line, LINE, fs); // header

        int found = 0;
        while (fgets(line, LINE, fs)) {
            col(c_time,  line, 0);
            col(c_mag,   line, 4);
            col(c_place, line, 13);

            double m = atof(c_mag);
            if (m <= 0) continue;

            char place_short[512];
            strncpy(place_short, short_place(c_place), sizeof(place_short) - 1);
            place_short[sizeof(place_short) - 1] = '\0';

            char place_lower[512];
            char query_lower[128];

            strncpy(place_lower, place_short, sizeof(place_lower) - 1);
            place_lower[sizeof(place_lower) - 1] = '\0';

            strncpy(query_lower, query, sizeof(query_lower) - 1);
            query_lower[sizeof(query_lower) - 1] = '\0';

            tolower_str(place_lower);
            tolower_str(query_lower);

            if (strstr(place_lower, query_lower)) {
                double cost = calc_cost(m);
                if (!found) {
                    printf(BLUE "\nResults:\n" RESET);
                }
                found = 1;

                printf("%-25s %-30s ", c_time, place_short);
                print_mag_color(m);
                printf("   " MAG "$%.0f" RESET "\n", cost);
            }
        }

        fclose(fs);

        if (!found) {
            printf("No results for \"%s\".\n", query);
        }
    }

    return 0;
}
