#include "CHeapManager.h"
#include <iostream>
using namespace std;

CHeapManager::CHeapManager() {
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	pageSize = info.dwPageSize;
	maxMediumBlockSize = 32 * pageSize;
	pages = vector<int>();
	freeSmallBlocks = set <LPVOID>();
	freeBigBlocks = set <AddresSize>();
	freeMediumBlocks = set <AddresSize>();
}

CHeapManager::~CHeapManager() {
}

void CHeapManager::Create(int _minSize, int _maxSize) {
	minSize = roundUp(_minSize, pageSize);
	maxSize = roundUp(_maxSize, pageSize);
	//резевируем адресное пространство
	startAddress = ::VirtualAlloc(
		NULL,									// базовые адрес
		maxSize,								// размер
		MEM_RESERVE,							// способ получения
		PAGE_READWRITE);						// тип доступа
	if (startAddress == 0) {
		cout << "ERROR : VirtualAlloc return NULL in Create with MEM_RESERVE" << endl;
		system("pause");
		exit(1);
	}

	LPVOID commitedSpace = ::VirtualAlloc(startAddress, minSize, MEM_COMMIT, PAGE_READWRITE);
	if (commitedSpace == 0) {
		cout << "ERROR : VirtualAlloc return NULL in Create with MEM_COMMIT" << endl;
		system("pause");
		exit(1);
	}

	int numberOfPages = maxSize / pageSize;
	pages.resize(numberOfPages, 0);
	//	numberOfPages = minSize / pageSize;
	//	for (int i = 0; i <= numberOfPages; ++i) {
	//		pages[i]++;
	//	}
	//	pages[0] = 1;
	addFreeBlock(startAddress, maxSize);
}

void* CHeapManager::Alloc(int size) {
	size = roundUp(size, sizeof(int));
	void* result = nullptr;

	AddresSize suitableFreeBlock = findSuitableFreeBlock(size);
	if (suitableFreeBlock.second == -1) {
		system("pause");
		exit(1);
	} else {
		result = updateFreeBlocks(suitableFreeBlock, size);
	}
	return result;
}

void CHeapManager::Free(void *mem) {
	mem = static_cast<byte*>(mem) - sizeof(int);
	int blockSize;
	memcpy(&blockSize, mem, sizeof(int));

	int offset = static_cast<byte*>(mem) - static_cast<byte*>(startAddress);
	int startPage = offset / pageSize;
	int endPage = (offset + sizeof(int) + blockSize - 1) / pageSize;

	vector <int> pagesToFree;
	for (int i = startPage; i <= endPage; i++) {
		pages[i]--;
		if (pages[i] == 0) {
			pagesToFree.push_back(i);
		}
	}

	mergeFreeBlocks(mem, blockSize);

	allocatedBlocks.erase(mem);
	supportAllocatedBlocks.erase(make_pair(mem, blockSize));

	if (!pagesToFree.empty()) {
		int firstPage = pagesToFree[0];
		int lastPage = pagesToFree[pagesToFree.size() - 1];
		VirtualFree(static_cast<byte*>(startAddress) + firstPage * pageSize, (lastPage - firstPage + 1) * pageSize, MEM_DECOMMIT);
	}
}

void CHeapManager::Destroy() {
	for (auto it = allocatedBlocks.begin(); it != allocatedBlocks.end(); it++) {
		cout << "Unfreed memory: start " << *it << " size: " << getBlockSize(*it) << endl;
	}
	VirtualFree(startAddress, maxSize, MEM_RELEASE);
	startAddress = 0;
	pages = vector <int>();
	freeSmallBlocks = set <LPVOID>();
	freeMediumBlocks = set <AddresSize>();
	freeBigBlocks = set <AddresSize>();
}

//-----------------------------------------------------------------------------------------

int CHeapManager::roundUp(int value, int divisor) {
	if (value % divisor != 0) {
		value = (value / divisor + 1) * divisor;
	}
	return value;
}

int CHeapManager::getBlockSize(LPVOID pointer) {
	int result;
	memcpy(&result, pointer, sizeof(int));
	return result;
}

void CHeapManager::addFreeBlock(LPVOID start, int size) {
	if (size < pageSize) {
		::memcpy(start, &size, sizeof(int));					// записываем размер блока
																//		int chkSize;
																//		::memcpy(&chkSize, start, sizeof(int));
		freeSmallBlocks.insert(start);
	} else if (size < maxMediumBlockSize) {
		freeMediumBlocks.insert(make_pair(start, size + sizeof(int)));
	} else {
		freeBigBlocks.insert(make_pair(start, size + sizeof(int)));
	}
}

