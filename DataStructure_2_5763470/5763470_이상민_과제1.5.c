#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define ERROR 0
#define TRUE 1
#define FALSE 2

void trim(char* buf) {
	char* src = buf;   // 읽을 위치
	char* dst = buf;   // 쓸 위치

	while (*src != '\0') {
		if (!isspace((unsigned char)*src)) {
			*dst++ = *src;   // 공백이 아니면 복사
		}
		src++;
	}
	*dst = '\0';  // 문자열 끝 처리
}

int check_paren(char* buf, int size) {
    int balance = 0;
    for (int i = 0; i < size; i++) {
        if (buf[i] == '(') balance++;
        else if (buf[i] == ')') balance--;
        if (balance < 0) return ERROR; // 닫는 괄호가 더 많을 때
    }
    return (balance == 0) ? TRUE : ERROR;
}

// 2. 루트 노드가 하나인지 검사
int check_root(char* buf, int size) {
    // 맨 처음 '(' 로 시작하고 맨 끝이 ')' 가 아니면 에러
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
                // depth==1이면 바로 루트 괄호 내부
                rootCount++;
            }
        }
    }

    return (rootCount == 1) ? TRUE : ERROR;
}

// 3. 각 노드 자식이 최대 2개인지 검사
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