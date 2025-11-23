#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================= 설정 ================= */
#define CSV_PATH "dataset_id_ascending.csv"  
#define MAX_LINE 200
#define MAX_NAME_LEN 50

#define TESTS 1000

/* ================= 구조체 ================= */
typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char gender; /* 'M' or 'F' */
    int kor, eng, math;
} Student;

/* ================= 전역 상태(정렬 기준 & 카운터) ================= */
typedef enum { KEY_ID, KEY_NAME, KEY_GENDER, KEY_SUM } KeyType;
KeyType CUR_KEY = KEY_ID;
int CUR_ASC = 1; /* 1 = ascending, 0 = descending */

/* 비교 카운트 (long long) */
long long COMP_COUNT = 0;

/* 정렬 중 추가로 malloc으로 할당한 바이트 누적 (각 실행마다 리셋) */
size_t MEM_COUNT = 0;

/* 비교 수행할 때는 compare_students()를 사용 */
int compare_students(const Student* a, const Student* b) {
    COMP_COUNT++;
    if (CUR_KEY == KEY_ID) {
        if (a->id < b->id) return CUR_ASC ? -1 : 1;
        if (a->id > b->id) return CUR_ASC ? 1 : -1;
        return 0;
    }
    else if (CUR_KEY == KEY_NAME) {
        int r = strcmp(a->name, b->name);
        if (r < 0) return CUR_ASC ? -1 : 1;
        if (r > 0) return CUR_ASC ? 1 : -1;
        return 0;
    }
    else if (CUR_KEY == KEY_GENDER) {
        if (a->gender < b->gender) return CUR_ASC ? -1 : 1;
        if (a->gender > b->gender) return CUR_ASC ? 1 : -1;
        return 0;
    }
    else { /* KEY_SUM with tie-break: kor, eng, math */
        int sa = a->kor + a->eng + a->math;
        int sb = b->kor + b->eng + b->math;
        if (sa != sb) return CUR_ASC ? (sa < sb ? -1 : 1) : (sa < sb ? 1 : -1);
        if (a->kor != b->kor) return CUR_ASC ? (a->kor < b->kor ? -1 : 1) : (a->kor < b->kor ? 1 : -1);
        if (a->eng != b->eng) return CUR_ASC ? (a->eng < b->eng ? -1 : 1) : (a->eng < b->eng ? 1 : -1);
        if (a->math != b->math) return CUR_ASC ? (a->math < b->math ? -1 : 1) : (a->math < b->math ? 1 : -1);
        if (a->id != b->id) return CUR_ASC ? (a->id < b->id ? -1 : 1) : (a->id < b->id ? 1 : -1);
        return 0;
    }
}

/* 비교자 매크로 */
#define CMP(a,b) compare_students((a),(b))

/* ================= 파일 로드 ================= */
Student* load_students(const char* filename, int* out_count) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("파일 열기 실패");
        return NULL;
    }
    char line[MAX_LINE];
    int capacity = 16;
    int count = 0;
    Student* arr = malloc(sizeof(Student) * capacity);
    if (!arr) { fclose(fp); perror("메모리 할당 실패"); return NULL; }

    /* 첫 줄 헤더 스킵 (있는 경우) */
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

        count++;
    }
    fclose(fp);
    Student* tight = realloc(arr, sizeof(Student) * count);
    if (tight) arr = tight;
    *out_count = count;
    return arr;
}

/* ================= 키 동등 검사 ================= */
int key_equal(const Student* a, const Student* b) {
    if (CUR_KEY == KEY_ID) return a->id == b->id;
    if (CUR_KEY == KEY_NAME) return strcmp(a->name, b->name) == 0;
    if (CUR_KEY == KEY_GENDER) return a->gender == b->gender;
    if (CUR_KEY == KEY_SUM) {
        int sa = a->kor + a->eng + a->math;
        int sb = b->kor + b->eng + b->math;
        return sa == sb && a->kor == b->kor && a->eng == b->eng && a->math == b->math;
    }
    return 0;
}

int has_duplicates_key(Student* arr, int n) {
    for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++)
        if (key_equal(&arr[i], &arr[j])) return 1;
    return 0;
}
/* ================= 정렬 알고리즘 구현 ================= */
/* 각 정렬 함수는 arr를 정렬하고, 비교/메모리는 전역 COMP_COUNT/MEM_COUNT를 사용 */

