#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// 연결 자료형
typedef struct Node {
    char data;
    struct Node* left;
    struct Node* right;
} Node;

char buf[10000];
char* expr;            // 전역 파서 포인터
int totalNodes = 0;
char arr[10000];
int maxArrIndex = 0;

Node* createNode(char c) {
    Node* n = (Node*)malloc(sizeof(Node));
    if (!n) { perror("malloc"); exit(1); }
    n->data = c; n->left = n->right = NULL;
    totalNodes++;
    return n;
}

void skipSpaces() {
    while (*expr && isspace((unsigned char)*expr)) expr++;
}

int is_simple_label_group(char* p) { // (C D) 같은 단순 라벨 검사
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
        expr++; // '(' 소비
        skipSpaces();

        // 빈 괄호 '()'
        if (*expr == ')') { expr++; return NULL; }

        if (!isalpha((unsigned char)*expr)) {
            while (*expr && *expr != ')') expr++;
            if (*expr == ')') expr++;
            return NULL;
        }

        // 정상 노드
        char label = *expr++;
        Node* node = createNode(label);
        if (index > 0 && index < (int)(sizeof(arr) / sizeof(arr[0]))) {
            arr[index] = label;
            if (index > maxArrIndex) maxArrIndex = index;
        }

        // 자식 노드
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
                        Node* child = createNode(lab);
                        int childIndex = 0;
                        if (index > 0) {
                            childIndex = (index * 2) + childCount;
                            if (childIndex < (int)(sizeof(arr) / sizeof(arr[0]))) {
                                arr[childIndex] = lab;
                                if (childIndex > maxArrIndex) maxArrIndex = childIndex;
                            }
                            else childIndex = 0;
                        }
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

            // 단순 라벨이 아니면 parseItem 
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
                        Node* child = createNode(lab);
                        int childIndex = 0;
                        if (index > 0) {
                            childIndex = (index * 2) + childCount;
                            if (childIndex < (int)(sizeof(arr) / sizeof(arr[0]))) {
                                arr[childIndex] = lab;
                                if (childIndex > maxArrIndex) maxArrIndex = childIndex;
                            }
                            else childIndex = 0;
                        }
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

// 높이, 단말 계산
int treeHeight(Node* r) {
    if (!r) return -1;
    int lh = treeHeight(r->left);
    int rh = treeHeight(r->right);
    return (lh > rh ? lh : rh) + 1;
}

int countLeaves(Node* r) {
    if (!r) return 0;
    if (!r->left && !r->right) return 1;
    return countLeaves(r->left) + countLeaves(r->right);
}

void freeTree(Node* r) {
    if (!r) return;
    freeTree(r->left);
    freeTree(r->right);
    free(r);
}

// --- 연결 자료형 트리 출력 확인용 ---
void printLinkedTree(Node* root, int level) {
    if (!root) return;
    for (int i = 0; i < level; i++) printf("  "); // 들여쓰기
    printf("%c\n", root->data);
    printLinkedTree(root->left, level + 1);
    printLinkedTree(root->right, level + 1);
}

// --- 배열 트리 출력 확인용 ---
void printArrayTree(char arr[], int maxIndex) {
    printf("배열 트리\n");
    for (int i = 1; i <= maxIndex; i++) {
        if (arr[i]) {
            printf("[%d]=%c ", i, arr[i]);
        }
    }
    printf("\n");
}

int main(void) {
    if (scanf(" %9999[^\n]", buf) != 1) return 0;

    maxArrIndex = 0;
    totalNodes = 0;
    expr = buf;

    Node* root = parseItem(1);

    int height = treeHeight(root);
    int nodeCount = totalNodes;
    int leafCount = countLeaves(root);

    printf("%d, %d, %d\n", height, nodeCount, leafCount);
    // printLinkedTree(root, 0); 
    // printArrayTree(arr, maxArrIndex); 
    // 트리 확인용 함수

    freeTree(root);
    return 0;
}
