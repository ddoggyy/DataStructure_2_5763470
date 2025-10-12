#include <stdio.h>
#include <stdlib.h>

#define SIZE 100

// 구조체
typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
} Node;

// 이진 탐색 트리 생성
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

// 탐색 (탐색 횟수 세기)
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

// 트리 해제
void freeTree(Node* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

// 선형 탐색 
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

   // 무작위 데이터 생성
    printf("랜덤 데이터 100개 생성:\n");
    for (int i = 0; i < SIZE; i++) {
        arr[i] = rand() % 1001;  // 0~1000
        root = insert(root, arr[i]);
        printf("%d ", arr[i]);
    }
    printf("\n\n");

    // 검색 대상 랜덤 선택
    int key = arr[rand() % SIZE];
    printf("검색할 숫자: %d\n\n", key);

    // 선형 탐색
    int countLinear = 0;
    int foundLinear = linearSearch(arr, SIZE, key, &countLinear);;

    // 트리 탐색
    int countBST = 0;
    int foundBST = searchBST(root, key, &countBST);

    // 결과 출력
    printf("===== 탐색 결과 비교 =====\n");
    printf("[선형 탐색]\n");
    printf("결과: %s\n", foundLinear ? "탐색 성공" : "탐색 실패");
    printf("탐색 횟수: %d\n\n", countLinear);

    printf("[이진 탐색 트리]\n");
    printf("결과: %s\n", foundBST ? "탐색 성공" : "탐색 실패");
    printf("탐색 횟수: %d\n", countBST);

    // 6. 트리 해제
    freeTree(root);
    return 0;
}
