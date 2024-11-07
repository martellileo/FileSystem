#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BLOCOS 50
#define TAM_BLOCO 256
#define NOME_ARQUIVO "sistema_arquivo.dat"

typedef struct sBloco {
    char dados[TAM_BLOCO];
} BLOCO;

typedef struct sInode {
    int numero;
    char descricao[100];
    BLOCO blocos[MAX_BLOCOS];
    int num_blocos; // Número de blocos usados
} INODE;

typedef struct sFilaInode {
    INODE *next;
} FILA;

void terminalChar(char destino[]) {
    printf("%s > ", destino);
}

void proximaAcao(char *comando, char *destino) {
    char leitura[50];
    scanf(" %[^\n]", leitura);
    
    char *token = strtok(leitura, " "); // separa a string em tokens
    if (token != NULL) {
        strcpy(comando, token); 
        token = strtok(NULL, " "); // primeiro token vira comando
        if (token != NULL) {
            strcpy(destino, token); // segundo token vira destino
        } else {
            destino[0] = '\0'; // vazio se nao tiver nada depois do espaco
        }
    } else {
        comando[0] = '\0'; // nao tem string = nao tem token
        destino[0] = '\0';
    }
}

void criarArquivo(char *nome) {
    FILE *file = fopen(NOME_ARQUIVO, "ab");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }
    INODE inode;
    inode.numero = ftell(file) / sizeof(INODE);
    strcpy(inode.descricao, nome);
    inode.num_blocos = 0;

    // Inicializa os blocos
    for (int i = 0; i < MAX_BLOCOS; i++) {
        memset(inode.blocos[i].dados, 0, TAM_BLOCO);
    }

    fwrite(&inode, sizeof(INODE), 1, file);
    fclose(file);
    printf("Arquivo '%s' criado com sucesso!\n", nome);
}

void listarArquivos() {
    FILE *file = fopen(NOME_ARQUIVO, "rb");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    INODE inode;
    printf("Arquivos:\n");
    while (fread(&inode, sizeof(INODE), 1, file)) {
        printf("Arquivo: %s\n", inode.descricao);
    }
    fclose(file);
}

void removerArquivo(char *nome) {
    // Remover arquivo não é trivial sem uma estrutura de dados mais complexa.
    // Para simplificação, apenas informamos que a remoção não é implementada.
    printf("Remoção de arquivo não implementada ainda.\n");
}

int main() {
    int saida = 0;
    char comando[10], destino[50], local[50] = "home";
    do {
        terminalChar(local);
        proximaAcao(comando, destino);
        printf("comando: %s, destino: %s\n", comando, destino);

        if (strcmp(comando, "cd") == 0) { // cd
            strcpy(local, destino); 
        } else if (strcmp(comando, "exit") == 0) { // sair
            saida = 1;
        } else if (strcmp(comando, "clear") == 0) { // limpar tela
            system("cls");
        } else if (strcmp(comando, "ls") == 0) { // listar arquivos
            listarArquivos();
        } else if (strcmp(comando, "mkdir") == 0) { // criar arquivo
            criarArquivo(destino);
        } else if (strcmp(comando, "rm") == 0) { // remover arquivo
            removerArquivo(destino);
        }
    } while (saida != 1);

    return 0;
}