#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main() {
    FILE *arquivo;
    unsigned char buffer[16];
    size_t b_lidos, i;

    arquivo = fopen("texto2.txt", "rb");

    if (arquivo == NULL) {
        printf("erro ao abrir o arquivo\n");
        return 1;
    }

    while ((b_lidos = fread(buffer, 1, 16, arquivo)) > 0) {
        for (i = 0; i < 16; i++) {
            if (i < b_lidos) {
                printf("%02X ", buffer[i]);
            } else {
                printf("   ");
            }
        }

        for (i = 0; i < b_lidos; i++) {
            if (isprint(buffer[i])) {
                printf("%c", buffer[i]);
            } else {
                printf(".");
            }
        }
        
        printf("\n");
    }

    fclose(arquivo);
    return 0;
}