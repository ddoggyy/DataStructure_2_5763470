#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char data;
    struct Node* left;
    struct Node* right;
} Node;

char buf[10000];
char* expr;            // ���� �ļ� ������
int totalNodes = 0;
char arr[10000];      
int maxArrIndex = 0;

Node* createNode(char c) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = c; newNode->left = newNode->right = NULL;
    totalNodes++;
    return newNode;
}

void skipSpaces() {
    while (*expr && isspace((unsigned char)*expr)) expr++;
}

int is_simple_label_group(char* p) { // (C D) ���� �ܼ� �� �˻�
    if (!p || *p != '(') return 0;
    int depth = 0;
    int has_inner_paren = 0;
    for (char* q = p; *q; ++q) {
        if (*q == '(') {
            depth++;
            if (depth > 1) { has_inner_paren = 1; break; }
        }
        else if (*q == ')') {
            depth--;
            if (depth == 0) break;
        }
    }
    return (!has_inner_paren);
}

Node* parseItem(int index) {
    skipSpaces();
    if (!*expr) return NULL;

    if (*expr == '(') {
        expr++; // '(' �Һ�
        skipSpaces();

        // �� ��ȣ '()'
        if (*expr == ')') { expr++; return NULL; }

        if (!isalpha((unsigned char)*expr)) {
            while (*expr && *expr != ')') expr++;
            if (*expr == ')') expr++;
            return NULL;
        }

        // ���� ���
        char label = *expr++;
        Node* node = createNode(label);
        if (index > 0 && index < (int)(sizeof(arr) / sizeof(arr[0]))) {
            arr[index] = label;
            if (index > maxArrIndex) maxArrIndex = index;
        }

        // �ڽ� ���
        int childCount = 0;
        while (1) {
            skipSpaces();
            if (*expr == ')' || *expr == '\0') break;

            if (*expr == '(' && is_simple_label_group(expr)) {
                expr++; skipSpaces();
                while (*expr && *expr != ')') {
                    skipSpaces();
                    if (isalpha((unsigned char)*expr)) {
                        char lab = *expr++;
                        // �迭�� ����(����=2*index, ������=2*index+1)
                        int childIndex = 0;
                        if (index > 0) {
                            childIndex = (index * 2) + childCount;
                            if (childIndex < (int)(sizeof(arr) / sizeof(arr[0]))) {
                                arr[childIndex] = lab;
                                if (childIndex > maxArrIndex) maxArrIndex = childIndex;
                            }
                            else childIndex = 0;
                        }
                        // linked nodes still created (kept for compatibility)
                        Node* child = createNode(lab);
                        if (childCount == 0) node->left = child;
                        else if (childCount == 1) node->right = child;
                        childCount++;
                    }
                    else {
                        expr++;
                    }
                }
                if (*expr == ')') expr++;
                continue;
            }

            // �ܼ� ���� �ƴϸ� parseItem 
            Node* child = parseItem(index > 0 ? index * 2 + childCount : 0);
            childCount++;
            if (child) {
                if (childCount == 1) node->left = child;
                else if (childCount == 2) node->right = child;
            }
        }
        if (*expr == ')') expr++;
        return node;
    }

    if (isalpha((unsigned char)*expr)) {
        char label = *expr++;
        Node* node = createNode(label);
        if (index > 0 && index < (int)(sizeof(arr) / sizeof(arr[0]))) {
            arr[index] = label;
            if (index > maxArrIndex) maxArrIndex = index;
        }

        skipSpaces();
        if (*expr == '(') {
            if (is_simple_label_group(expr)) {
                expr++;
                int childCount = 0;
                while (*expr && *expr != ')') {
                    skipSpaces();
                    if (isalpha((unsigned char)*expr)) {
                        char lab = *expr++;
                        int childIndex = 0;
                        if (index > 0) {
                            childIndex = (index * 2) + childCount;
                            if (childIndex < (int)(sizeof(arr) / sizeof(arr[0]))) {
                                arr[childIndex] = lab;
                                if (childIndex > maxArrIndex) maxArrIndex = childIndex;
                            }
                            else childIndex = 0;
                        }
                        Node* child = createNode(lab);
                        if (childCount == 0) node->left = child;
                        else if (childCount == 1) node->right = child;
                        childCount++;
                    }
                    else expr++;
                }
                if (*expr == ')') expr++;
            }
            else {
                expr++;
                int childCount = 0;
                while (*expr && *expr != ')') {
                    skipSpaces();
                    Node* child = parseItem(index > 0 ? index * 2 + childCount : 0);
                    childCount++;
                    if (child) {
                        if (childCount == 1) node->left = child;
                        else if (childCount == 2) node->right = child;
                    }
                    skipSpaces();
                }
                if (*expr == ')') expr++;
            }
        }

        return node;
    }

    return NULL;
}

