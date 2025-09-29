// array_bt_char0_insert_at_delete.c

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

typedef struct {

    char* a;       // 1-based: a[1..max_i], a[i]==0�̸� �� ���

    size_t cap;     // ���� �Ҵ� �뷮

    size_t max_i;   // ���� �ִ� �ε���(�߰��� ������� �� ����)

    size_t count;   // ���� ��ȿ ��� �� (a[i] != 0)

} ArrBT;

/* --- ��ƿ / �ε��� --- */

static void die(const char* m) { perror(m); exit(EXIT_FAILURE); }

static inline size_t left(size_t i) { return 2 * i; }

static inline size_t right(size_t i) { return 2 * i + 1; }

static inline size_t parent(size_t i) { return i / 2; }

static inline int is_upper(char c) { return c >= 'A' && c <= 'Z'; }

/* �ʿ� �뷮 ����: ���� �þ ������ 0���� ä�� */

static void ensure_cap(ArrBT* t, size_t need) {

    if (need < t->cap) return;                 // cap�� 0..cap-1 �ε��� ũ��

    size_t ncap = t->cap ? t->cap : 2;

    while (ncap <= need) ncap <<= 1;

    char* na = (char*)realloc(t->a, ncap * sizeof(char));

    if (!na) die("realloc");

    if (ncap > t->cap) memset(na + t->cap, 0, ncap - t->cap);  // Ȯ�� ���� 0���� �ʱ�ȭ

    t->a = na; t->cap = ncap;

}

/* -------- ����/���� -------- */

ArrBT* bt_create(size_t initial_cap) {

    if (initial_cap < 2) initial_cap = 2; // a[0] �̻��

    ArrBT* t = (ArrBT*)malloc(sizeof(ArrBT));

    if (!t) die("malloc ArrBT");

    t->a = (char*)malloc(initial_cap * sizeof(char));

    if (!t->a) die("malloc a");

    memset(t->a, 0, initial_cap);  // ���� �� ���(0)��

    t->cap = initial_cap;

    t->max_i = 0;

    t->count = 0;

    return t;

}

void bt_free(ArrBT* t) {

    if (!t) return;

    free(t->a);

    t->a = NULL;

    free(t);

}

/* -------- ���� API -------- */

/* i�� ������� ���� ch ����. ����: i, ����: 0

   - ch�� 'A'~'Z'�� ���

   - ���� üũ: i!=1�̸� parent(i)�� �����ؾ� ��

*/

size_t insert_node(ArrBT* t, size_t i, char ch) {

    if (!t || i == 0 || !is_upper(ch)) return 0;

    // �θ� ���缺 üũ(��Ʈ ����)

    if (i != 1) {

        size_t p = parent(i);

        ensure_cap(t, p);

        if (p  0 || t->a[p]  0) return 0;  // �θ� ���� �� ����

    }

    ensure_cap(t, i);

    if (t->a[i] != 0) return 0;  // �̹� ���� ���� �� ����

    t->a[i] = ch;

    t->count++;

    if (i > t->max_i) t->max_i = i;

    return i;

}

/* -------- ���� API (�ܸ���) -------- */

/* �ܸ�(leaf)�� ���� ����. ����: 1, ����: 0 */

int node_delete(ArrBT* t, size_t i) {

    if (!t || i == 0 || i > t->max_i) return 0;

    if (t->a[i] == 0) return 0;  // �� ĭ

    size_t li = left(i), ri = right(i);

    int has_left = (li <= t->max_i && t->a[li] != 0);

    int has_right = (ri <= t->max_i && t->a[ri] != 0);

    if (has_left || has_right) return 0;  // �ܸ��� �ƴ� �� ����

    // ����

    t->a[i] = 0;

    t->count--;

    // ���� ���� �� ĭ �����Ͽ� max_i ���

    if (i == t->max_i) {

        while (t->max_i > 0 && t->a[t->max_i] == 0) t->max_i--;

    }

    return 1;

}

/* -------- ��ȸ/���/��ȸ -------- */

size_t bt_count(const ArrBT* t) { return t->count; }

/* ����(���� ����): ���� -1, ���� 0 */

int height_idx(const ArrBT* t, size_t i) {

    if (i == 0 || i > t->max_i) return -1;

    if (t->a[i] == 0) return -1;

    int hl = height_idx(t, left(i));

    int hr = height_idx(t, right(i));

    return 1 + (hl > hr ? hl : hr);

}

/* ��ȸ: �� ���(0)�� �ǳʶ� */

void preorder_idx(const ArrBT* t, size_t i) {

    if (i == 0 || i > t->max_i || t->a[i] == 0) return;

    printf("%c ", t->a[i]);

    preorder_idx(t, left(i));

    preorder_idx(t, right(i));

}

void inorder_idx(const ArrBT* t, size_t i) {

    if (i == 0 || i > t->max_i || t->a[i] == 0) return;

    inorder_idx(t, left(i));

    printf("%c ", t->a[i]);

    inorder_idx(t, right(i));

}

void postorder_idx(const ArrBT* t, size_t i) {

    if (i == 0 || i > t->max_i || t->a[i] == 0) return;

    postorder_idx(t, left(i));

    postorder_idx(t, right(i));

    printf("%c ", t->a[i]);

}

/* ���� ��ȸ(���� ��常 ���) */

void levelorder_present(const ArrBT* t) {

    for (size_t i = 1; i <= t->max_i; i++) {

        if (t->a[i]) printf("%c ", t->a[i]);

    }

}

/* �����: �ε��� 1..max_i �� �״�� ǥ��(�� ĭ�� 0���� ǥ��) */

void dump_array(const ArrBT* t) {

    printf("[1..%zu]: ", t->max_i);

    for (size_t i = 1; i <= t->max_i; i++) {

        if (t->a[i] == 0) printf("0 ");

        else printf("%c ", t->a[i]);

    }

    puts("");

}

/* -------- ���� -------- */

int main(void) {

    ArrBT* t = bt_create(8);

    // Ʈ�� ����

    insert_node(t, 1, 'A');  // ��Ʈ

    insert_node(t, 2, 'B');  // A�� ����

    insert_node(t, 3, 'C');  // A�� ������

    insert_node(t, 4, 'D');  // B�� ����  (leaf)

    insert_node(t, 5, 'E');  // B�� ������(leaf)

    // 6�� ���

    insert_node(t, 7, 'G');  // C�� ������(leaf)

    puts("�ʱ� ����:");

    dump_array(t);

    // �ܸ� ���� �õ�

    if (node_delete(t, 4)) puts("i=4(D) ���� ����"); else puts("i=4 ���� ����");

    if (node_delete(t, 2)) puts("i=2(B) ���� ����"); else puts("i=2 ���� ����(�ܸ� �ƴ�)");

    puts("���� ��:");

    dump_array(t);

    printf("��� ��(count): %zu\n", bt_count(t));

    printf("����(���� ����): %d\n", height_idx(t, 1));

    bt_free(t);

    return 0;

}

