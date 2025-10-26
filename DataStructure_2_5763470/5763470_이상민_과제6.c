#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NODES 100
#define E_SPARSE 100
#define E_DENSE 4000
#define OPS 1000    // �� ���� ���� �ݺ� Ƚ��

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

// ---------- ������� ���� ----------
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

// ---------- ��������Ʈ ���� ----------
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

// ---------- �޸� ���� ----------
size_t mem_adj_matrix_bytes() {
    return sizeof(unsigned char) * NODES * NODES;
}
size_t mem_adj_list_bytes(int nodesAllocated) {
    size_t ptrArray = sizeof(EdgeNode*) * NODES;
    size_t nodesBytes = (size_t)nodesAllocated * sizeof(EdgeNode);
    return ptrArray + nodesBytes;
}

// ---------- ��� ���� (�� ī��Ʈ) ----------
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

// ---------- ����Ʈ ���� (�� ī��Ʈ) ----------
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

// ---------- ����Ʈ ���� ----------
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

// ---------- ���̽� ���� (��� ���� �Ǵ� ����Ʈ ���� ��� ����) ----------
void run_case_matrix(const char* title, int edgesCount, const int edges[][2]) {
    unsigned char mat[NODES][NODES];
    build_adj_matrix(edges, edgesCount, mat);

    ll mat_insdel_cmp = 0, mat_conn_cmp = 0, mat_print_cmp = 0;

    // OPS�� ����/����
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

    // OPS�� ����Ȯ��
    for (int i = 0; i < OPS; ++i) {
        int u = rand() % NODES, v = rand() % NODES;
        if (u == v) { i--; continue; }
        matrix_connected(u, v, mat, &mat_conn_cmp);
    }

    // OPS�� ������� ���(�񱳸�)
    for (int i = 0; i < OPS; ++i) {
        int u = rand() % NODES;
        matrix_print_adj(u, mat, &mat_print_cmp);
    }

    // ��� 
    printf("���̽�: %s\n", title);
    printf("�޸� %zu Bytes\n", mem_adj_matrix_bytes());
    printf("���� ����/���� �� %lld��\n", mat_insdel_cmp);
    printf("�� ������ ���� Ȯ�� �� %lld��\n", mat_conn_cmp);
    printf("�� ����� ���� ��� ��� �� %lld��\n\n", mat_print_cmp);
}

void run_case_list(const char* title, int edgesCount, const int edges[][2]) {
    EdgeNode* list[NODES];
    int nodesAllocated = 0;
    build_adj_list(edges, edgesCount, list, &nodesAllocated);

    ll list_insdel_cmp = 0, list_conn_cmp = 0, list_print_cmp = 0;

    // OPS�� ����/����
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

    // OPS�� ����Ȯ��
    for (int i = 0; i < OPS; ++i) {
        int u = rand() % NODES, v = rand() % NODES;
        if (u == v) { i--; continue; }
        list_connected_one_side(u, v, list, &list_conn_cmp);
    }

    // OPS�� ������� ���(�񱳸�)
    for (int i = 0; i < OPS; ++i) {
        int u = rand() % NODES;
        list_print_adj_one_side(u, list, &list_print_cmp);
    }

    // ��� (�ѱ�)
    printf("���̽�: %s\n", title);
    printf("�޸� %zu Bytes\n", mem_adj_list_bytes(nodesAllocated));
    printf("���� ����/���� �� %lld��\n", list_insdel_cmp);
    printf("�� ������ ���� Ȯ�� �� %lld��\n", list_conn_cmp);
    printf("�� ����� ���� ��� ��� �� %lld��\n\n", list_print_cmp);

    free_adj_list(list);
}

int main(void) {
    srand((unsigned int)time(NULL));

    int edges_sparse[E_SPARSE][2];
    int edges_dense[E_DENSE][2];

    gen_random_edge_set(E_SPARSE, edges_sparse);
    gen_random_edge_set(E_DENSE, edges_dense);

    // ���̽� 1: ��� - �������
    run_case_matrix("��� �׷��� - �������", E_SPARSE, edges_sparse);

    // ���̽� 2: ��� - ��������Ʈ
    run_case_list("��� �׷��� - ��������Ʈ", E_SPARSE, edges_sparse);

    // ���̽� 3: ���� - �������
    run_case_matrix("���� �׷��� - �������", E_DENSE, edges_dense);

    // ���̽� 4: ���� - ��������Ʈ
    run_case_list("���� �׷��� - ��������Ʈ", E_DENSE, edges_dense);

    return 0;
}
