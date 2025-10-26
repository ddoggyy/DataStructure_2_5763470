#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NODES 100
#define E_SPARSE 100
#define E_DENSE 4000
#define OPS 1000    // 각 종류 연산 반복 횟수

typedef long long ll;

typedef struct EdgeNode {
    int v;
    struct EdgeNode* next;
} EdgeNode;

void gen_random_edge_set(int edgesCount, int edges[][2]) {
    int used[NODES][NODES];
    for (int i = 0; i < NODES; ++i)
        for (int j = 0; j < NODES; ++j)
            used[i][j] = 0;

    int added = 0;
    while (added < edgesCount) {
        int a = rand() % NODES;
        int b = rand() % NODES;
        if (a == b) continue;
        if (a > b) { int t = a; a = b; b = t; }
        if (used[a][b]) continue;
        used[a][b] = 1;
        edges[added][0] = a;
        edges[added][1] = b;
        added++;
    }
}

// ---------- 인접행렬 생성 ----------
void build_adj_matrix(const int edges[][2], int m, unsigned char mat[NODES][NODES]) {
    for (int i = 0; i < NODES; ++i)
        for (int j = 0; j < NODES; ++j)
            mat[i][j] = 0;
    for (int e = 0; e < m; ++e) {
        int a = edges[e][0], b = edges[e][1];
        mat[a][b] = 1;
        mat[b][a] = 1;
    }
}

// ---------- 인접리스트 생성 ----------
EdgeNode* make_node(int v) {
    EdgeNode* p = (EdgeNode*)malloc(sizeof(EdgeNode));
    p->v = v; p->next = NULL;
    return p;
}

void build_adj_list(const int edges[][2], int m, EdgeNode* list[], int* nodesAllocated) {
    for (int i = 0; i < NODES; ++i) list[i] = NULL;
    *nodesAllocated = 0;
    for (int e = 0; e < m; ++e) {
        int a = edges[e][0], b = edges[e][1];
        EdgeNode* na = make_node(b);
        na->next = list[a]; list[a] = na; (*nodesAllocated)++;
        EdgeNode* nb = make_node(a);
        nb->next = list[b]; list[b] = nb; (*nodesAllocated)++;
    }
}

// ---------- 메모리 추정 ----------
size_t mem_adj_matrix_bytes() {
    return sizeof(unsigned char) * NODES * NODES;
}
size_t mem_adj_list_bytes(int nodesAllocated) {
    size_t ptrArray = sizeof(EdgeNode*) * NODES;
    size_t nodesBytes = (size_t)nodesAllocated * sizeof(EdgeNode);
    return ptrArray + nodesBytes;
}

// ---------- 행렬 연산 (비교 카운트) ----------
void matrix_insert(int u, int v, unsigned char mat[NODES][NODES], ll* cmp) {
    (*cmp)++; 
    if (!mat[u][v]) {
        mat[u][v] = mat[v][u] = 1;
    }
}
void matrix_delete(int u, int v, unsigned char mat[NODES][NODES], ll* cmp) {
    (*cmp)++;
    if (mat[u][v]) {
        mat[u][v] = mat[v][u] = 0;
    }
}
int matrix_connected(int u, int v, unsigned char mat[NODES][NODES], ll* cmp) {
    (*cmp)++;
    return mat[u][v] ? 1 : 0;
}
void matrix_print_adj(int u, unsigned char mat[NODES][NODES], ll* cmp) {
    for (int i = 0; i < NODES; ++i) {
        (*cmp)++;
        if (mat[u][i]) { /* would print i */ (void)0; }
    }
}

// ---------- 리스트 연산 (비교 카운트) ----------
void list_insert_one_side(int u, int v, EdgeNode* list[], ll* cmp, int* nodesAllocated) {
    EdgeNode* p = list[u];
    while (p) {
        (*cmp)++;
        if (p->v == v) return; // exists
        p = p->next;
    }
    EdgeNode* node = make_node(v);
    node->next = list[u];
    list[u] = node;
    (*nodesAllocated)++;
}
void list_delete_one_side(int u, int v, EdgeNode* list[], ll* cmp, int* nodesAllocated) {
    EdgeNode* p = list[u];
    EdgeNode* prev = NULL;
    while (p) {
        (*cmp)++;
        if (p->v == v) {
            if (prev) prev->next = p->next;
            else list[u] = p->next;
            free(p);
            (*nodesAllocated)--;
            return;
        }
        prev = p; p = p->next;
    }
}
int list_connected_one_side(int u, int v, EdgeNode* list[], ll* cmp) {
    EdgeNode* p = list[u];
    while (p) {
        (*cmp)++;
        if (p->v == v) return 1;
        p = p->next;
    }
    return 0;
}
void list_print_adj_one_side(int u, EdgeNode* list[], ll* cmp) {
    EdgeNode* p = list[u];
    while (p) {
        (*cmp)++;
        /* would print p->v */
        p = p->next;
    }
}

