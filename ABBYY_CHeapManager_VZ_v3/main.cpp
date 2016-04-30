#include "CHeapManager.h"
#include "TestClasses.h"
#include <iostream>
#include <time.h>
#include <chrono>
#include <Psapi.h>
#include <algorithm>
#include <list>
using namespace std;
using namespace std::chrono;

void test0(int n) {
	heapManager.Create(64 * KB, 256 * n *KB);
	list <CMyHeapRandom*> l;
	list <CMyHeapRandom*> v;
	//	n = 4;
	for (int i = 0; i < n; ++i) {
		//		cout << i << ' ';
		CMyHeapRandom* tmp = new CMyHeapRandom();

		l.push_back(tmp);
		v.push_back(tmp);
	}
	//	cout << sizeof(heapManager) << endl;
	cout << "Alloc Success" << endl;
	int i = 0;
	for (auto it = l.begin(); it != l.end(); ++it) {
		//		cout << i << ' '; ++i;
		if (i == 23) {
			system("pause");
		}
		CMyHeapRandom* toDel = *it;
		delete (toDel);
	}
	cout << "Free Success" << endl;
	l.swap(list <CMyHeapRandom*>());
}

void testTimeStandartHeap(int steps, int startDeletAtStep) {
	high_resolution_clock::time_point time1 = high_resolution_clock::now();
	list<C_256KB_standart*> l;

	for (int i = 0; i < steps; ++i) {
		C_256KB_standart* c = new C_256KB_standart();
		l.push_back(c);

		if (i >= startDeletAtStep) {
			int r = rand() % l.size();
			auto it = l.begin();
			for (int j = 0; j < r; ++j) {
				++it;
			}
			C_256KB_standart* toDelete = *it;
			l.erase(it);
			delete(toDelete);
		}
	}
	high_resolution_clock::time_point time2 = high_resolution_clock::now();

	auto duration = duration_cast<microseconds>(time2 - time1).count();
	cout << "Standart: " << duration << " microseconds" << endl;
}

void testTimeMyHeap(int steps, int startDeletAtStep) {
	high_resolution_clock::time_point time1 = high_resolution_clock::now();
	if (startDeletAtStep > steps) {
		cout << "ERROR : not possible params" << endl;
		return;
	}
	heapManager = CHeapManager();

	heapManager.Create(4 * KB, 2 * 256 * steps * KB);
	list<C_256KB*> l;

	for (int i = 0; i < steps; ++i) {
		C_256KB* c = new C_256KB();
		l.push_back(c);
		//		cout << i << endl;
		if (i >= startDeletAtStep) {
			int r = rand() % l.size();
			auto it = l.begin();
			for (int j = 0; j < r; ++j) {
				++it;
			}
			C_256KB* toDelete = *it;
			l.erase(it);
			delete(toDelete);
		}
	}
	high_resolution_clock::time_point time2 = high_resolution_clock::now();
	//	heapManager.Destroy();

	auto duration = duration_cast<microseconds>(time2 - time1).count();
	cout << "My  Heap: " << duration << " microseconds" << endl;
}

void testTime() {
	int steps = 2000;
	int deleteAt = 100;
	testTimeStandartHeap(steps, deleteAt);
	testTimeMyHeap(steps, deleteAt);
}
/*
void testTimeStandartHeap3(int n) {
list <CRandomSize_standart*> l;

high_resolution_clock::time_point time1 = high_resolution_clock::now();
for (int i = 0; i < n; i++) {
int monetka = rand() % 2;
if (monetka) {
CRandomSize_standart* tmp = new CRandomSize_standart();
l.push_back(tmp);
}
else {
if (!l.empty()) {
auto it = l.begin();
CRandomSize_standart* toDelete = *it;
l.erase(l.begin());
delete(toDelete);
} else {
CRandomSize_standart* tmp = new CRandomSize_standart();
l.push_back(tmp);
}
}
}
high_resolution_clock::time_point time2 = high_resolution_clock::now();

auto duration = duration_cast<microseconds>(time2 - time1).count();
cout << "Standart: " << duration << " microseconds" << endl;
}
//*/
void testTimeStandartHeap2(int n) {
	list <char*> l;

	high_resolution_clock::time_point time1 = high_resolution_clock::now();
	for (int i = 0; i < n; i++) {
		int size = rand() % 64 * KB;
		int monetka = rand() % 2;
		if (monetka) {
			char* tmp = new char(size);
			l.push_back(tmp);
		} else {
			if (!l.empty()) {
				auto it = l.begin();
				delete *it;
				l.erase(l.begin());
			} else {
				char* tmp = new char(size);
				l.push_back(tmp);
			}
		}
	}
	high_resolution_clock::time_point time2 = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(time2 - time1).count();
	cout << "Standart: " << duration << " microseconds" << endl;

}

