#include "CHeapManager.h"
#include "TestClasses.h"
#include <iostream>
#include <time.h>
#include <chrono>
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

void testTimeStandartHeap(int n) {
	high_resolution_clock::time_point time1 = high_resolution_clock::now();
	standartHeapManager = HeapCreate(0, 64 * KB, 32 * n * KB);

	list <CStandartHeapRandom*> l;
	for (int i = 0; i < n; ++i) {
		CStandartHeapRandom* tmp = new CStandartHeapRandom();
		l.push_back(tmp);
	}

	high_resolution_clock::time_point time2 = high_resolution_clock::now();

	int i = 0;
	for (auto it = l.begin(); it != l.end(); ++it) {
		//		cout << i << " "; ++i;
		CStandartHeapRandom* toDel = *it;
		delete (toDel);
	}

	HeapDestroy(standartHeapManager);
	high_resolution_clock::time_point time3 = high_resolution_clock::now();

	auto duration = duration_cast<microseconds>(time3 - time1).count();
	auto delta1 = duration_cast<microseconds>(time2 - time1).count();
	auto delta2 = duration_cast<microseconds>(time3 - time2).count();
	cout << "Standart Heap: " << duration << "   microseconds" << endl;
//	cout << "Time To Alloc: " << delta1 << "    microseconds" << endl;
//	cout << "Time To Free : " << delta2 << "   microseconds" << endl;
}

void testTimeMyHeap(int n) {
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
//	cout << "Time To Alloc: " << delta1 << "   microseconds" << endl;
//	cout << "Time To Free : " << delta2 << "    microseconds" << endl;
}

void testTime() {
	int n = 10000;
	testTimeMyHeap(n);
	cout << endl;
	testTimeStandartHeap(n);
}


int main() {
	testTime();
	cout << endl << "end of tests" << endl;
	system("pause");
	return 0;
}