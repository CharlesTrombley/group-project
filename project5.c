// Define this BEFORE including stdio.h to silence MSVC "unsafe" warnings
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif

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
// Use escaped backslashes for Windows paths and quote when used on the command line
#define CSV_FILE "C:\\Users\\charl\\OneDrive\\Pictures\\Documents\\newgroup_Project\\allmonth.csv"

double calc_cost(double m) { return m * 1000000.0; }

void print_mag_color(double m) {
    if (m >= 5)      printf(RED   "%.2f" RESET, m);
    else if (m >= 4) printf(YELLOW"%.2f" RESET, m);
    else if (m >= 3) printf(GREEN "%.2f" RESET, m);
    else             printf(CYAN  "%.2f" RESET, m);
}

// Simple CSV parser that respects quotes
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

#ifdef _WIN32
#include <windows.h>

// Enable ANSI escape codes on Windows 10+ console
static void enable_ansi_colors(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#else
static void enable_ansi_colors(void) {
    // Nothing to do on Unix-like systems
}
#endif

// Pause before exiting when running from an interactive console so
// the user can read output when launching the .exe by double-click.
static void wait_for_enter_if_console(void) {
    if (isatty(fileno(stdin))) {
        printf("\nPress Enter to exit...");
        fflush(stdout);
        int c = getchar();
        (void)c;
    }
}

static void sleep_seconds(int s) {
#ifdef _WIN32
    Sleep(s * 1000);
#else
    sleep(s);
#endif
}

static void print_usage(void) {
    printf("Usage: project5.exe [--hold N] [--wait] [--no-pause]\n");
    printf("  --hold N   : keep program open for N seconds before exiting\n");
    printf("  --wait     : wait for Enter before exiting (interactive)\n");
    printf("  --no-pause : do not pause before exit (useful for scripting)\n");
}

int main(int argc, char **argv) {
    enable_ansi_colors();

    int no_pause = 0;
    int wait_enter = 0;
    int hold_seconds = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--no-pause") == 0) {
            no_pause = 1;
        } else if (strcmp(argv[i], "--wait") == 0) {
            wait_enter = 1;
        } else if (strcmp(argv[i], "--hold") == 0) {
            if (i + 1 < argc) {
                hold_seconds = (int)strtol(argv[++i], NULL, 10);
                if (hold_seconds < 0) hold_seconds = 0;
            } else {
                print_usage();
                return 1;
            }
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage();
            return 0;
        }
    }

    printf("Program developed by team 1, Wayne, Charlie, Havi\n");

    // Download / refresh earthquakes.csv
    char cmd[512];
    // Note: requires "curl" available in PATH on Windows
    // Quote the output path to handle spaces in the folder name
    snprintf(cmd, sizeof(cmd), "curl -s \"%s\" -o \"%s\"", CSV_URL, CSV_FILE);
    int rc = system(cmd);
    if (rc != 0) {
        fprintf(stderr, "Warning: failed to download CSV (curl returned %d).\n", rc);
        fprintf(stderr, "Continuing and attempting to read existing file: %s\n", CSV_FILE);
    }

    FILE *f = fopen(CSV_FILE, "r");
    if (!f) {
        perror("fopen");
        if (!no_pause) {
            if (hold_seconds > 0) {
                printf("\nHolding for %d seconds before exit...\n", hold_seconds);
                fflush(stdout);
                sleep_seconds(hold_seconds);
            } else if (wait_enter || isatty(fileno(stdin))) {
                wait_for_enter_if_console();
            }
        }
        return 1;
    }

    char line[LINE];
    if (!fgets(line, LINE, f)) {
        fclose(f);
        printf("Empty CSV.\n");
        return 1;
    }

    char c_time[256], c_mag[32], c_place[512];
    long   count_total   = 0;
    double sumMag_total  = 0.0;
    double sumCost_total = 0.0;

    // Header: keep numeric columns aligned, place at the end
    printf(BLUE "#  %-25s %-12s %-15s %-30s\n" RESET,
           "Time", "Magnitude", "Damage Cost", "Place");
    printf(BLUE "---------------------------------------------------------------------------------------------\n" RESET);

    int idx   = 1;
    int shown = 0;
    const int MAX_SHOW = 100000;   // show first 30 rows

    while (fgets(line, LINE, f)) {
        col(c_time,  line, 0);   // time column
        col(c_mag,   line, 4);   // magnitude column
        col(c_place, line, 13);  // place column

        double m = atof(c_mag);
        if (m <= 0) continue;

        double cost = calc_cost(m);

        // Global statistics for all rows with mag > 0
        count_total++;
        sumMag_total  += m;
        sumCost_total += cost;

        // Print only the first MAX_SHOW rows
        if (shown < MAX_SHOW) {
            printf(GREEN "%-2d " RESET, idx++);
            printf("%-25s ", c_time);

            // Magnitude (colored)
            print_mag_color(m);
            printf("   ");

            // Cost
            printf(MAG "$%.0f" RESET "   ", cost);

            // City / place at the end, so numeric columns stay aligned
            printf("%s\n", c_place);

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

    if (!no_pause) {
        if (hold_seconds > 0) {
            printf("\nHolding for %d seconds before exit...\n", hold_seconds);
            fflush(stdout);
            sleep_seconds(hold_seconds);
        } else if (wait_enter || isatty(fileno(stdin))) {
            wait_for_enter_if_console();
        }
    }
    return 0;
}