/* ---------- 1. Bubble (stable) ---------- */
void sort_bubble(Student* arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            if (CMP(&arr[j], &arr[j + 1]) > 0) {
                Student t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t;
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

/* ---------- 2. Selection (unstable) ---------- */
void sort_selection(Student* arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        int minidx = i;
        for (int j = i + 1; j < n; j++) {
            if (CMP(&arr[j], &arr[minidx]) < 0) minidx = j;
        }
        if (minidx != i) { Student t = arr[i]; arr[i] = arr[minidx]; arr[minidx] = t; }
    }
}

/* ---------- 3. Insertion (stable) ---------- */
void sort_insertion(Student* arr, int n) {
    for (int i = 1; i < n; i++) {
        Student key = arr[i];
        int j = i - 1;
        while (j >= 0 && CMP(&arr[j], &key) > 0) { arr[j + 1] = arr[j]; j--; }
        arr[j + 1] = key;
    }
}

/* ---------- 4. Shell (기본: gap = n/2, n/4, ...) ---------- */
void sort_shell_basic(Student* arr, int n) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            Student temp = arr[i];
            int j = i;
            while (j >= gap && CMP(&arr[j - gap], &temp) > 0) {
                arr[j] = arr[j - gap];
                j -= gap;
            }
            arr[j] = temp;
        }
    }
}

/* ---------- 4b. Shell 개선: Ciura 시퀀스 ---------- */
void sort_shell_ciura(Student* arr, int n) {
    /* Ciura seq (내림차순 적용 필요) */
    int ciura[] = { 701,301,132,57,23,10,4,1 };
    int cnt = sizeof(ciura) / sizeof(ciura[0]);
    for (int k = 0; k < cnt; k++) {
        int gap = ciura[k];
        if (gap >= n) continue;
        for (int i = gap; i < n; i++) {
            Student temp = arr[i];
            int j = i;
            while (j >= gap && CMP(&arr[j - gap], &temp) > 0) {
                arr[j] = arr[j - gap];
                j -= gap;
            }
            arr[j] = temp;
        }
    }
}

/* ---------- 5. Quick (basic Lomuto partition using median-of-three improvement in improved) ---------- */
/* 기본 quick: Hoare partition with middle pivot (no extra improvement) */
int partition_hoare(Student* arr, int low, int high) {
    Student pivot = arr[(low + high) / 2];
    int i = low - 1;
    int j = high + 1;
    while (1) {
        do { i++; } while (CMP(&arr[i], &pivot) < 0);
        do { j--; } while (CMP(&arr[j], &pivot) > 0);
        if (i >= j) return j;
        Student t = arr[i]; arr[i] = arr[j]; arr[j] = t;
    }
}
void sort_quick_basic_rec(Student* arr, int low, int high) {
    if (low < high) {
        int p = partition_hoare(arr, low, high);
        sort_quick_basic_rec(arr, low, p);
        sort_quick_basic_rec(arr, p + 1, high);
    }
}
void sort_quick_basic(Student* arr, int n) {
    if (n > 0) sort_quick_basic_rec(arr, 0, n - 1);
}

/* ---------- Improved Quick: median-of-three pivot ---------- */
int partition_med3(Student* arr, int low, int high) {
    int mid = (low + high) / 2;
    /* median-of-three ordering */
    if (CMP(&arr[low], &arr[mid]) > 0) { Student t = arr[low]; arr[low] = arr[mid]; arr[mid] = t; }
    if (CMP(&arr[low], &arr[high]) > 0) { Student t = arr[low]; arr[low] = arr[high]; arr[high] = t; }
    if (CMP(&arr[mid], &arr[high]) > 0) { Student t = arr[mid]; arr[mid] = arr[high]; arr[high] = t; }
    Student pivot = arr[mid];
    int i = low - 1, j = high + 1;
    while (1) {
        do { i++; } while (CMP(&arr[i], &pivot) < 0);
        do { j--; } while (CMP(&arr[j], &pivot) > 0);
        if (i >= j) return j;
        Student t = arr[i]; arr[i] = arr[j]; arr[j] = t;
    }
}
void sort_quick_med3_rec(Student* arr, int low, int high) {
    if (low < high) {
        int p = partition_med3(arr, low, high);
        sort_quick_med3_rec(arr, low, p);
        sort_quick_med3_rec(arr, p + 1, high);
    }
}
void sort_quick_med3(Student* arr, int n) {
    if (n > 0) sort_quick_med3_rec(arr, 0, n - 1);
}

