#include <stdio.h>
#include <stdlib.h>

#define SIZE 100

// ����ü
typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
} Node;

// ���� Ž�� Ʈ�� ����
Node* createNode(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->left = newNode->right = NULL;
    return newNode;
}

Node* insert(Node* root, int data) {
    if (root == NULL) return createNode(data);
    if (data < root->data)
        root->left = insert(root->left, data);
    else
        root->right = insert(root->right, data);
    return root;
}

// Ž�� (Ž�� Ƚ�� ����)
int searchBST(Node* root, int key, int* count) {
    while (root != NULL) {
        (*count)++;
        if (key == root->data) return 1;
        else if (key < root->data)
            root = root->left;
        else
            root = root->right;
    }
    return 0;
}

// Ʈ�� ����
void freeTree(Node* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

// ���� Ž�� 
    int linearSearch(int arr[], int n, int key, int* count) {
    for (int i = 0; i < n; i++) {
        (*count)++;
        if (arr[i] == key) return 1;
    }
    return 0;
}

int main() {
    int arr[SIZE];
    Node* root = NULL;

    srand((unsigned int)time(NULL));

   // ������ ������ ����
    printf("���� ������ 100�� ����:\n");
    for (int i = 0; i < SIZE; i++) {
        arr[i] = rand() % 1001;  // 0~1000
        root = insert(root, arr[i]);
        printf("%d ", arr[i]);
    }
    printf("\n\n");

    // �˻� ��� ���� ����
    int key = arr[rand() % SIZE];
    printf("�˻��� ����: %d\n\n", key);

    // ���� Ž��
    int countLinear = 0;
    int foundLinear = linearSearch(arr, SIZE, key, &countLinear);;

    // Ʈ�� Ž��
    int countBST = 0;
    int foundBST = searchBST(root, key, &countBST);

    // ��� ���
    printf("===== Ž�� ��� �� =====\n");
    printf("[���� Ž��]\n");
    printf("���: %s\n", foundLinear ? "Ž�� ����" : "Ž�� ����");
    printf("Ž�� Ƚ��: %d\n\n", countLinear);

    printf("[���� Ž�� Ʈ��]\n");
    printf("���: %s\n", foundBST ? "Ž�� ����" : "Ž�� ����");
    printf("Ž�� Ƚ��: %d\n", countBST);

    // 6. Ʈ�� ����
    freeTree(root);
    return 0;
}