// �迭 ������� ��ȸ

// ���� ��ȸ (root, left, right) - �迭 �ε��� ���
void iterative_preorder_array(char out[], int* n) {
    int stack[10000], top = -1;
    if (arr[1]) stack[++top] = 1;
    while (top >= 0) {
        int idx = stack[top--];
        if (idx <= 0) continue;
        if (!arr[idx]) continue;
        out[(*n)++] = arr[idx];
        // push right then left
        int r = idx * 2 + 1;
        int l = idx * 2;
        if (r < (int)(sizeof(arr) / sizeof(arr[0])) && arr[r]) stack[++top] = r;
        if (l < (int)(sizeof(arr) / sizeof(arr[0])) && arr[l]) stack[++top] = l;
    }
}

// ���� ��ȸ (left, root, right)
void iterative_inorder_array(char out[], int* n) {
    int stack[10000], top = -1;
    int cur = 1;
    while ((cur < (int)(sizeof(arr) / sizeof(arr[0])) && arr[cur]) || top >= 0) {
        while (cur < (int)(sizeof(arr) / sizeof(arr[0])) && arr[cur]) {
            stack[++top] = cur;
            cur = cur * 2; // go left
        }
        if (top < 0) break;
        cur = stack[top--];
        out[(*n)++] = arr[cur];
        cur = cur * 2 + 1; // go right
    }
}

// ���� ��ȸ (left, right, root) - �� ���� ���
void iterative_postorder_array(char out[], int* n) {
    int s1[10000], s2[10000], t1 = -1, t2 = -1;
    if (arr[1]) s1[++t1] = 1;
    while (t1 >= 0) {
        int idx = s1[t1--];
        if (!arr[idx]) continue;
        s2[++t2] = idx;
        int l = idx * 2, r = idx * 2 + 1;
        if (l < (int)(sizeof(arr) / sizeof(arr[0])) && arr[l]) s1[++t1] = l;
        if (r < (int)(sizeof(arr) / sizeof(arr[0])) && arr[r]) s1[++t1] = r;
    }
    while (t2 >= 0) out[(*n)++] = arr[s2[t2--]];
}

// ����Ʈ
void print_seq(const char* title, char seq[], int n) {
    printf("%s", title);
    for (int i = 0; i < n; ++i) printf(" %c", seq[i]);
    printf("\n\n");
}

int main(void) {
    if (scanf(" %9999[^\n]", buf) != 1) return 0;

    // �ʱ�ȭ
    memset(arr, 0, sizeof(arr));
    maxArrIndex = 0;
    totalNodes = 0;
    expr = buf;

    // �Ľ�(���� �Լ��� arr[]���� ���� ä��)
    Node* root = parseItem(1);

    // �迭 ��� ��ȸ ����� ���� ����
    char pre[1000], in[1000], post[1000];
    int pn = 0, in_n = 0, postn = 0;

    iterative_preorder_array(pre, &pn);
    iterative_inorder_array(in, &in_n);
    iterative_postorder_array(post, &postn);

    print_seq("pre-order:", pre, pn);
    print_seq("in-order:", in, in_n);
    print_seq("post-order:", post, postn);

    return 0;
}
