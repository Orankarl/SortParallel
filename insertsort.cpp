#include "stdlib.h"
#include "iostream"
#include "omp.h"
#include "time.h"
#include "vector"
#include "stack"
#include <ctime>
#include <cstdio>
using namespace std;

//#define random(x) (rand()%x)
#define BOUNDARY 100000	//定义随机数产生的区间
#define MAX_NUM 100000	//随机数组的元素个数
// const double MinProb = 1.0 / (RAND_MAX + 1);	//概率
//定义一个记录待排序的区间[low,high]  
typedef struct Region
{
	int low;
	int high;
}Region;

vector<int> InsertSortParallel(int N, int *p);
vector<int> InsertVectorSort(vector<int> &vec);

int main() {
	int *p = new int[MAX_NUM];//p用作并行插入排序数据
	vector<int> v(MAX_NUM);//q用作串行插入排序数据
	srand(time(NULL));
	for (int i = 0; i < MAX_NUM; i++) {
		v[i] = p[i] = rand() % BOUNDARY;
	}InsertVectorSort(v);
	InsertSortParallel(MAX_NUM, p);
	
	return 0;
}

int* InsertSort(int N, int *p)//插入排序
{
	int temp;
	for (int i = 1; i < N; i++) {
		for (int j = i; (j > 0) && (p[j] < p[j - 1]); j--) {
			temp = p[j];
			p[j] = p[j - 1];
			p[j - 1] = temp;
		}
	}
	return p;
}

vector<int> InsertVectorSort(vector<int> &vec)
{
	int temp;
	int start = clock();
	for (int i = 1; i < vec.size(); i++) {
		for (int j = i; j > 0; j--) {
			if (vec[j] < vec[j - 1]) {
				temp = vec[j];
				vec[j] = vec[j - 1];
				vec[j - 1] = temp;
				// swap(vec[j - 1], vec[j]);
			} else if (vec[j] >= vec[j - 1])
				break;
		}
	}
	int end = clock();
	printf("The serial time is:%lf\n", (double)(end - start)/CLOCKS_PER_SEC);
	return vec;
}

vector<int> InsertSortParallel(int N, int *p)//插入排序并行
{
	long start = clock();
	int i;
	int interval = BOUNDARY / 4;
	vector<int> vec[4];

	//将数据根据大小分为四个区间	
	for (i = 0; i < N; i++) {
		if (p[i] < interval)
			vec[0].push_back(p[i]);
		else if (p[i] < 2 * interval)
			vec[1].push_back(p[i]);
		else if (p[i] < 3 * interval)
			vec[2].push_back(p[i]);
		else
			vec[3].push_back(p[i]);
	}

	int* arr0 = new int[vec[0].size()];
	int* arr1 = new int[vec[1].size()];
	int* arr2 = new int[vec[2].size()];
	int* arr3 = new int[vec[3].size()];
	for (i = 0; i < vec[0].size(); i++)
		arr0[i] = vec[0][i];
	for (i = 0; i < vec[1].size(); i++)
		arr1[i] = vec[1][i];
	for (i = 0; i < vec[2].size(); i++)
		arr2[i] = vec[2][i];
	for (i = 0; i < vec[3].size(); i++)
		arr3[i] = vec[3][i];
	omp_set_num_threads(4);

	//分成四个线程执行插入排序
#pragma omp parallel
	{
#pragma omp sections
	{
#pragma omp section
	{
		arr0 = InsertSort(vec[0].size(), arr0);
		//printf("%d\n", omp_get_thread_num());
	}
#pragma omp section
	{
		arr1 = InsertSort(vec[1].size(), arr1);
		//printf("%d\n", omp_get_thread_num());
	}
#pragma omp section
	{
		arr2 = InsertSort(vec[2].size(), arr2);
		//printf("%d\n", omp_get_thread_num());
	}
#pragma omp section
	{
		arr3 = InsertSort(vec[3].size(), arr3);
		//printf("%d\n", omp_get_thread_num());
	}
	}
	}

	vector<int> vec1[4];
	for (i = 0; i < vec[0].size(); i++) {
		vec1[0].push_back(arr0[i]);
	}
	cout << endl;
	for (i = 0; i < vec[1].size(); i++) {
		vec1[1].push_back(arr1[i]);
	}
	for (i = 0; i < vec[2].size(); i++)
		vec1[2].push_back(arr2[i]);
	for (i = 0; i < vec[3].size(); i++)
		vec1[3].push_back(arr3[i]);
	vec1[0].insert(vec1[0].end(), vec1[1].begin(), vec1[1].end());
	vec1[0].insert(vec1[0].end(), vec1[2].begin(), vec1[2].end());
	vec1[0].insert(vec1[0].end(), vec1[3].begin(), vec1[3].end());

	long end = clock();

	printf("The time of InsertSortParallel is:%lf\n", (double)(end - start)/CLOCKS_PER_SEC);
	// for (int i = 0; i < vec1[0].size(); i++) {
	// 	cout << vec1[0][i] << ' ';
	// }
	// return vec[0];
	return vec1[0];
}