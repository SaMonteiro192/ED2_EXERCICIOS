#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAX 200
#define TAM 256

typedef struct {
    char cpf[20];
    char nome[80];
    char sobrenome[80];
    char telefone[30];
    char cidade[50];
} Cliente;

long indice[MAX];
int qtd = 0;

void pega_campo(char *dest, char **p) {
    char *fim = strchr(*p, '|');

    if (fim) {
        int n = fim - *p;
        strncpy(dest, *p, n);
        dest[n] = '\0';
        *p = fim + 1;
    } else {
        strcpy(dest, *p);
        *p += strlen(*p);
    }
}

int pega_registro(FILE *arq, Cliente *c) {
    char linha[TAM];
    char *p;

    if (!fgets(linha, TAM, arq))
        return 0;

    linha[strcspn(linha, "\n")] = '\0';
    p = linha;

    pega_campo(c->cpf, &p);
    pega_campo(c->nome, &p);
    pega_campo(c->sobrenome, &p);
    pega_campo(c->telefone, &p);
    pega_campo(c->cidade, &p);

    return 1;
}

void grava_registro(FILE *arq, Cliente *c) {
    fprintf(arq, "%s|%s|%s|%s|%s\n",
            c->cpf, c->nome, c->sobrenome,
            c->telefone, c->cidade);
}

void reindexar(const char *nome) {
    FILE *arq = fopen(nome, "r");
    Cliente c;

    qtd = 0;

    if (!arq)
        return;

    while (1) {
        long pos = ftell(arq);

        if (!pega_registro(arq, &c))
            break;

        if (qtd < MAX)
            indice[qtd++] = pos;
    }

    fclose(arq);
}

int buscar(FILE *arq, const char *cpf) {
    int ini = 0, fim = qtd - 1;

    while (ini <= fim) {
        int meio = (ini + fim) / 2;
        Cliente c;

        fseek(arq, indice[meio], SEEK_SET);

        if (!pega_registro(arq, &c))
            return -1;

        int cmp = strcmp(cpf, c.cpf);

        if (cmp == 0)
            return meio;

        if (cmp < 0)
            fim = meio - 1;
        else
            ini = meio + 1;
    }

    return -1;
}

int inserir(const char *nome, Cliente novo) {
    FILE *in = fopen(nome, "r");
    FILE *out = fopen("temp.bin", "w");
    Cliente c;
    int inserido = 0;

    if (!out)
        return 0;

    if (in) {
        while (pega_registro(in, &c)) {
            int cmp = strcmp(novo.cpf, c.cpf);

            if (cmp == 0) {
                fclose(in);
                fclose(out);
                remove("temp.bin");
                return 0;
            }

            if (!inserido && cmp < 0) {
                grava_registro(out, &novo);
                inserido = 1;
            }

            grava_registro(out, &c);
        }

        fclose(in);
    }

    if (!inserido)
        grava_registro(out, &novo);

    fclose(out);

    remove(nome);
    rename("temp.bin", nome);

    reindexar(nome);
    return 1;
}

int remover(const char *nome, const char *cpf) {
    FILE *in = fopen(nome, "r");
    FILE *out = fopen("temp.bin", "w");
    Cliente c;
    int encontrou = 0;

    if (!in || !out)
        return 0;

    while (pega_registro(in, &c)) {
        if (strcmp(c.cpf, cpf) == 0)
            encontrou = 1;
        else
            grava_registro(out, &c);
    }

    fclose(in);
    fclose(out);

    if (!encontrou) {
        remove("temp.bin");
        return 0;
    }

    remove(nome);
    rename("temp.bin", nome);

    reindexar(nome);
    return 1;
}

int atualizar(const char *nome, const char *cpf,
              const char *novoNome,
              const char *novoSobrenome) {
    FILE *in = fopen(nome, "r");
    FILE *out = fopen("temp.bin", "w");
    Cliente c;
    int encontrou = 0;

    if (!in || !out)
        return 0;

    while (pega_registro(in, &c)) {
        if (strcmp(c.cpf, cpf) == 0) {
            strcpy(c.nome, novoNome);
            strcpy(c.sobrenome, novoSobrenome);
            encontrou = 1;
        }

        grava_registro(out, &c);
    }

    fclose(in);
    fclose(out);

    if (!encontrou) {
        remove("temp.bin");
        return 0;
    }

    remove(nome);
    rename("temp.bin", nome);

    reindexar(nome);
    return 1;
}

