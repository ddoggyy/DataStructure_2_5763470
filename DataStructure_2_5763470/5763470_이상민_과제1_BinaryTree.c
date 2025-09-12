#include <stdio.h>
#include <ctype.h>

const char* expr;

// ����� �� ����
void skipSpace() {
    while (*expr == ' ' || *expr == '\t') expr++;
}

int parseNode();

int parseChildren(int* childCount) {
    int violated = 0; // �ڽ� �� �ʰ� �Ǵ� �������� FALSE �߻��ϸ� 1��
    while (1) {
        skipSpace();

        if (*expr == ')') {
            // �ݴ� ��ȣ�� �Һ����� �ʰ� �θ� ó���ϵ��� ����
            break;
        }
        if (*expr == '\0') return -1; // �߰� ����� ERROR

        int r = parseNode();
        if (r == -1) return -1; // ������ ������ ��� ���� ��ȯ
        if (r == 0) {
            // �������� ���� ������ ����� �صε� ��� �Ľ��ؼ� ���� ��ȣ �Һ�
            violated = 1;
            continue;
        }
        if (r == 1) {
            (*childCount)++;
            if (*childCount > 2) {
                violated = 1; // ����Ʈ�� ���������� ��� �Ľ��Ͽ� ��   ��
            }
        }
        // r == 2 (�� ���)�� childCount�� �������� ����
    }

    return violated ? 0 : 1;
}

int parseNode() {
    skipSpace();

    if (*expr == '(') {
        expr++; // '(' �Һ�
        skipSpace();

        if (*expr == ')') { // �� ��ȣ 
            expr++; // ')' �Һ�
            return 2; // �� ��� ǥ��
        }

        if (!isalpha((unsigned char)*expr)) return -1; // ERROR: ���̺� ����
        expr++; // ���̺� �Һ�

        int childCount = 0;
        int r = parseChildren(&childCount);
        if (r != 1) {
            skipSpace();
            if (*expr != ')') return r == -1 ? -1 : r; // ���������� �ݴ� ��ȣ�� ������ ERROR
            expr++; // �θ��� ')' �Һ�
            return r; // r == 0 or -1(=error)
        }

        skipSpace();
        if (*expr != ')') return -1; // �ݴ� ��ȣ ���� �� ERROR
        expr++; // ')' �Һ�
        return 1;
    }
    else if (isalpha((unsigned char)*expr)) {
        // ��ȣ ���� ���̺� ���: Label �Ǵ� Label(children)
        expr++; // ���̺� �Һ�
        skipSpace();

        if (*expr == '(') {
            // Label(...) ����: ���̺� ���� ��ȣ�� �� ����� �ڽ���
            expr++; // '(' �Һ�
            int childCount = 0;
            int r = parseChildren(&childCount);
            if (r != 1) { 
                skipSpace();
                if (*expr != ')') return r == -1 ? -1 : r;
                expr++; // ')' �Һ�
                return r;
            }
            skipSpace();
            if (*expr != ')') return -1;
            expr++; // ')' �Һ�
        }
        // ���̺� ������ ����(����)
        return 1;
    }
    else {
        return -1; // ���� ����
    }
}

int main() {
    char input[1000];
    if (!fgets(input, sizeof(input), stdin)) {
        printf("ERROR\n");
        return 0;
    }

    // ���� ����
    for (int i = 0; input[i]; i++)
        if (input[i] == '\n') input[i] = '\0';

    expr = input;
    int result = parseNode();

    skipSpace();
    if (*expr != '\0') result = -1; // ���� ���� �� ERROR

    // ���� 2 (�� ��Ʈ)�� TRUE�� ó��
    if (result == 2) result = 1;

    if (result == 1) printf("TRUE\n");
    else if (result == 0) printf("FALSE\n");
    else printf("ERROR\n");

    return 0;
}
