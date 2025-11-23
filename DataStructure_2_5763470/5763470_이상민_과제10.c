#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CSV_PATH "dataset_id_ascending.csv"
#define MAX_LINE 200
#define MAX_NAME_LEN 50
#define TESTS 10

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char gender;
    int kor, eng, math;
    long long product; // 세 과목 곱
} Student;

/* ================= 파일 로드 ================= */
Student* load_students(const char* filename, int* out_count) {
    FILE* fp = fopen(filename, "r");
    if (!fp) { perror("파일 열기 실패"); return NULL; }

    char line[MAX_LINE];
    int capacity = 16;
    int count = 0;
    Student* arr = malloc(sizeof(Student) * capacity);
    if (!arr) { fclose(fp); perror("메모리 할당 실패"); return NULL; }

    // 헤더 스킵
    if (!fgets(line, sizeof(line), fp)) { fclose(fp); free(arr); return NULL; }

    while (fgets(line, sizeof(line), fp)) {
        if (count >= capacity) {
            capacity *= 2;
            Student* tmp = realloc(arr, sizeof(Student) * capacity);
            if (!tmp) { perror("realloc 실패"); free(arr); fclose(fp); return NULL; }
            arr = tmp;
        }

        char buf[MAX_LINE];
        strncpy(buf, line, MAX_LINE - 1); buf[MAX_LINE - 1] = 0;

        char* tok = strtok(buf, ",");
        if (!tok) continue;
        arr[count].id = atoi(tok);

        tok = strtok(NULL, ",");
        if (!tok) arr[count].name[0] = 0;
        else { strncpy(arr[count].name, tok, MAX_NAME_LEN - 1); arr[count].name[MAX_NAME_LEN - 1] = 0; char* p = strchr(arr[count].name, '\n'); if (p)*p = 0; }

        tok = strtok(NULL, ",");
        arr[count].gender = (tok && tok[0]) ? tok[0] : 'U';

        tok = strtok(NULL, ",");
        arr[count].kor = tok ? atoi(tok) : 0;

        tok = strtok(NULL, ",");
        arr[count].eng = tok ? atoi(tok) : 0;

        tok = strtok(NULL, ",");
        arr[count].math = tok ? atoi(tok) : 0;

        // 곱 계산 (long long)
        arr[count].product = (long long)arr[count].kor * arr[count].eng * arr[count].math;

        count++;
    }

    fclose(fp);
    *out_count = count;
    return arr;
}

/* ================= 순차 탐색 ================= */
long long sequential_search(Student* arr, int n, long long target) {
    long long comp = 0;
    for (int i = 0; i < n; i++) {
        comp++;
        if (arr[i].product == target) return comp;
    }
    return comp; // 못 찾으면 전체 탐색
}

/* ================= 비교 함수 ================= */
int compare_product(const void* a, const void* b) {
    Student* s1 = (Student*)a;
    Student* s2 = (Student*)b;
    return (s1->product < s2->product) ? -1 : (s1->product > s2->product ? 1 : 0);
}

/* ================= 이진 탐색 ================= */
long long binary_search(Student* arr, int n, long long target) {
    long long comp = 0;
    int left = 0, right = n - 1;
    while (left <= right) {
        comp++;
        int mid = left + (right - left) / 2;
        if (arr[mid].product == target) return comp;
        else if (arr[mid].product < target) left = mid + 1;
        else right = mid - 1;
    }
    return comp;
}

/* ================= 랜덤 target 생성 ================= */
long long generate_random_target() {
    return ((long long)rand() << 31 | rand() << 15 | rand()) % 1000001;
}

/* ================= 메인 ================= */
int main(void) {
    srand((unsigned int)(time(NULL) ^ clock()));

    int n;
    Student* arr = load_students(CSV_PATH, &n);
    if (!arr) { fprintf(stderr, "데이터 로드 실패\n"); return 1; }
    printf("Loaded %d records from %s\n", n, CSV_PATH);

    for (int t = 0; t < TESTS; t++) {
        long long target = generate_random_target();
        printf("\n=== 테스트 %d : target 값 = %lld ===\n", t + 1, target);

        long long seq_comp = sequential_search(arr, n, target);
        printf("순차 탐색 비교횟수: %lld\n", seq_comp);

        // 정렬 후 이진 탐색
        qsort(arr, n, sizeof(Student), compare_product);
        long long bin_comp = binary_search(arr, n, target);
        printf("정렬 + 이진 탐색 비교횟수: %lld\n", bin_comp);
    }

    free(arr);
    return 0;
}
