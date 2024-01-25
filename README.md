# Assignment 1: Prime Finder

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

## Correctness

Since the implementation of this task is carried out with multiple concurrent threads, there is a need to verify the algorithm's correctness. 
To distribute workload fairly, a counter object is shared between all threads; this counter gives each thread a unique number by utilizing a mutex 
to protect its critical section from corruption. Therefore, all values between 1 and the upperbound are checked and checked only once. Once a thread 
is finished it updates shared values with its individual results. These results values are also protected with mutexes to enforce atomic access to 
their values.

## Efficiency

To increase efficiency, several optimizations were made to the code.

- The counter increments by 2, and thus skips all even numbers (2 is accounted for)
- The primality algorithm only checks for factors that are less than or equal to the square root of the number being checked
- The primality algorithm only checks for prime factors of the form 6*n+1 or 6*n+5

## Experimentation

To check the validity of the output, the upperbound was set to smaller powers of 10 (10, 100, 1000, ...) to check the results against known values. 
The output did match expected results for these smaller bounds, providing confidence in the output for larger bounds.