AddresSize CHeapManager::findSuitableFreeBlock(int size) {
	AddresSize result = make_pair(nullptr, -1);
	if (size + sizeof(int) < pageSize) {
		if (freeSmallBlocks.size() > 0) {
			for (auto it = freeSmallBlocks.begin(); it != freeSmallBlocks.end(); it++) {
				int freeMemory;
				::memcpy(&freeMemory, *it, sizeof(int));
				if (freeMemory >= size + sizeof(int)) {
					result = make_pair(*it, freeMemory);
					freeSmallBlocks.erase(it);
					return result;
				}
			}
		}
		if (freeMediumBlocks.size() > 0) {
			for (auto it = freeMediumBlocks.begin(); it != freeMediumBlocks.end(); it++) {
				int freeMemory = it->second;
				if (freeMemory >= size + sizeof(int)) {
					result = *it;
					freeMediumBlocks.erase(it);
					return result;
				}
			}
		}
		if (freeBigBlocks.size() > 0) {
			for (auto it = freeBigBlocks.begin(); it != freeBigBlocks.end(); it++) {
				int freeMemory = it->second;
				if (freeMemory >= size + sizeof(int)) {
					result = *it;
					freeBigBlocks.erase(it);
					return result;
				}
			}
		}
	}
	if (pageSize <= size + sizeof(int) && size + sizeof(int) <	maxMediumBlockSize) {
		if (freeMediumBlocks.size() > 0) {
			for (auto it = freeMediumBlocks.begin(); it != freeMediumBlocks.end(); it++) {
				int freeMemory = it->second;
				if (freeMemory >= size + sizeof(int)) {
					result = *it;
					freeMediumBlocks.erase(it);
					return result;
				}
			}
		}
		if (freeBigBlocks.size() > 0) {
			for (auto it = freeBigBlocks.begin(); it != freeBigBlocks.end(); it++) {
				int freeMemory = it->second;
				if (freeMemory >= size + sizeof(int)) {
					result = *it;
					freeBigBlocks.erase(it);
					return result;
				}
			}
		}
	}
	if (maxMediumBlockSize <= size + sizeof(int)) {
		if (freeBigBlocks.size() > 0) {
			for (auto it = freeBigBlocks.begin(); it != freeBigBlocks.end(); it++) {
				int freeMemory = it->second;
				if (freeMemory >= size + sizeof(int)) {
					result = *it;
					freeBigBlocks.erase(it);
					return result;
				}
			}
		}
	}
	if (result.second == -1) {
		cout << "ERROR : not enough free blocks in findSuitableFreeBlock" << endl;
		system("pause");
	}
	return result;
}

void* CHeapManager::updateFreeBlocks(AddresSize block, int size) {
	LPVOID blockPtr = block.first;
	int blockSize = block.second;

	int offset = static_cast<byte*>(blockPtr) - static_cast<byte*>(startAddress);
	int startPage = offset / pageSize;
	int endPage = (offset + sizeof(int) + size - 1) / pageSize;

	LPVOID commitedSpace = ::VirtualAlloc(blockPtr, size + sizeof(int), MEM_COMMIT, PAGE_READWRITE);
	if (commitedSpace == 0) {
		cout << "ERROR : in VirtualAlloc commitedSpace == NULL in updateFreeBlocks" << endl;
		system("pause");
		exit(1);
	}
	memcpy(blockPtr, &size, sizeof(int));						// записали размер в начало алокируемой памяти

																//	int checkSize;												////////
																//	memcpy(&checkSize, blockPtr, sizeof(int));					////////

	for (int i = startPage; i <= endPage; i++) {
		pages[i] += 1;
	}
	if (size + sizeof(int) < blockSize) {
		addFreeBlock(static_cast<byte*>(blockPtr) + sizeof(int) + size, blockSize - size - sizeof(int));
	}
	allocatedBlocks.insert(blockPtr);
	supportAllocatedBlocks.insert(make_pair(blockPtr, size));	// отдельно сохрнаим, то что записано в размер перед алоцируемой памятью
	return static_cast<byte*>(blockPtr) + sizeof(int);
}

