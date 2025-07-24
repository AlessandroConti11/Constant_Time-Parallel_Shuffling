# Constant_Time-Parallel_Shuffling


Author: Alessandro Conti - [AlessandroConti11](https://github.com/AlessandroConti11)

License: [MIT license](LICENSE).


Tags: `#c`, `#complexity_study`, `#computer_engineering`, `#computer_science_and_engineering`, `#constant-weight_word`, `#cww`, `#cryptography`, `#cryptography_and_architecture_for_computer_security`, `#daniel_j_bernstein`, `#djb`, `#insertion_in_array`, `#insertion_series`, `#polimi`, `#porting`, `#porting_python_to_c`, `#python_to_c`.


## University

Politecnico di Milano.

Academic Year: 2024/2025.

095947 - CRYPTOGRAPHY AND ARCHITECTURES FOR COMPUTER SECURITY - professor Pelosi Gerardo


## Specification

Shuffling arrays in constant time is a common problem in modern cryptography. This project involves analyzing the technique proposed in https://cr.yp.to/2024/insertionseries-20240515.py, and implementing it in C, possibly employing parallelization.

This repository contains the porting to C of:
- the insertionSeries algorithm;
- the constant-weight word creation algorithm

implemented by Daniel J. Bernstein in Python.
Both the sequential and parallel versions of the algorithm have been implemented to demonstrate scalability and adaptability in multicore environments.

In addition to simple transcription, the project also analyses the computational complexities of the algorithms, providing a basis for optimisation and benchmarking.


## How to run

The steps below refer to a Unix environment, for other environments the commands may change.
0. install gcc
    ```bash
    sudo apt-get install gcc 
    ```
1. compile each source file into an object file
    ```bash
    gcc -std=c11 -Wall -Werror -Wextra -O2 -fopenmp -c FILE.c -o FILE.o
    ```
2. link all object files into an executable
    ```bash
    g++ -std=c11 -Wall -Werror -Wextra -O2 -fopenmp\
    main.o \
    utility/intList.o \
    utility/pairList.o \
    utility/safeRealloc.o \
    insertion_series/insertionSeries.o \
    constant-weight_words/constantWeightWord.o
    -o EXECUTABLE
    ```
3. run the executable
    ```bash
    ./EXECUTABLE
    ```

<br>

The Makefile in the repository can also be used to compile the code.
- this option allows you to compile with the following tags: *-std=c11 -Wall -Werror -Wextra -O2 -fopenmp*
    ```bash
    make
    ```
- if you want to specify different tags, you can set them
   ```bash
   make compile CXXFLAGS=YOUR_FLAGS
   ```
- if you want to remove all .o files and the final executable
    ```bash
    make clean
    ```

<br>

The CMakeLists.txt in the repository can also be used to compile the code.
0. install cmake
    ```bash
    sudo apt-get install cmake
    ```
1. create the build folder
    ```bash
    mkdir build && cd build
    ```
2. generate compilation files
    ```bash
    cmake ..
    ```
3. build the project
    ```bash
    cmake --build .
    ```
4. run the executable
    ```bash
    ./djb
    ```

<br>

If you want to test the functioning of the algorithms, there is a main program suitable for testing the two algorithms.
- To run the **insertionSeries** algorithm
    ```bash
    ./EXECUTABLE --insertionseries
    ```
- To run the **cww** algorithm
    ```bash
    ./EXECUTABLE --cww
    ```
It is also possible to decide whether to run the two algorithms in serial or in parallel mode by adding the *--serial* or *--parallel* options.

## Contribute

- If you find a security vulnerability, do NOT open an issue. Email [Alessandro Conti](mailto:ale.conti.1101@gmail.com) instead.
- If you find a bug or error or want to add some other function that is not implemented yet
    1. **FORK** the repo
    2. **CLONE** the project to your own machine
    3. **COMMIT** to your own branch
    4. **PUSH** your work back up to your fork
    5. submit a **PULL REQUEST** so that I can review your changes
  > NOTE: Be sure to merge the latest from "upstream" before making a pull request!


### Code Style

Each new function must be documented using the following style:
- *Short function description*: A brief description explaining what the function does.
- *@warning*: A section listing all the assumptions made by the function, such as the preconditions that the parameters must fulfil.
- *Blank line*: Add a blank line to separate the documentation sections.
- *@details*: A detailed section describing how the function works, explaining the algorithms and logic used.
- *Blank line*: Add a blank line to separate the documentation sections.
- *@param*: A list of the parameters required by the function, each accompanied by a brief description of its role and type.
- *@return*: A description of what the function returns, including the data type.

In general, any additional comments that might improve understanding of the code are welcome. 😃
