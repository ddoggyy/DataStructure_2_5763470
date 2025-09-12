#include <stdio.h>
#include <ctype.h>

const char* expr;

// 공백과 탭 무시
void skipSpace() {
    while (*expr == ' ' || *expr == '\t') expr++;
}

int parseNode();

int parseChildren(int* childCount) {
    int violated = 0; // 자식 수 초과 또는 하위에서 FALSE 발생하면 1로
    while (1) {
        skipSpace();

        if (*expr == ')') {
            // 닫는 괄호는 소비하지 않고 부모가 처리하도록 놔둠
            break;
        }
        if (*expr == '\0') return -1; // 중간 끊기면 ERROR

        int r = parseNode();
        if (r == -1) return -1; // 문법적 문제면 즉시 에러 반환
        if (r == 0) {
            // 하위에서 구조 문제면 기록은 해두되 계속 파싱해서 남은 괄호 소비
            violated = 1;
            continue;
        }
        if (r == 1) {
            (*childCount)++;
            if (*childCount > 2) {
                violated = 1; // 이진트리 위반이지만 계속 파싱하여 소   비
            }
        }
        // r == 2 (빈 노드)은 childCount에 포함하지 않음
    }

    return violated ? 0 : 1;
}

int parseNode() {
    skipSpace();

    if (*expr == '(') {
        expr++; // '(' 소비
        skipSpace();

        if (*expr == ')') { // 빈 괄호 
            expr++; // ')' 소비
            return 2; // 빈 노드 표시
        }

        if (!isalpha((unsigned char)*expr)) return -1; // ERROR: 레이블 없음
        expr++; // 레이블 소비

        int childCount = 0;
        int r = parseChildren(&childCount);
        if (r != 1) {
            skipSpace();
            if (*expr != ')') return r == -1 ? -1 : r; // 문법적으로 닫는 괄호가 없으면 ERROR
            expr++; // 부모의 ')' 소비
            return r; // r == 0 or -1(=error)
        }

        skipSpace();
        if (*expr != ')') return -1; // 닫는 괄호 누락 → ERROR
        expr++; // ')' 소비
        return 1;
    }
    else if (isalpha((unsigned char)*expr)) {
        // 괄호 없는 레이블 허용: Label 또는 Label(children)
        expr++; // 레이블 소비
        skipSpace();

        if (*expr == '(') {
            // Label(...) 형태: 레이블 뒤의 괄호는 이 노드의 자식임
            expr++; // '(' 소비
            int childCount = 0;
            int r = parseChildren(&childCount);
            if (r != 1) { 
                skipSpace();
                if (*expr != ')') return r == -1 ? -1 : r;
                expr++; // ')' 소비
                return r;
            }
            skipSpace();
            if (*expr != ')') return -1;
            expr++; // ')' 소비
        }
        // 레이블만 있으면 리프(정상)
        return 1;
    }
    else {
        return -1; // 문법 오류
    }
}

int main() {
    char input[1000];
    if (!fgets(input, sizeof(input), stdin)) {
        printf("ERROR\n");
        return 0;
    }

    // 개행 제거
    for (int i = 0; input[i]; i++)
        if (input[i] == '\n') input[i] = '\0';

    expr = input;
    int result = parseNode();

    skipSpace();
    if (*expr != '\0') result = -1; // 남은 문자 → ERROR

    // 내부 2 (빈 루트)는 TRUE로 처리
    if (result == 2) result = 1;

    if (result == 1) printf("TRUE\n");
    else if (result == 0) printf("FALSE\n");
    else printf("ERROR\n");

    return 0;
}
