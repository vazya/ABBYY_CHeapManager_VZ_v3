#pragma once
#include "CHeapManager.h"
#include <time.h>
#include <cmath>
using namespace std;

#define KB 1024

CHeapManager heapManager;
HANDLE standartHeapManager;

class CMyHeapRandom {
public:
	void* operator new(unsigned int size) {
		int randSize = rand() % (1024 * KB);
		return heapManager.Alloc(size * randSize);
	}

	void operator delete(void* ptr) {
		return heapManager.Free(ptr);
	}
};

class CStandartHeapRandom {
public:
	void* operator new(unsigned int size) {
		int randSize = rand() % (1024 * KB);
		return HeapAlloc(standartHeapManager, HEAP_NO_SERIALIZE, size * randSize);
	}

	void operator delete(void* ptr) {
		HeapFree(standartHeapManager, 0, ptr);
	}
};