//
// PFStructures.cpp
//
//   by Paul Puhnaty - 1/24/2024
//
//   A module defining helpful data structures for primefinder.cpp
//


/*/        Includes        /*/

// Sorting
#include <algorithm>

#include "PFStructures.hpp"



/*/  Function Definitions  /*/


/*/         MaxList        /*/

MaxList::MaxList(int32_t size)
{
	if (size <= 0)
		throw std::invalid_argument("MaxList size must be greater than zero");
	
	this->capacity = size;
	this->index = 0;
	this->size = 0;
	
	this->values = new int64_t[this->capacity];
}

MaxList::~MaxList()
{
	delete[] this->values;
}

void MaxList::add(int64_t newMax)
{
	this->values[this->index] = newMax;
	
	this->index = (this->index + 1) % this->capacity;
	
	if (this->size < this->capacity)
	{
		this->size += 1;
	}
}

void MaxList::add(MaxList* ml)
{
	this->key.lock();
	
	Iterator itr = ml->getIterator();
	
	while (itr.hasNext())
	{
		this->add(itr.next());
	}
	
	this->key.unlock();
}

MaxList::Iterator MaxList::getIterator()
{
	std::sort(this->values, this->values + this->size);
	
	this->index = 0;
	
	return Iterator(this->values, 0, this->size);
}



/*/   MaxList::Iterator    /*/

MaxList::Iterator::Iterator(int64_t* data, int32_t start, int32_t stop)
{
	this->at = data;
	this->where = start;
	this->stop = stop;
}

bool MaxList::Iterator::hasNext()
{
	if (this->where < this->stop)
		return true;
	
	return false;
}

int64_t MaxList::Iterator::next()
{
	int64_t ret = *(this->at);
	
	this->at += 1;
	this->where += 1;
	
	return ret;
}

void MaxList::Iterator::forward(int32_t skip)
{
	if (skip < 0)
	{
		// Negative values indicate the distance
		// from the end to skip to (e.g. -3 means
		// skip all but the last 3 values).
		
		skip = (this->stop - this->where) + skip;
		
		if (skip < 0)
		{
			skip = 0;
		}
	}
	
	this->where += skip;
	this->at += skip;
}



/*/        Counter         /*/

Counter::Counter(int64_t start)
{
	this->count = start;
}

int64_t Counter::next()
{
	this->key.lock();
	
	int64_t ret = this->count;
	
	this->count += 1;
	
	this->key.unlock();
	
	return ret;
}

int64_t Counter::nextnext()
{
	this->key.lock();
	
	int64_t ret = this->count;
	
	this->count += 2;
	
	this->key.unlock();
	
	return ret;
}



/*/        AtomicInt       /*/

AtomicInt::AtomicInt(int64_t i)
{
	this->val = i;
}

void AtomicInt::operator+=(int64_t i)
{
	this->key.lock();
	
	this->val += i;
	
	this->key.unlock();
}

AtomicInt::operator int64_t()
{
	this->key.lock();
	
	int64_t ret = this->val;
	
	this->key.unlock();
	
	return ret;
}

