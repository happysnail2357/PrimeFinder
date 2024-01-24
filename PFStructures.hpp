//
// PFStructures.hpp
//
//   by Paul Puhnaty - 1/24/2024
//
//   A module defining helpful data structures for primefinder.cpp
//


/*/        Includes        /*/

// Mutual Exclusion
#include <mutex>


// A list to hold the largest values passed to it
//
class MaxList
{
private:

	int64_t* values;
	
	int32_t index, capacity, size;
	
	std::mutex key;
	
public:

	class Iterator
	{
	private:
	
		int32_t where, stop;
		
		int64_t* at;
		
	public:
	
		Iterator(int64_t* data, int32_t start, int32_t stop);
		
		// Returns true if another value is available
		bool hasNext();
		
		// Returns the next value available
		//
		int64_t next();
		
		// Skips a certain number of elements
		//
		void forward(int32_t skip);
	};
	

	MaxList(int32_t size);
	~MaxList();
	
	// Overwrites the smallest number in the list
	//
	void add(int64_t newMax);
	
	// Adds the contents of another MaxList to this one
	//
	void add(MaxList* ml);
	
	Iterator getIterator();
};


// An atomic counter class
//
class Counter
{
private:

	int64_t count;
	
	std::mutex key;
	
public:

	Counter(int64_t start);

	// Return the current value and increment
	//
	int64_t next();
	
	// Return the current value and increment twice
	//
	int64_t nextnext();
};


// A wrapper for an integer to enforce atomic access
//
class AtomicInt
{
private:

	int64_t val;
	
	std::mutex key;
	
public:

	AtomicInt(int64_t i);
	
	void operator+=(int64_t i);
	
	operator int64_t();
};

