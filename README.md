#Assignment 1: Prime Finder

A program to find all the prime numbers from 1 to 1e8 by utililizing multiple concurrent threads.

## Build Instructions

Make sure that a c++ compiler (g++) is installed on you device, and then execute the following (Linux) commands in a terminal:

To compile...

$ `g++ primefinder.cpp PFStructures.cpp -o primefinder`

To run...

$ `./primefinder`

Optionally, an integer representing the upperbound of the search can be passed as a parameter...

$ `./primefinder 1e8`


## Output

After each run, the program will create/overwrite a file _"primes.txt"_
The format of the file is as follows:

```
<execution time> <total number of primes found> <sum of all primes found>
<top ten maximum primes, listed in order from lowest to highest>
```
