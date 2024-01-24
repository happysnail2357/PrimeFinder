//
// primefinder.cpp
//
//   by Paul Puhnaty - 1/24/2024
//
//   A program to find all the prime numbers from 1 to an upper bound by utililizing multiple
//   concurrent threads.
//


/*/        Includes        /*/

// IO
#include <iostream>
#include <fstream>

// Benchmarking
#include <chrono>

// sqrt() and pow()
#include <cmath>

// Multi-threading
#include <thread>

// Custom data structures
#include "PFStructures.hpp"



/*/  Forward Declarations  /*/

int64_t parseArgument(const char* arg);
void displayUsage(const char* errorMessage);
void findPrimes(Counter* counter, const int64_t upperLimit, AtomicInt* numPrimes, AtomicInt* sumPrimes, MaxList* maxPrimes);
bool isPrime(int64_t n);
void saveResults(double time, int64_t numPrimes, int64_t sumPrimes, MaxList::Iterator maxPrimes);


/*/  Constant Definitions  /*/

const int64_t DEFAULT_SEARCH_UPPER_BOUND = 1e8;
const int64_t MAX_SEARCH_UPPER_BOUND = 1e10;
const int32_t NUM_MAX_PRIMES_SAVED = 10;
const int32_t NUM_THREADS = 8;

const char* OUTPUT_FILE_NAME = "primes.txt";



/*/      Entry Point       /*/

int main(int argc, char* argv[])
{
	if (argc > 2)
	{
		displayUsage("ERROR: too many arguments");
		
		return -1;
	}
	
	// Determine upper bound for prime search
	//
	int64_t upperBound = (argc == 1) ? DEFAULT_SEARCH_UPPER_BOUND : parseArgument(argv[1]);
	
	std::cout << "\nUpper Bound: " << upperBound << "\n\nSearching...\n\n";
	
	
	// Variables to store execution time
	//
	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::duration<double> duration;
	
	
	// The following variables will be initialized assuming that 1 and 2 have already been processed
	
	Counter counter(3);
	
	MaxList maxPrimes(NUM_MAX_PRIMES_SAVED * NUM_THREADS);
	maxPrimes.add(2);
	
	AtomicInt numPrimes(1);
	AtomicInt sumOfPrimes(2);
	
	
	// Start the timer
	//
	start = std::chrono::system_clock::now();
	
	
	std::thread thrds[NUM_THREADS];
	
	for (int i = 0; i < NUM_THREADS; i++)
	{
		thrds[i] = std::thread(findPrimes, &counter, upperBound, &numPrimes, &sumOfPrimes, &maxPrimes);
		
		std::cout << "Thread Spawned\n";
	}
	
	for (int i = 0; i < NUM_THREADS; i++)
	{
		thrds[i].join();
	}
	
	
	// Stop the timer
	//
	end = std::chrono::system_clock::now();
	
	duration = end - start;
	
	
	std::cout << "\nExecution complete (" << std::to_string(duration.count()) << "s)\n\n";
	
	
	MaxList::Iterator itr = maxPrimes.getIterator();
	
	// Skip until there are NUM_MAX_PRIMES_SAVED left
	//
	itr.forward(- NUM_MAX_PRIMES_SAVED);
	
	
	saveResults(duration.count(), numPrimes, sumOfPrimes, itr);
	
	
	return 0;
}


/*/  Function Definitions  /*/


// Parses a number from the argument
//
int64_t parseArgument(const char* arg)
{
	int64_t val = 0;
	int32_t exp = 0;
	
	int32_t decimalPlaces = 0;
	
	bool e = false;
	
	for (int32_t i = 0; arg[i] != '\0'; i++)
	{
		const char ch = arg[i];
		
		if (ch >= '0' && ch <= '9')
		{
			if (e)
			{
				exp *= 10;
				exp += (ch - '0');
			}
			else
			{
				if (val > 0 || ch != '0')
				{
					decimalPlaces++;
				}
				
				val *= 10;
				val += (ch - '0');
			}
			
			continue;
		}
		
		if (!e && (ch == 'e' || ch == 'E'))
		{
			e = true;
		}
		else
		{
			displayUsage("ERROR: argument is not a valid decimal integer");
			
			exit(-1);
		}
	}
	
	decimalPlaces += exp;
	
	if (decimalPlaces <= 11)
	{
		val *= pow(10, exp);
		
		if (val <= MAX_SEARCH_UPPER_BOUND)
		{
			return val;
		}	
	}
		
	displayUsage("ERROR: Upper bound parameter exceeds 1e10 limit");
	
	exit(-2);
}


// Prints the usage information for this program
//
void displayUsage(const char* errorMessage)
{
	std::cout << "\n  " << errorMessage << "\n\n\tUsage: ./primefinder [optional]<upper bound (1e10 max)>\n\n";
}


// Checks numbers from a counter for primality
//
void findPrimes(Counter* counter, const int64_t upperLimit, AtomicInt* numPrimes, AtomicInt* sumPrimes, MaxList* maxPrimes)
{
	int64_t count = counter->nextnext();
	
	// To reduce the need for synchronization, each
	// thread will track its own prime statistics
	// then update the shared memory before exiting.
	
	int64_t num = 0, sum = 0;
	
	MaxList max(NUM_MAX_PRIMES_SAVED);
	
	
	while (count < upperLimit)
	{
		if (isPrime(count))
		{
			num++;
			sum += count;
			max.add(count);
		}
		
		count = counter->nextnext();
	}
	
	// Update the shared variables
	
	*numPrimes += num;
	*sumPrimes += sum;
	
	maxPrimes->add(&max);
}


// Checks if a number is prime.
//
//   For optimization purposes, it is assumed that
//   all numbers passed to this function are odd
//   and greater than 2
//
bool isPrime(int64_t n)
{
	// Base case
	//
	if (n == 3)
	{
		return true;
	}

	// Check if the number is divisible by 3
	//
	if (n % 3 == 0)
	{
		return false;
	}
	
	//
	// For the numbers that do not fall into the
	// above categories, we will have to check
	// them for all possible factors.
	// 
	// Two optimization are used:
	//
	// 	1. Only numbers less than or equal to the square root
	//		of n are tried as possible factors.
	//
	//	2. Only prime numbers (and multiples of five) are
	//		tried as possible factors. This uses the fact
	//		that prime numbers (>3) can be expressed in
	//		the form: 6*k +[1 or 5] , where k is non-negative.
	//
	
	int32_t square_root_n = sqrt(n);

	int32_t div = 5;

	while (true)
	{
		if (div > square_root_n)
		{
			return true;
		}


		if (n % div == 0)
		{
			return false;
		}

		div += 2;


		if (n % div == 0)
		{
			return false;
		}

		div += 4;
	}
}


// Saves the results to a file
//
void saveResults(double time, int64_t numPrimes, int64_t sumPrimes, MaxList::Iterator maxPrimes)
{
	std::ofstream file{OUTPUT_FILE_NAME};
	
	if (file.is_open())
	{
		file << std::to_string(time) << "s " << numPrimes << " " << sumPrimes << "\n";
		
		while (maxPrimes.hasNext())
		{
			file << maxPrimes.next() << " ";
		}
		
		file << "\n";
		
		file.close();
		
		std::cout << "Results saved to file \"" << OUTPUT_FILE_NAME << "\"\n\n";
	}
	else
	{
		std::cout << "ERROR: Failed to create file \"" << OUTPUT_FILE_NAME << "\"\n\n";
	}
}

