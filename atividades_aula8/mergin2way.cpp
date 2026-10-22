#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void mergin2way(int a[], int n, int b[], int m, int c[]) {
    int i = 0, j = 0, k = 0;
    while (i < n && j < m) {
        if (a[i] <= b[j])
            c[k++] = a[i++];
        else
            c[k++] = b[j++];
    }
    while (i < n)
        c[k++] = a[i++];
    while (j < m)
        c[k++] = b[j++];
}
int main() {
    int a[] = {10, 25, 30, 45, 60};
    int b[] = {5, 20, 30, 40, 50, 70};
    int c[11];
    mergin2way(a, 5, b, 6, c);
    for (int i = 0; i < 11; i++)
        printf("%d ", c[i]);
    return 0;
}