// ---------- 리스트 해제 ----------
void free_adj_list(EdgeNode* list[]) {
    for (int i = 0; i < NODES; ++i) {
        EdgeNode* p = list[i];
        while (p) {
            EdgeNode* t = p->next;
            free(p);
            p = t;
        }
        list[i] = NULL;
    }
}

// ---------- 케이스 실행 (행렬 전용 또는 리스트 전용 출력 형식) ----------
void run_case_matrix(const char* title, int edgesCount, const int edges[][2]) {
    unsigned char mat[NODES][NODES];
    build_adj_matrix(edges, edgesCount, mat);

    ll mat_insdel_cmp = 0, mat_conn_cmp = 0, mat_print_cmp = 0;

    // OPS번 삽입/삭제
    for (int op = 0; op < OPS; ++op) {
        int u = rand() % NODES, v = rand() % NODES;
        if (u == v) { op--; continue; }
        int doInsert = rand() & 1;
        if (doInsert) {
            matrix_insert(u, v, mat, &mat_insdel_cmp);
            matrix_insert(v, u, mat, &mat_insdel_cmp);
        }
        else {
            matrix_delete(u, v, mat, &mat_insdel_cmp);
            matrix_delete(v, u, mat, &mat_insdel_cmp);
        }
    }

    // OPS번 연결확인
    for (int i = 0; i < OPS; ++i) {
        int u = rand() % NODES, v = rand() % NODES;
        if (u == v) { i--; continue; }
        matrix_connected(u, v, mat, &mat_conn_cmp);
    }

    // OPS번 인접노드 출력(비교만)
    for (int i = 0; i < OPS; ++i) {
        int u = rand() % NODES;
        matrix_print_adj(u, mat, &mat_print_cmp);
    }

    // 출력 
    printf("케이스: %s\n", title);
    printf("메모리 %zu Bytes\n", mem_adj_matrix_bytes());
    printf("간선 삽입/삭제 비교 %lld번\n", mat_insdel_cmp);
    printf("두 정점의 연결 확인 비교 %lld번\n", mat_conn_cmp);
    printf("한 노드의 인접 노드 출력 비교 %lld번\n\n", mat_print_cmp);
}

void run_case_list(const char* title, int edgesCount, const int edges[][2]) {
    EdgeNode* list[NODES];
    int nodesAllocated = 0;
    build_adj_list(edges, edgesCount, list, &nodesAllocated);

    ll list_insdel_cmp = 0, list_conn_cmp = 0, list_print_cmp = 0;

    // OPS번 삽입/삭제
    for (int op = 0; op < OPS; ++op) {
        int u = rand() % NODES, v = rand() % NODES;
        if (u == v) { op--; continue; }
        int doInsert = rand() & 1;
        if (doInsert) {
            list_insert_one_side(u, v, list, &list_insdel_cmp, &nodesAllocated);
            list_insert_one_side(v, u, list, &list_insdel_cmp, &nodesAllocated);
        }
        else {
            list_delete_one_side(u, v, list, &list_insdel_cmp, &nodesAllocated);
            list_delete_one_side(v, u, list, &list_insdel_cmp, &nodesAllocated);
        }
    }

    // OPS번 연결확인
    for (int i = 0; i < OPS; ++i) {
        int u = rand() % NODES, v = rand() % NODES;
        if (u == v) { i--; continue; }
        list_connected_one_side(u, v, list, &list_conn_cmp);
    }

    // OPS번 인접노드 출력(비교만)
    for (int i = 0; i < OPS; ++i) {
        int u = rand() % NODES;
        list_print_adj_one_side(u, list, &list_print_cmp);
    }

    // 출력 (한글)
    printf("케이스: %s\n", title);
    printf("메모리 %zu Bytes\n", mem_adj_list_bytes(nodesAllocated));
    printf("간선 삽입/삭제 비교 %lld번\n", list_insdel_cmp);
    printf("두 정점의 연결 확인 비교 %lld번\n", list_conn_cmp);
    printf("한 노드의 인접 노드 출력 비교 %lld번\n\n", list_print_cmp);

    free_adj_list(list);
}

int main(void) {
    srand((unsigned int)time(NULL));

    int edges_sparse[E_SPARSE][2];
    int edges_dense[E_DENSE][2];

    gen_random_edge_set(E_SPARSE, edges_sparse);
    gen_random_edge_set(E_DENSE, edges_dense);

    // 케이스 1: 희소 - 인접행렬
    run_case_matrix("희소 그래프 - 인접행렬", E_SPARSE, edges_sparse);

    // 케이스 2: 희소 - 인접리스트
    run_case_list("희소 그래프 - 인접리스트", E_SPARSE, edges_sparse);

    // 케이스 3: 밀집 - 인접행렬
    run_case_matrix("밀집 그래프 - 인접행렬", E_DENSE, edges_dense);

    // 케이스 4: 밀집 - 인접리스트
    run_case_list("밀집 그래프 - 인접리스트", E_DENSE, edges_dense);

    return 0;
}
