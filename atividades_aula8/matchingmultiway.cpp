#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
    int a[] = {10, 25, 30, 45, 60};
    int b[] = {10, 25, 30, 40, 50, 70};
    int c[] = {1, 8, 10, 18, 25};
    int i = 0, j = 0, k = 0;
    while (i < 5 && j < 6 && k < 5) {
        if (a[i] == b[j] && b[j] == c[k]) {
            printf("%d ", a[i]);
            i++;
            j++;
            k++;
        } else if (a[i] < b[j]) {
            i++;
        } else if (b[j] < c[k]) {
            j++;
        } else {
            k++;
        }
    }
    return 0;
}
