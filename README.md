# ICS 265 : C Programming  

<img src="logo.png" alt="Logo" width="50%">


## Project Team

- Wayne
- Charlie
- Havi

**Course**: ICS 265 - C Programming  
**Instructor**: Sam Espana, CF  
**Institution**: Department of Computer Science and Cybersecurity, College of Science  
**Date**: December 1, 2025



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

## Links

- [GitHub Repository](https://github.com/4rji/C-Programming/tree/main/project5)
- [USGS Earthquake Data](https://earthquake.usgs.gov/earthquakes/feed/v1.0/csv.php)

