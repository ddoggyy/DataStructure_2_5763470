#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define ERROR 0
#define TRUE 1
#define FALSE 2

void trim(char* buf) {
	char* src = buf;   // ���� ��ġ
	char* dst = buf;   // �� ��ġ

	while (*src != '\0') {
		if (!isspace((unsigned char)*src)) {
			*dst++ = *src;   // ������ �ƴϸ� ����
		}
		src++;
	}
	*dst = '\0';  // ���ڿ� �� ó��
}

int check_paren(char* buf, int size) {
    int balance = 0;
    for (int i = 0; i < size; i++) {
        if (buf[i] == '(') balance++;
        else if (buf[i] == ')') balance--;
        if (balance < 0) return ERROR; // �ݴ� ��ȣ�� �� ���� ��
    }
    return (balance == 0) ? TRUE : ERROR;
}

// 2. ��Ʈ ��尡 �ϳ����� �˻�
int check_root(char* buf, int size) {
    // �� ó�� '(' �� �����ϰ� �� ���� ')' �� �ƴϸ� ����
    if (buf[0] != '(' || buf[size - 1] != ')') return ERROR;

    int depth = 0;
    int rootCount = 0;

    for (int i = 0; i < size; i++) {
        if (buf[i] == '(') {
            depth++;
        }
        else if (buf[i] == ')') {
            depth--;
        }
        else if (isalpha(buf[i])) {
            if (depth == 1) {  
                // depth==1�̸� �ٷ� ��Ʈ ��ȣ ����
                rootCount++;
            }
        }
    }

    return (rootCount == 1) ? TRUE : ERROR;
}

// 3. �� ��� �ڽ��� �ִ� 2������ �˻�
int check_binary_tree(char* buf) {
    int childCount = 0;
    for (int i = 0; buf[i] != '\0'; i++) {
        if (buf[i] == '(') {
            childCount = 0;
        }
        else if (isalpha(buf[i])) {
            childCount++;
            if (childCount > 2) return FALSE;
        }
        else if (buf[i] == ')') {
            childCount = 0;
        }
    }
    return TRUE;
}

int main() {
	char buf[1000];
	fgets(buf, sizeof(buf), stdin);

	trim(buf);

	if (check_paren(buf, strlen(buf)) == 0 || check_root(buf, strlen(buf)) == 0) {
		printf("ERROR");
		return 0;
	}

	int result = check_binary_tree(buf);

	if (result == TRUE) {
		printf("TRUE");
		return 0;
	}
	else {
		printf("FALSE");
		return 0;
	}
}