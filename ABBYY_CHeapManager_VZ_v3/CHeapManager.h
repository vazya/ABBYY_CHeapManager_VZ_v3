#pragma once
#include <windows.h>
#include <utility>
#include <set>
#include <vector>
using namespace std;

typedef pair<LPVOID, int> AddresSize;

struct pairCompare {
	bool operator()(const AddresSize &a, const AddresSize &b) const {
		return a.first < b.first;
	}
};

class CHeapManager {
public:
	CHeapManager();
	~CHeapManager();

	// резервирует непрерывный регоион размера maxSize
	// передает в физическую память небольшой регион minSize
	void Create(int minSize, int maxSize);
	void* Alloc(int size);
	void Free(void* mem);
	void Destroy();
private:
	int maxSize;
	int minSize;
	LPVOID startAddress;

	set <LPVOID> freeSmallBlocks;				// .. < 4KB
	set <AddresSize> freeMediumBlocks;			// 4KB < .. < 128KB
	set <AddresSize> freeBigBlocks;				// 128KB < ..

	set <LPVOID> allocatedBlocks;
	set <AddresSize, pairCompare> supportAllocatedBlocks;

	vector <int> pages; // хранит количество используемых блоков в соотв странице

	int pageSize;
	int maxMediumBlockSize;

	int roundUp(int value, int divisor);
	int getBlockSize(LPVOID pointer);

	void addFreeBlock(LPVOID start, int size);

	AddresSize findSuitableFreeBlock(int size);
	void* updateFreeBlocks(AddresSize block, int size);

	void mergeFreeBlocks(LPVOID blockPtr, int blockSize);
	AddresSize getLeftBarier(LPVOID blockPtr, int blockSize);
	AddresSize getRigthBarier(LPVOID blockPtr, int blockSize);
};