void CHeapManager::mergeFreeBlocks(LPVOID blockPtr, int blockSize) {
	AddresSize leftBarierAndSize = make_pair(blockPtr, 0);
	AddresSize rigthBarierAndSize;

	if (blockPtr != startAddress) {
		leftBarierAndSize = getLeftBarier(blockPtr, blockSize);
	}

	if (static_cast<byte*>(blockPtr) - static_cast<byte*>(startAddress) + blockSize < maxSize) {
		rigthBarierAndSize = getRigthBarier(blockPtr, blockSize);
	}
	LPVOID leftBarier = leftBarierAndSize.first;
	int size = leftBarierAndSize.second + blockSize + rigthBarierAndSize.second;
	addFreeBlock(leftBarier, size);
}

AddresSize CHeapManager::getLeftBarier(LPVOID blockPtr, int blockSize) {
	AddresSize result = make_pair(blockPtr, 0);
	if (!freeSmallBlocks.empty()) {
		set <LPVOID>::iterator it = freeSmallBlocks.upper_bound(blockPtr);
		if (it == freeSmallBlocks.begin()) {
			return result;
		}
		it--;
		LPVOID freeBlockPtr = *it;
		int freeBlockSize = getBlockSize(*it);
		if (static_cast<byte*>(freeBlockPtr) + freeBlockSize + sizeof(int) == static_cast<byte*>(blockPtr)) {
			result.first = freeBlockPtr;
			result.second = freeBlockSize;
			freeSmallBlocks.erase(it);
			return result;
		}
	}
	if (!freeMediumBlocks.empty()) {
		set <AddresSize>::iterator it = freeMediumBlocks.upper_bound(result);
		if (it == freeMediumBlocks.begin()) {
			return result;
		}
		it--;
		LPVOID freeBlockPtr = it->first;
		int freeBlockSize = it->second;
		//		auto leftarg0 = static_cast<byte*>(freeBlockPtr)+freeBlockSize;
		//		auto leftarg = static_cast<byte*>(freeBlockPtr)+freeBlockSize + sizeof(int);
		//		auto rightarg = static_cast<byte*>(blockPtr);
		if (static_cast<byte*>(freeBlockPtr) + freeBlockSize + sizeof(int) == static_cast<byte*>(blockPtr)) {
			result.first = freeBlockPtr;
			result.second = freeBlockSize;
			freeMediumBlocks.erase(it);
			return result;
		}
	}
	if (!freeBigBlocks.empty()) {
		set <AddresSize>::iterator it = freeBigBlocks.upper_bound(result);
		if (it == freeBigBlocks.begin()) {
			return result;
		}
		it--;
		LPVOID freeBlockPtr = it->first;
		int freeBlockSize = it->second;
		if (static_cast<byte*>(freeBlockPtr) + freeBlockSize + sizeof(int) == static_cast<byte*>(blockPtr)) {
			result.first = freeBlockPtr;
			result.second = freeBlockSize;
			freeBigBlocks.erase(it);
			return result;
		}
	}
	return result;
}

AddresSize CHeapManager::getRigthBarier(LPVOID blockPtr, int blockSize) {
	AddresSize result = make_pair(static_cast<LPVOID>(0), 0);
	if (!freeSmallBlocks.empty()) {
		set <LPVOID>::iterator it = freeSmallBlocks.upper_bound(blockPtr);
		if (it != freeSmallBlocks.end()) {
			LPVOID freeBlockPtr = *it;
			int freeBlockSize = getBlockSize(*it);
			if (static_cast<byte*>(blockPtr) + blockSize + sizeof(int) == static_cast<byte*>(freeBlockPtr)) {
				result.first = freeBlockPtr;
				result.second = freeBlockSize;
				freeSmallBlocks.erase(it);
				return result;
			}
		}
	}
	if (!freeMediumBlocks.empty()) {
		set <AddresSize>::iterator it = freeMediumBlocks.upper_bound(result);
		if (it != freeMediumBlocks.end()) {
			LPVOID freeBlockPtr = it->first;
			int freeBlockSize = it->second;
			if (static_cast<byte*>(blockPtr) + blockSize + sizeof(int) == static_cast<byte*>(freeBlockPtr)) {
				result.first = freeBlockPtr;
				result.second = freeBlockSize;
				freeMediumBlocks.erase(it);
				return result;
			}
		}
	}
	if (!freeBigBlocks.empty()) {
		set <AddresSize>::iterator it = freeBigBlocks.upper_bound(result);
		if (it != freeBigBlocks.end()) {
			LPVOID freeBlockPtr = it->first;
			int freeBlockSize = it->second;
			if (static_cast<byte*>(blockPtr) + blockSize + sizeof(int) == static_cast<byte*>(freeBlockPtr)) {
				result.first = freeBlockPtr;
				result.second = freeBlockSize;
				freeBigBlocks.erase(it);
				return result;
			}
		}
	}
	return result;
}
