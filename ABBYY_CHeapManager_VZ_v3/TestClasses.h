#include "CHeapManager.h"
#include <time.h>
#include <cmath>

#pragma once
using namespace std;

#define KB 1024

CHeapManager heapManager;
HANDLE defaultHeapHandle;

class C_1KB {
public:
	void* operator new(size_t) {
		return heapManager.Alloc(KB);
	}
	void operator delete(void* mem) {
		heapManager.Free(mem);
	}
};

class C_256KB_standart {
	char c[256 * KB];
};

class C_256KB {
public:
	void* operator new(size_t) {
		return heapManager.Alloc(256 * KB);
	}
	void operator delete(void* mem) {
		heapManager.Free(mem);
	}
};

class CRandomSize {
public:
	void* operator new(size_t) {
		int rand_size = rand() % 128 * KB;
		if (rand_size >= 4 * KB) {
			return heapManager.Alloc(rand_size);
		} else {
			return heapManager.Alloc(4 * KB);
		}
	}
	void operator delete(void* mem) {
		heapManager.Free(mem);
	}
};

class CRandomSize2 {
public:
	void* operator new(size_t);
	void operator delete(void*);
};

void CRandomSize2::operator delete(void* mem)
{
	heapManager.Free(mem);
}

void* CRandomSize2::operator new(size_t size) {
	int rand_size = pow(2.0, log(rand() % 1001)) * (rand() % 500);
	//	cout << "RAND SIZE: " << rand_size << endl;
	if (rand_size >= size) {
		return heapManager.Alloc(rand_size);
	} else {
		return heapManager.Alloc(size);
	}
}

class CMyHeapRandom {
public:
	void* operator new(unsigned int size) {
		int memberSize = rand() % (1024 * KB);
		return heapManager.Alloc(size * memberSize);
	}

	void operator delete(void* pointer) {
		return heapManager.Free(pointer);
	}
};

class CDefaultHeapRandom {
public:
	void* operator new(unsigned int size) {
		int memberSize = rand() % (1024 * KB);
		return HeapAlloc(defaultHeapHandle, HEAP_NO_SERIALIZE, size * memberSize);
	}

	void operator delete(void* pointer) {
		HeapFree(defaultHeapHandle, 0, pointer);
	}
};