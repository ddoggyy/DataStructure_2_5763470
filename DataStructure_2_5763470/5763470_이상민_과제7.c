#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10    // 정점 수
#define E 20    // 간선 수 

void generate_random_graph(int adj[N][N]) {
    int count = 0;
    while (count < E) {
        int u = rand() % N;
        int v = rand() % N;
        if (u == v) continue;            // 자기루프 금지
        if (adj[u][v] == 0) {            // 중복 간선 금지
            adj[u][v] = adj[v][u] = 1;
            count++;
        }
    }
}

void print_edges(int adj[N][N]) {
    printf("생성된 간선 (무향, 정점 번호 1..%d):\n", N);
    for (int i = 0; i < N; ++i) {
        for (int j = i + 1; j < N; ++j) {
            if (adj[i][j]) {
                printf("  %2d - %2d\n", i + 1, j + 1);
            }
        }
    }
    printf("\n");
}

// BFS로 출발점 src에서 모든 정점까지의 최단거리와 부모 배열 채우기
void bfs(int adj[N][N], int src, int dist[N], int parent[N]) {
    for (int i = 0; i < N; ++i) {
        dist[i] = -1;
        parent[i] = -1;
    }
    int queue[N];
    int head = 0, tail = 0;
    queue[tail++] = src;
    dist[src] = 0;
    parent[src] = -1;

    while (head < tail) {
        int u = queue[head++];
        for (int v = 0; v < N; ++v) {
            if (adj[u][v] && dist[v] == -1) {
                dist[v] = dist[u] + 1;
                parent[v] = u;
                queue[tail++] = v;
            }
        }
    }
}

void print_path(int parent[N], int src, int dst) {
    if (dst == src) {
        printf("%d", src + 1);
        return;
    }
    int path[N];
    int idx = 0;
    int cur = dst;
    while (cur != -1) {
        path[idx++] = cur;
        if (cur == src) break;
        cur = parent[cur];
    }
    if (idx == 0 || path[idx - 1] != src) {
        // src로 거슬러 올라갈 수 없으면 경로 없음
        printf("경로 없음");
        return;
    }
    // 역순으로 저장되어 있으므로 뒤에서부터 출력
    for (int i = idx - 1; i >= 0; --i) {
        printf("%d", path[i] + 1);
        if (i > 0) printf(" -> ");
    }
}

int main(void) {
    int adj[N][N] = { 0 };
    srand((unsigned)time(NULL)); // 실행마다 다른 무작위 그래프 생성

    generate_random_graph(adj);

    print_edges(adj);

    int dist[N], parent[N];

    printf("모든 쌍 최단경로 (총 %d 쌍):\n\n", (N * (N - 1)) / 2);

    for (int i = 0; i < N; ++i) {
        bfs(adj, i, dist, parent); // i에서 시작하는 BFS
        for (int j = i + 1; j < N; ++j) {
            printf("정점 %2d <-> %2d : ", i + 1, j + 1);
            if (dist[j] == -1) {
                printf("연결되지 않음\n");
            }
            else {
                printf("거리 = %2d, 경로 = ", dist[j]);
                print_path(parent, i, j);
                printf("\n");
            }
        }
    }

    return 0;
}