void testTimeMyHeap2(int n) {
	list<CRandomSize*> l;

	high_resolution_clock::time_point time1 = high_resolution_clock::now();

	heapManager = CHeapManager();
	heapManager.Create(4 * KB, 2 * 256 * n * KB);

	for (int i = 0; i < n; ++i) {
		std::cout << i << ' ';
		if (i == 80) {
			system("pause");
		}
		int monetka = rand() % 2;
		if (monetka) {
			CRandomSize* c = new CRandomSize();
			l.push_back(c);
		} else {
			if (!l.empty()) {
				auto it = l.begin();
				CRandomSize* toDelete = *it;
				l.erase(it);
				delete(toDelete);
			} else {
				CRandomSize* c = new CRandomSize();
				l.push_back(c);
			}
		}
	}
	high_resolution_clock::time_point time2 = high_resolution_clock::now();
	//	heapManager.Destroy();

	auto duration = duration_cast<microseconds>(time2 - time1).count();
	std::cout << endl << "My  Heap: " << duration << " microseconds" << endl;
}

void testTime2() {
	int n = 100;
	//	testTimeStandartHeap2(n);
	testTimeMyHeap2(n);
}

void testTimeStandartHeap3(int n) {
	high_resolution_clock::time_point time1 = high_resolution_clock::now();
	defaultHeapHandle = HeapCreate(0, 64 * KB, 32 * n * KB);

	list <CDefaultHeapRandom*> l;
	for (int i = 0; i < n; ++i) {
		CDefaultHeapRandom* tmp = new CDefaultHeapRandom();
		l.push_back(tmp);
	}

	high_resolution_clock::time_point time2 = high_resolution_clock::now();

	int i = 0;
	for (auto it = l.begin(); it != l.end(); ++it) {
		//		cout << i << " "; ++i;
		CDefaultHeapRandom* toDel = *it;
		delete (toDel);
	}

	HeapDestroy(defaultHeapHandle);
	high_resolution_clock::time_point time3 = high_resolution_clock::now();

	auto duration = duration_cast<microseconds>(time3 - time1).count();
	auto delta1 = duration_cast<microseconds>(time2 - time1).count();
	auto delta2 = duration_cast<microseconds>(time3 - time2).count();
	cout << "Standart Heap: " << duration << "   microseconds" << endl;
	cout << "Time To Alloc: " << delta1 << "    microseconds" << endl;
	cout << "Time To Free : " << delta2 << "   microseconds" << endl;
}

void testTimeMyHeap3(int n) {
	high_resolution_clock::time_point time1 = high_resolution_clock::now();

	heapManager.Create(64 * KB, 32 * n * KB);
	list <CMyHeapRandom*> l;
	for (int i = 0; i < n; ++i) {
		CMyHeapRandom* tmp = new CMyHeapRandom();
		l.push_back(tmp);
	}

	high_resolution_clock::time_point time2 = high_resolution_clock::now();

	for (auto it = l.begin(); it != l.end(); ++it) {
		CMyHeapRandom* toDel = *it;
		delete (toDel);
	}

	heapManager.Destroy();
	high_resolution_clock::time_point time3 = high_resolution_clock::now();

	auto duration = duration_cast<microseconds>(time3 - time1).count();
	auto delta1 = duration_cast<microseconds>(time2 - time1).count();
	auto delta2 = duration_cast<microseconds>(time3 - time2).count();
	cout << "My  Heap Time: " << duration << "   microseconds" << endl;
	cout << "Time To Alloc: " << delta1 << "   microseconds" << endl;
	cout << "Time To Free : " << delta2 << "    microseconds" << endl;
}

void testTime3() {
	int n = 20000;
	testTimeStandartHeap3(n);
	std::cout << endl;
	testTimeMyHeap3(n);
}


int main() {
	//	test0(1000);
	//	testTime();
	//	testTime2();
	testTime3();
	std::cout << endl << "end of tests" << endl;
	system("pause");
	return 0;
}