void merge(const char *a_nome, const char *b_nome,
           const char *saida) {
    FILE *a = fopen(a_nome, "r");
    FILE *b = fopen(b_nome, "r");
    FILE *out = fopen(saida, "w");

    Cliente aC, bC;
    int temA, temB;

    if (!a || !b || !out) {
        printf("Erro ao abrir arquivos.\n");
        return;
    }

    temA = pega_registro(a, &aC);
    temB = pega_registro(b, &bC);

    while (temA && temB) {
        int cmp = strcmp(aC.cpf, bC.cpf);

        if (cmp < 0) {
            grava_registro(out, &aC);
            temA = pega_registro(a, &aC);
        } else if (cmp > 0) {
            grava_registro(out, &bC);
            temB = pega_registro(b, &bC);
        } else {
            grava_registro(out, &aC);
            temA = pega_registro(a, &aC);
            temB = pega_registro(b, &bC);
        }
    }

    while (temA) {
        grava_registro(out, &aC);
        temA = pega_registro(a, &aC);
    }

    while (temB) {
        grava_registro(out, &bC);
        temB = pega_registro(b, &bC);
    }

    fclose(a);
    fclose(b);
    fclose(out);
}

void listar(const char *nome) {
    FILE *arq = fopen(nome, "r");
    Cliente c;

    if (!arq)
        return;

    while (pega_registro(arq, &c)) {
        printf("%s | %s %s | %s | %s\n",
               c.cpf, c.nome, c.sobrenome,
               c.telefone, c.cidade);
    }

    fclose(arq);
}

int main() {
    const char *arquivo = "clientes.bin";
    int opcao;

    reindexar(arquivo);

    do {
        printf("\n1 - Inserir\n");
        printf("2 - Remover\n");
        printf("3 - Atualizar\n");
        printf("4 - Buscar CPF\n");
        printf("5 - Listar\n");
        printf("6 - Merge\n");
        printf("0 - Sair\n");
        printf("Opcao: ");
        scanf("%d", &opcao);
        getchar();

        if (opcao == 1) {
            Cliente c;

            printf("CPF: ");
            fgets(c.cpf, 20, stdin);
            c.cpf[strcspn(c.cpf, "\n")] = 0;

            printf("Nome: ");
            fgets(c.nome, 80, stdin);
            c.nome[strcspn(c.nome, "\n")] = 0;

            printf("Sobrenome: ");
            fgets(c.sobrenome, 80, stdin);
            c.sobrenome[strcspn(c.sobrenome, "\n")] = 0;

            printf("Telefone: ");
            fgets(c.telefone, 30, stdin);
            c.telefone[strcspn(c.telefone, "\n")] = 0;

            printf("Cidade: ");
            fgets(c.cidade, 50, stdin);
            c.cidade[strcspn(c.cidade, "\n")] = 0;

            if (inserir(arquivo, c))
                printf("Inserido.\n");
            else
                printf("CPF ja existente.\n");
        }

        else if (opcao == 2) {
            char cpf[20];

            printf("CPF: ");
            fgets(cpf, 20, stdin);
            cpf[strcspn(cpf, "\n")] = 0;

            if (remover(arquivo, cpf))
                printf("Removido.\n");
            else
                printf("CPF nao encontrado.\n");
        }

        else if (opcao == 3) {
            char cpf[20], nome[80], sobrenome[80];

            printf("CPF: ");
            fgets(cpf, 20, stdin);
            cpf[strcspn(cpf, "\n")] = 0;

            printf("Novo nome: ");
            fgets(nome, 80, stdin);
            nome[strcspn(nome, "\n")] = 0;

            printf("Novo sobrenome: ");
            fgets(sobrenome, 80, stdin);
            sobrenome[strcspn(sobrenome, "\n")] = 0;

            if (atualizar(arquivo, cpf, nome, sobrenome))
                printf("Atualizado.\n");
            else
                printf("CPF nao encontrado.\n");
        }

        else if (opcao == 4) {
            char cpf[20];
            int pos;
            FILE *arq;

            printf("CPF: ");
            fgets(cpf, 20, stdin);
            cpf[strcspn(cpf, "\n")] = 0;

            arq = fopen(arquivo, "r");

            if (!arq) {
                printf("Arquivo inexistente.\n");
                continue;
            }

            pos = buscar(arq, cpf);

            if (pos >= 0) {
                Cliente c;

                fseek(arq, indice[pos], SEEK_SET);
                pega_registro(arq, &c);

                printf("%s | %s %s | %s | %s\n",
                       c.cpf, c.nome, c.sobrenome,
                       c.telefone, c.cidade);
            } else {
                printf("CPF nao encontrado.\n");
            }

            fclose(arq);
        }

        else if (opcao == 5) {
            listar(arquivo);
        }

        else if (opcao == 6) {
            merge("clientes.bin", "importados.bin",
                  "clientes_merge.bin");
            printf("Merge realizado.\n");
        }

    } while (opcao != 0);

    return 0;
}
