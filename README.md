# IEDA Final Project

## Overview

This project is a C-based application that reads input from a file and performs various operations. The main components include reading inputs, handling FF and gate blocks, managing instances and nets, and performing placements and timing calculations.

## Requirements

- GCC (or another C compiler)
- CMake (version 3.10 or higher)

## Building the Project

1. **Clone the repository**:

```sh
git clone <repository_url>
cd <repository_directory>
```

2. **Create a build directory**:

```
mkdir build
cd build
```

3. **Generate the build system with CMake**:

```
cmake ..
```

4. **Compile the project**:

```
make
```

## Running the Project

To run the executable, use the following command:

```
./IEDA_Final_Project <input_filename>
```