#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TAM_LINHA 1024

int main() {
    FILE *arq;
    int num_linhas, linhas_lidas = 0, i;
    char **historico;

    printf("Informe o numero de linhas para ler do final: ");
    scanf("%d", &num_linhas);

    if (num_linhas <= 0) {
        return 0;
    }

    arq = fopen("dados_tail.txt", "r");

    if (arq == NULL) {
        arq = fopen("dados_tail.txt", "w");
        for (i = 0; i < 15; i++) {
            fprintf(arq, "Conteudo gerado automaticamente - Linha %d\n", i + 1);
        }
        fclose(arq);
        arq = fopen("dados_tail.txt", "r");
    }

    historico = malloc(num_linhas * sizeof(char *));
    for (i = 0; i < num_linhas; i++) {
        historico[i] = malloc(TAM_LINHA * sizeof(char));
    }

    while (fgets(historico[linhas_lidas % num_linhas], TAM_LINHA, arq)) {
        linhas_lidas++;
    }

    fclose(arq);

    if (linhas_lidas < num_linhas) {
        for (i = 0; i < linhas_lidas; i++) {
            printf("%s", historico[i]);
        }
    } else {
        int ponto_corte = linhas_lidas % num_linhas;
        
        for (i = ponto_corte; i < num_linhas; i++) {
            printf("%s", historico[i]);
        }
        for (i = 0; i < ponto_corte; i++) {
            printf("%s", historico[i]);
        }
    }

    for (i = 0; i < num_linhas; i++) {
        free(historico[i]);
    }
    free(historico);

    return 0;
}