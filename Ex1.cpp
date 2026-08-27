
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

int main() {
    FILE *arquivo;
    char ch;
    arquivo = fopen("texto.txt", "w");

    if (arquivo == NULL) {
        printf("erro ao criar o arquivo\n");
        return 1;
    }

    printf("Digite seu texto ou pressione ESC para encerrar:\n");

    while (1) {
        ch = getch();

        if (ch == 27) {
            break;
        }

        putchar(ch);
        fputc(ch, arquivo);
    }

    fclose(arquivo);

    printf("fim!");

    return 0;
}

