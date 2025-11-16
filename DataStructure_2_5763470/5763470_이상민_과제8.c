#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 무작위 데이터 생성 상수
#define N 10000           
#define RANGE 1000000     
#define TESTS 100         


// Tokuda gap으로 최적의 쉘 간격 설정, h(k) = (9^k – 4) / 5
static const int tokuda_gaps[] = {
    1, 4, 9, 20, 46, 103, 233, 525, 1182, 2660, 5985
};
static const int tokuda_gcount = sizeof(tokuda_gaps) / sizeof(tokuda_gaps[0]);

void gen_random_array(int* a, int n) {
    for (int i = 0; i < n; ++i) {
        a[i] = rand() % RANGE;
    }
}

/* 단순 삽입 정렬 (비교 횟수 측정)
   비교 카운트 정의: "배열 요소끼리 비교한 횟수"만 센다.
   구현 방식: while 루프에서 j >= 0 검사는 인덱스 검사이므로
               arr[j]와 key 비교가 실제 요소 비교일 때만 카운트한다.
   100회 누적 후 평균을 내야하는데 오버플로우 발생하므로 long long 사용
*/

long long insertion_sort_count(int* arr, int n) {
    long long comps = 0;
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0) {
            comps++; // arr[j] > key 인지 검사 -> 요소 비교 
            if (arr[j] > key) {
                arr[j + 1] = arr[j];
                --j;
            }
            else {
                break;
            }
        }
        arr[j + 1] = key;
    }
    return comps;
}

/* 기본 쉘 정렬 (gap = n/2, n/4, ... )
   비교 카운트: 삽입 단계에서 실제 요소 비교가 발생할 때마다 카운트
*/

long long shell_sort_basic_count(int* arr, int n) {
    long long comps = 0;
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; ++i) {
            int temp = arr[i];
            int j = i;
            while (j >= gap) {
                comps++;  // arr[j-gap] > temp 인지 비교 -> 요소 비교 
                if (arr[j - gap] > temp) {
                    arr[j] = arr[j - gap];
                    j -= gap;
                }
                else {
                    break;
                }
            }
            arr[j] = temp;
        }
    }
    return comps;
}

/* Tokuda 기반 쉘 정렬 (사전 정의된 gaps 사용) 
   tokuda_gaps[] 는 오름차순으로 정의되어 있으므로 '큰 gap부터' 사용해야 함.
   비교 카운트 규칙은 동일.
*/

long long shell_sort_tokuda_count(int* arr, int n) {
    long long comps = 0;
    for (int gi = tokuda_gcount - 1; gi >= 0; --gi) {
        int gap = tokuda_gaps[gi];
        if (gap >= n) continue; // N보다 큰 gap 무시 
        for (int i = gap; i < n; ++i) {
            int temp = arr[i];
            int j = i;
            while (j >= gap) {
                comps++;  // 요소 비교 
                if (arr[j - gap] > temp) {
                    arr[j] = arr[j - gap];
                    j -= gap;
                }
                else {
                    break;
                }
            }
            arr[j] = temp;
        }
    }
    return comps;
}

// 100회 반복 후 평균 계산 및 출력 
int main(void) {
    // 현재 시각으로 난수 생성
    srand((unsigned)time(NULL));

    // 동적 할당: 스택에 큰 배열을 놓지 않기 위해 malloc 사용 
    int* original = (int*)malloc(sizeof(int) * N);
    int* work = (int*)malloc(sizeof(int) * N);
    if (!original || !work) {
        fprintf(stderr, "메모리 할당 실패\n");
        return 1;
    }

    long long total_ins = 0;
    long long total_shell_basic = 0;
    long long total_shell_tokuda = 0;

    for (int t = 0; t < TESTS; ++t) {
        gen_random_array(original, N);

        // 삽입 정렬 
        memcpy(work, original, sizeof(int) * N);
        total_ins += insertion_sort_count(work, N);

        // 기본 쉘 정렬 
        memcpy(work, original, sizeof(int) * N);
        total_shell_basic += shell_sort_basic_count(work, N);

        // Tokuda 쉘 정렬 
        memcpy(work, original, sizeof(int) * N);
        total_shell_tokuda += shell_sort_tokuda_count(work, N);
    }

    // 평균 계산(정수 나눗셈) 
    printf("===== 결과: 평균 비교 횟수 (%d회) =====\n", TESTS);
    printf("단순 삽입 정렬 : %lld\n", total_ins / TESTS);
    printf("기본 쉘 정렬 : %lld\n", total_shell_basic / TESTS);
    printf("Tokuda 쉘 정렬 : %lld\n", total_shell_tokuda / TESTS);

    free(original);
    free(work);
    return 0;
}
