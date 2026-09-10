#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define ARQ_DADOS "seguradoras.dat"

typedef struct {
    char cod[4];
    char nome[51];
    char seguradora[51];
    char tipo[31];
} Registro;

void inicializa_arquivo() {
    FILE *f = fopen(ARQ_DADOS, "rb+");
    if (f == NULL) {
        f = fopen(ARQ_DADOS, "wb+");
        int cabecalho = -1;
        fwrite(&cabecalho, sizeof(int), 1, f);
    }
    if (f != NULL) fclose(f);
}

void inserir_registro(Registro reg) {
    FILE *f = fopen(ARQ_DADOS, "rb+");
    if (!f) return;

    char buffer[200];
    sprintf(buffer, "%s#%s#%s#%s", reg.cod, reg.nome, reg.seguradora, reg.tipo);
    int tam_necessario = strlen(buffer);

    int offset_atual, offset_anterior = -1;
    int cabecalho;
    
    rewind(f);
    fread(&cabecalho, sizeof(int), 1, f);
    offset_atual = cabecalho;

    int tam_espaco;
    char status;
    int prox_offset;
    int achou_espaco = 0;

    while (offset_atual != -1) {
        fseek(f, offset_atual, SEEK_SET);
        fread(&tam_espaco, sizeof(int), 1, f);
        fread(&status, sizeof(char), 1, f);
        fread(&prox_offset, sizeof(int), 1, f);

        if (tam_espaco >= tam_necessario) {
            achou_espaco = 1;
            break;
        }
        offset_anterior = offset_atual;
        offset_atual = prox_offset;
    }

    if (achou_espaco) {
        if (offset_anterior == -1) {
            rewind(f);
            fwrite(&prox_offset, sizeof(int), 1, f);
        } else {
            fseek(f, offset_anterior + sizeof(int) + sizeof(char), SEEK_SET);
            fwrite(&prox_offset, sizeof(int), 1, f);
        }

        fseek(f, offset_atual, SEEK_SET);
        fwrite(&tam_espaco, sizeof(int), 1, f); 
        fwrite(buffer, sizeof(char), tam_necessario, f);
        
    } else {
        fseek(f, 0, SEEK_END);
        fwrite(&tam_necessario, sizeof(int), 1, f);
        fwrite(buffer, sizeof(char), tam_necessario, f);
    }

    fclose(f);
}

void remover_registro(char* codigo) {
    FILE *f = fopen(ARQ_DADOS, "rb+");
    if (!f) return;

    int cabecalho;
    rewind(f);
    fread(&cabecalho, sizeof(int), 1, f);

    int tam;
    int offset_atual = sizeof(int);

    while (fread(&tam, sizeof(int), 1, f) == 1) {
        char *buffer = (char *)malloc(tam + 1);
        fread(buffer, sizeof(char), tam, f);
        buffer[tam] = '\0';

        if (buffer[0] != '*') {
            char cod_lido[4];
            strncpy(cod_lido, buffer, 3);
            cod_lido[3] = '\0';

            if (strcmp(cod_lido, codigo) == 0) {
                fseek(f, offset_atual, SEEK_SET);
                
                fwrite(&tam, sizeof(int), 1, f);
                char marcador = '*';
                fwrite(&marcador, sizeof(char), 1, f);
                fwrite(&cabecalho, sizeof(int), 1, f);

                rewind(f);
                fwrite(&offset_atual, sizeof(int), 1, f);
                
                free(buffer);
                break; 
            }
        }
        free(buffer);
        offset_atual = ftell(f);
    }
    fclose(f);
}

void compactar_arquivo() {
    FILE *f = fopen(ARQ_DADOS, "rb");
    if (!f) return;

    FILE *f_temp = fopen("temp.dat", "wb");
    int cabecalho = -1;
    fwrite(&cabecalho, sizeof(int), 1, f_temp);

    fseek(f, sizeof(int), SEEK_SET);
    int tam;

    while (fread(&tam, sizeof(int), 1, f) == 1) {
        char *buffer = (char *)malloc(tam);
        fread(buffer, sizeof(char), tam, f);

        if (buffer[0] != '*') {
            int tamanho_real = 0;
            while(tamanho_real < tam && buffer[tamanho_real] != '\0' && buffer[tamanho_real] != '*') {
                tamanho_real++;
            }
            
            fwrite(&tamanho_real, sizeof(int), 1, f_temp);
            fwrite(buffer, sizeof(char), tamanho_real, f_temp);
        }
        free(buffer);
    }

    fclose(f);
    fclose(f_temp);

    remove(ARQ_DADOS);
    rename("temp.dat", ARQ_DADOS);
    printf("Arquivo compactado com sucesso!\n");
}

void dump_arquivo() {
    FILE *f = fopen(ARQ_DADOS, "rb");
    if (!f) {
        printf("Erro ao abrir arquivo.\n");
        return;
    }

    int cabecalho;
    fread(&cabecalho, sizeof(int), 1, f);
    printf("Cabecalho (offset topo lista): %d\n\n", cabecalho);

    int tam;
    while (fread(&tam, sizeof(int), 1, f) == 1) {
        int offset = ftell(f) - sizeof(int);
        char *buffer = (char *)malloc(tam + 1);
        fread(buffer, sizeof(char), tam, f);
        buffer[tam] = '\0';

        if (buffer[0] == '*') {
            int prox;
            fseek(f, offset + sizeof(int) + sizeof(char), SEEK_SET);
            fread(&prox, sizeof(int), 1, f);
            printf("[%d] ESPACO REMOVIDO | Tamanho: %d | Prox: %d\n", offset, tam, prox);
            
            fseek(f, offset + sizeof(int) + tam, SEEK_SET);
        } else {
            printf("[%d] REGISTRO | Tamanho: %d | Dados: %s\n", offset, tam, buffer);
        }
        free(buffer);
    }
    fclose(f);
}

int main() {
    inicializa_arquivo();
    int opcao = -1;

    while (opcao != 0) {
        printf("\n--- Sistema de Seguradoras ---\n");
        printf("1. Insercao\n");
        printf("2. Remocao\n");
        printf("3. Compactacao\n");
        printf("4. Dump Arquivo\n");
        printf("5. Carregar de binario (Simulacao)\n");
        printf("0. Sair\n");
        printf("Opcao: ");
        scanf("%d", &opcao);

        if (opcao == 1) {
            Registro r;
            printf("Codigo (3 chars): "); scanf("%s", r.cod);
            printf("Nome: "); scanf(" %[^\n]", r.nome);
            printf("Seguradora: "); scanf(" %[^\n]", r.seguradora);
            printf("Tipo: "); scanf(" %[^\n]", r.tipo);
            inserir_registro(r);
        } else if (opcao == 2) {
            char cod[4];
            printf("Codigo para remover: ");
            scanf("%s", cod);
            remover_registro(cod);
        } else if (opcao == 3) {
            compactar_arquivo();
        } else if (opcao == 4) {
            dump_arquivo();
        } else if (opcao == 5) {
            printf("Logica de carga do .bin dependente da implementacao.\n");
        }
    }
    return 0;
}
