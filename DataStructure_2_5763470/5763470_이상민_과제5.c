#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000
#define MAX 10000 

int maxValue(int a, int b) { return a > b ? a : b; }

typedef struct Node {
    int key;
    int height;
    struct Node* left;
    struct Node* right;
} Node;

int heightNode(Node* n) { return n ? n->height : 0; }

Node* createNode(int key) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->key = key;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}

Node* insertBST(Node* root, int key) {
    if (root == NULL) return createNode(key);
    if (key < root->key)
        root->left = insertBST(root->left, key);
    else
        root->right = insertBST(root->right, key);
    return root;
}

int getBalance(Node* n) { return n ? heightNode(n->left) - heightNode(n->right) : 0; }

Node* rotateRight(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = maxValue(heightNode(y->left), heightNode(y->right)) + 1;
    x->height = maxValue(heightNode(x->left), heightNode(x->right)) + 1;
    return x;
}

Node* rotateLeft(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = maxValue(heightNode(x->left), heightNode(x->right)) + 1;
    y->height = maxValue(heightNode(y->left), heightNode(y->right)) + 1;
    return y;
}

Node* insertAVL(Node* node, int key) {
    if (node == NULL) return createNode(key);

    if (key < node->key)
        node->left = insertAVL(node->left, key);
    else if (key > node->key)
        node->right = insertAVL(node->right, key);
    else
        return node;

    node->height = 1 + maxValue(heightNode(node->left), heightNode(node->right));
    int balance = getBalance(node);

    if (balance > 1 && key < node->left->key) return rotateRight(node);
    if (balance < -1 && key > node->right->key) return rotateLeft(node);
    if (balance > 1 && key > node->left->key) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && key < node->right->key) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

int searchArray(int arr[], int key, int* count) {
    for (int i = 0; i < N; i++) {
        (*count)++;
        if (arr[i] == key) return 1;
    }
    return 0;
}

int searchBST(Node* root, int key, int* count) {
    while (root) {
        (*count)++;
        if (key == root->key) return 1;
        root = (key < root->key) ? root->left : root->right;
    }
    return 0;
}

void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

void makeRandomData(int arr[]) {
    int used[MAX + 1];
    for (int i = 0; i <= MAX; i++) used[i] = 0;
    for (int i = 0; i < N; i++) {
        int x;
        do { x = rand() % (MAX + 1); } while (used[x]);
        used[x] = 1;
        arr[i] = x;
    }
}

void makeSortedData(int arr[]) {
    for (int i = 0; i < N; i++) arr[i] = i;
}

void makeReverseData(int arr[]) {
    for (int i = 0; i < N; i++) arr[i] = N - 1 - i;
}

void makeFormulaData(int arr[]) {
    for (int i = 0; i < N; i++) arr[i] = i * (i % 2 + 2);
}

void testSearch(int arr[], Node* bst, Node* avl) {
    double avgArr = 0, avgBST = 0, avgAVL = 0;
    for (int i = 0; i < N; i++) {
        int key = rand() % (MAX + 1);
        int c1 = 0, c2 = 0, c3 = 0;
        searchArray(arr, key, &c1);
        searchBST(bst, key, &c2);
        searchBST(avl, key, &c3); 
        avgArr += c1;
        avgBST += c2;
        avgAVL += c3;
    }
    avgArr /= N;
    avgBST /= N;
    avgAVL /= N;
    printf("Array Æò±Õ Å½»ö È½¼ö: %.2f\n", avgArr);
    printf("BST Æò±Õ Å½»ö È½¼ö: %.2f\n", avgBST);
    printf("AVL Æò±Õ Å½»ö È½¼ö: %.2f\n\n", avgAVL);
}

int main() {
    srand((unsigned int)time(NULL));
    int arr[N];
    Node* bst = NULL, * avl = NULL;

    void (*generators[4])(int[]) = { makeRandomData, makeSortedData, makeReverseData, makeFormulaData };
    const char* labels[4] = { "(1) ·£´ý", "(2) Á¤·Ä", "(3) ¿ª¼ø", "(4) ¼ö½Ä" };

    for (int d = 0; d < 4; d++) {
        printf("===== µ¥ÀÌÅÍ %s =====\n", labels[d]);
        generators[d](arr);

        bst = NULL;
        avl = NULL;
        for (int i = 0; i < N; i++) {
            bst = insertBST(bst, arr[i]);
            avl = insertAVL(avl, arr[i]);
        }

        testSearch(arr, bst, avl);

        freeTree(bst);
        freeTree(avl);
    }

    return 0;
}