/* ---------- 6. Heap Sort ---------- */
void heapify(Student* arr, int n, int i) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < n && CMP(&arr[l], &arr[largest]) > 0) largest = l;
    if (r < n && CMP(&arr[r], &arr[largest]) > 0) largest = r;
    if (largest != i) {
        Student t = arr[i]; arr[i] = arr[largest]; arr[largest] = t;
        heapify(arr, n, largest);
    }
}
void sort_heap(Student* arr, int n) {
    for (int i = n / 2 - 1; i >= 0; i--) heapify(arr, n, i);
    for (int i = n - 1; i > 0; i--) {
        Student t = arr[0]; arr[0] = arr[i]; arr[i] = t;
        heapify(arr, i, 0);
    }
}

/* ---------- 7. Merge Sort (stable) ---------- */
void merge_merge(Student* arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    Student* L = malloc(sizeof(Student) * n1);
    Student* R = malloc(sizeof(Student) * n2);
    if (L) MEM_COUNT += sizeof(Student) * n1;
    if (R) MEM_COUNT += sizeof(Student) * n2;
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (CMP(&L[i], &R[j]) <= 0) arr[k++] = L[i++]; else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    if (L) free(L);
    if (R) free(R);
}
void sort_merge_rec(Student* arr, int l, int r) {
    if (l >= r) return;
    int m = (l + r) / 2;
    sort_merge_rec(arr, l, m);
    sort_merge_rec(arr, m + 1, r);
    merge_merge(arr, l, m, r);
}
void sort_merge(Student* arr, int n) {
    if (n > 0) sort_merge_rec(arr, 0, n - 1);
}

/* ---------- 8. Radix Sort (정수 키: ID or SUM) (stable) ---------- */
/* LSD radix with base 256 (4 bytes) */
int key_id_or_sum(const Student* s) {
    if (CUR_KEY == KEY_ID) return s->id >= 0 ? s->id : 0;
    else { int sum = s->kor + s->eng + s->math; return sum >= 0 ? sum : 0; }
}
void sort_radix_int(Student* arr, int n) {
    if (n <= 0) return;
    Student* buf = malloc(sizeof(Student) * n);
    if (!buf) return;
    MEM_COUNT += sizeof(Student) * n;
    #define B 256
    size_t count[B];
    Student* from = arr;
    Student* to = buf;
    /* 4 bytes */
    for (int byte = 0; byte < 4; ++byte) {
        memset(count, 0, sizeof(count));
        for (int i = 0; i < n; i++) {
            unsigned int key = (unsigned int)key_id_or_sum(&from[i]);
            unsigned int b = (key >> (8 * byte)) & 0xFF;
            count[b]++;
        }
        size_t pos[B];
        pos[0] = 0;
        for (int i = 1; i < B; i++) pos[i] = pos[i - 1] + count[i - 1];
        for (int i = 0; i < n; i++) {
            unsigned int key = (unsigned int)key_id_or_sum(&from[i]);
            unsigned int b = (key >> (8 * byte)) & 0xFF;
            to[pos[b]++] = from[i];
        }
        /* swap */
        Student* tmp = from; from = to; to = tmp;
    }
    /* 최종 결과가 arr에 없다면 복사 */
    if (from != arr) memcpy(arr, from, sizeof(Student) * n);
    free(buf);
}

/* ---------- 9. Tree Sort (AVL) ---------- */
typedef struct AVLNode {
    Student val;
    struct AVLNode* left, * right;
    int height;
} AVLNode;

AVLNode* avl_newnode(const Student* s) {
    AVLNode* n = malloc(sizeof(AVLNode));
    if (n) { n->val = *s; n->left = n->right = NULL; n->height = 1; MEM_COUNT += sizeof(AVLNode); }
    return n;
}

