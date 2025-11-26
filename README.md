# Project 5 - Earthquake Analysis App

## Overview

This project focuses on file handling using CSV data by building a simple Earthquake Analysis App in C. The application reads the official USGS dataset containing all worldwide earthquakes from the past 30 days and displays key earthquake information with statistical analysis.

## Features

- **Real-time Data**: Downloads the latest earthquake data from USGS
- **CSV Parsing**: Custom CSV parser that handles quoted fields correctly
- **Data Extraction**: Extracts time, magnitude, damage cost, and location for each earthquake
- **Color-Coded Output**: Magnitude values displayed with color coding based on severity
- **Statistical Analysis**: Calculates and displays:
  - Total number of valid earthquakes
  - Sum of all magnitudes
  - Average magnitude
  - Average estimated damage cost

## How It Works

### Data Source

The program downloads earthquake data from:
```
https://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/all_month.csv
```

### Damage Cost Calculation

The damage cost is estimated by multiplying the earthquake's magnitude by $1,000,000:
```c
double calc_cost(double m) { 
    return m * 1000000.0; 
}
```

### CSV Parsing

The application includes a custom CSV parser that respects quoted fields:

```c
char *col(char *dest, const char *src, int colWanted) {
    // Extracts specific column from CSV line
    // Handles quoted fields containing commas
}
```

### Magnitude Color Coding

- **Red**: Magnitude ≥ 5.0 (Major)
- **Yellow**: Magnitude ≥ 4.0 (Light to Moderate)
- **Green**: Magnitude ≥ 3.0 (Minor)
- **Cyan**: Magnitude < 3.0 (Micro)

## Compilation

### Windows
```bash
gcc -o earthquake project5.c
```

### Linux/Mac
```bash
gcc -o earthquake project5.c
```

## Requirements

- C compiler (GCC recommended)
- `curl` command-line tool (for downloading CSV data)
- Internet connection (to download latest earthquake data)

## Usage

1. Run the program:
   ```bash
   ./earthquake
   ```

2. The program will:
   - Download the latest earthquake data
   - Parse the CSV file
   - Display the first 30 earthquakes with details
   - Show summary statistics

## Sample Output

```
Program developed by team 1, Wayne, Charlie, Havi
#  Time                      Magnitude    Damage Cost     Place
---------------------------------------------------------------------------------------------
1  2025-11-24T10:30:15.123Z  4.50         $4500000        10 km N of Los Angeles, CA
2  2025-11-24T09:15:42.456Z  3.20         $3200000        5 km E of San Francisco, CA
...
---------------------------------------------------------------------------------------------
Count (mag > 0): 1234
Sum Magnitude   : 4567.89
Avg Magnitude   : 3.70
Avg Damage Cost : $3700000
```

## Project Team

- Wayne
- Charlie
- Havi

**Course**: ICS 265 - C Programming  
**Instructor**: Sam Espana, CF  
**Institution**: Department of Computer Science and Cybersecurity, College of Science  
**Date**: December 1, 2025

## Deliverables

Each team member produces:
1. C program source code
2. Short report documenting the implementation
3. Demonstration video showing:
   - Downloading the USGS CSV file
   - Reviewing the C source code
   - Running the application
   - Displaying processed earthquake data and statistics

## Repository Structure

```
project5/
├── project5.c           # Main C source code
├── proyect4.tex         # LaTeX documentation
├── project.png          # Screenshot of execution
├── logo.png            # University logo
└── README.md           # This file
```

## Links

- [GitHub Repository](https://github.com/4rji/C-Programming/tree/main/project5)
- [USGS Earthquake Data](https://earthquake.usgs.gov/earthquakes/feed/v1.0/csv.php)

## License

Educational project for ICS 265 course.