int avl_height(AVLNode* n) { return n ? n->height : 0; }

int avl_getbalance(AVLNode* n) { return n ? avl_height(n->left) - avl_height(n->right) : 0; }
AVLNode* avl_rightrotate(AVLNode* y) {
    AVLNode* x = y->left; AVLNode* T2 = x->right;
    x->right = y; y->left = T2;
    y->height = 1 + (avl_height(y->left) > avl_height(y->right) ? avl_height(y->left) : avl_height(y->right));
    x->height = 1 + (avl_height(x->left) > avl_height(x->right) ? avl_height(x->left) : avl_height(x->right));
    return x;
}
AVLNode* avl_leftrotate(AVLNode* x) {
    AVLNode* y = x->right; AVLNode* T2 = y->left;
    y->left = x; x->right = T2;
    x->height = 1 + (avl_height(x->left) > avl_height(x->right) ? avl_height(x->left) : avl_height(x->right));
    y->height = 1 + (avl_height(y->left) > avl_height(y->right) ? avl_height(y->left) : avl_height(y->right));
    return y;
}
AVLNode* avl_insert_node(AVLNode* node, const Student* s) {
    if (!node) return avl_newnode(s);
    if (CMP(s, &node->val) < 0) node->left = avl_insert_node(node->left, s);
    else node->right = avl_insert_node(node->right, s);
    node->height = 1 + (avl_height(node->left) > avl_height(node->right) ? avl_height(node->left) : avl_height(node->right));
    int balance = avl_getbalance(node);
    if (balance > 1 && CMP(s, &node->left->val) < 0) return avl_rightrotate(node);
    if (balance < -1 && CMP(s, &node->right->val) > 0) return avl_leftrotate(node);
    if (balance > 1 && CMP(s, &node->left->val) > 0) { node->left = avl_leftrotate(node->left); return avl_rightrotate(node); }
    if (balance < -1 && CMP(s, &node->right->val) < 0) { node->right = avl_rightrotate(node->right); return avl_leftrotate(node); }
    return node;
}
void avl_inorder_tr(AVLNode* root, Student* out, int* idx) {
    if (!root) return;
    avl_inorder_tr(root->left, out, idx);
    out[(*idx)++] = root->val;
    avl_inorder_tr(root->right, out, idx);
}
void avl_free(AVLNode* root) {
    if (!root) return;
    avl_free(root->left); avl_free(root->right); free(root);
}
void sort_tree_avl(Student* arr, int n) {
    AVLNode* root = NULL;
    for (int i = 0; i < n; i++) root = avl_insert_node(root, &arr[i]);
    int idx = 0; avl_inorder_tr(root, arr, &idx);
    avl_free(root);
}

/* ================== 벤치마크 실행기 ================== */
/* 각 알고리즘을 문자열로 식별 */
typedef enum {
    ALG_BUBBLE, ALG_SELECTION, ALG_INSERTION,
    ALG_SHELL_BASIC, ALG_SHELL_CIURA,
    ALG_QUICK_BASIC, ALG_QUICK_MED3,
    ALG_HEAP, ALG_MERGE, ALG_RADIX,
    ALG_TREE_AVL,
    ALG_COUNT
} Alg;

const char* ALG_NAME[ALG_COUNT] = {
    "Bubble", "Selection", "Insertion",
    "Shell(n/2)", "Shell(Ciura)",
    "Quick(basic)","Quick(med3)",
    "Heap", "Merge", "Radix(ID/SUM)",
    "Tree(AVL)"
};

int is_stable_alg(Alg alg) {
    return (alg == ALG_BUBBLE || alg == ALG_INSERTION || alg == ALG_MERGE || alg == ALG_RADIX);
}

int alg_allowed_for_key(Alg alg, KeyType key, int has_dup) {
    if (key == KEY_GENDER && !is_stable_alg(alg)) return 0;
    if (alg == ALG_RADIX && !(key == KEY_ID || key == KEY_SUM)) return 0;
    if (has_dup && (alg == ALG_HEAP || alg == ALG_TREE_AVL)) return 0;
    return 1;
}

/* 실제 알고리즘 실행자 (위에 선언한 함수들을 실제로 호출) */
void run_alg(Alg alg, Student* arr, int n);

/* 벤치마크: 건너뛰는 이유도 출력하도록 개선 */
void bench_key(Student* base, int n, KeyType key) {
    CUR_KEY = key;
    const char* keyname = (key == KEY_ID) ? "ID" : (key == KEY_NAME) ? "NAME" : (key == KEY_GENDER) ? "GENDER" : "SUM";

    CUR_ASC = 1;
    int has_dup = has_duplicates_key(base, n);

    printf("\n=== 정렬 기준: %s  (duplicates=%s) ===\n", keyname, has_dup ? "YES" : "NO");

    for (int asc = 1; asc >= 0; asc--) {
        CUR_ASC = asc;
        printf("\n-- Order: %s --\n", asc ? "오름차순" : "내림차순");

        for (int alg = 0; alg < ALG_COUNT; alg++) {
            /* 판단 용이성: 각 알고리즘이 허용되는지 검사 */
            int allowed = alg_allowed_for_key((Alg)alg, key, has_dup);
            if (!allowed) {
                /* 건너뛰는 이유 출력 */
                if (key == KEY_GENDER && !is_stable_alg((Alg)alg))
                    printf("%-14s : SKIPPED (requires stable for GENDER)\n", ALG_NAME[alg]);
                else if (alg == ALG_RADIX && !(key == KEY_ID || key == KEY_SUM))
                    printf("%-14s : SKIPPED (Radix not for NAME/GENDER)\n", ALG_NAME[alg]);
                else if (has_dup && (alg == ALG_HEAP || alg == ALG_TREE_AVL))
                    printf("%-14s : SKIPPED (duplicates exist -> skip Heap/Tree)\n", ALG_NAME[alg]);
                else
                    printf("%-14s : SKIPPED (not allowed)\n", ALG_NAME[alg]);
                continue;
            }

            long long total_comp = 0;
            size_t total_mem = 0;

            for (int t = 0; t < TESTS; t++) {
                Student* work = malloc(sizeof(Student) * n);
                if (!work) { fprintf(stderr, "메모리 할당 실패\n"); exit(1); }
                memcpy(work, base, sizeof(Student) * n);
                COMP_COUNT = 0;
                MEM_COUNT = 0;

                /* 실행 */
                run_alg((Alg)alg, work, n);

                total_comp += COMP_COUNT;
                total_mem += MEM_COUNT;

                free(work);
            }

            printf("%-14s : 평균 비교횟수 = %12lld , 평균 메모리 사용량 = %8zu bytes\n",
                ALG_NAME[alg], total_comp / TESTS, total_mem / TESTS);
        }
    }
}

/* ================= 메인 ================= */
int main(void) {
    int n;
    Student* arr = load_students(CSV_PATH, &n);
    if (!arr) { fprintf(stderr, "데이터 로드 실패\n"); return 1; }
    if (n <= 0) { fprintf(stderr, "데이터 없음\n"); free(arr); return 1; }

    printf("Loaded %d records from %s\n", n, CSV_PATH);
    printf("TESTS = %d (각 정렬 당 반복 횟수)\n", TESTS);
            
    bench_key(arr, n, KEY_ID);
    bench_key(arr, n, KEY_NAME);
    bench_key(arr, n, KEY_GENDER);
    bench_key(arr, n, KEY_SUM);

    free(arr);
    return 0;
}


void run_alg(Alg alg, Student* arr, int n) {
    switch (alg) {
    case ALG_BUBBLE: sort_bubble(arr, n); break;
    case ALG_SELECTION: sort_selection(arr, n); break;
    case ALG_INSERTION: sort_insertion(arr, n); break;
    case ALG_SHELL_BASIC: sort_shell_basic(arr, n); break;
    case ALG_SHELL_CIURA: sort_shell_ciura(arr, n); break;
    case ALG_QUICK_BASIC: sort_quick_basic(arr, n); break;
    case ALG_QUICK_MED3: sort_quick_med3(arr, n); break;
    case ALG_HEAP: sort_heap(arr, n); break;
    case ALG_MERGE: sort_merge(arr, n); break;
    case ALG_RADIX: sort_radix_int(arr, n); break;
    case ALG_TREE_AVL: sort_tree_avl(arr, n); break;
    default: break;
    }
}