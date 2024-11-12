#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define qntd_blocos 10
#define tf_bloco 4096

typedef struct sBloco {
    // long int endereco;
    // int status; // 1 -> ocupado, 0 -> livre
    // alterar pra mapa/lista de bits
    char dados[tf_bloco];
} BLOCO;

typedef struct sInode {
    int id;
    char tipo[1]; // D -> diretório, A -> arquivos diversos
    char descricao[100];
    // int num_blocos;
    BLOCO blocos[qntd_blocos];
} INODE;

typedef struct sFilaInode {
    INODE *next;
    INODE *prev;
} FILA;

// preciso trocar isso aqui pra funcionar com a fila de inodes
void terminalChar(char destino[]){
    printf("%s > ", destino);
}

void proximaAcao(char *comando[], char *destino[]){
    char leitura[50];
    scanf(" %[^\n]", &leitura);

    // separa a string em tokens
    char *token = strtok(leitura, " ");
    if (token != NULL) {
        strcpy(comando, token);
        token = strtok(NULL, " "); // primeiro token vira comando
        if (token != NULL) {
            strcpy(destino, token); // segundo token vira destino
        } else {
            destino[0] = '\0'; // vazio se nao tiver nada depois do espaco
        }
    } else {
        // nao tem string = nao tem token
        comando[0] = '\0';
        destino[0] = '\0';
    }
}

// Salvar bloco em .dat se não existir
void criarDAT(BLOCO bloco, int i) {
    char nomeBloco[30];
    sprintf(nomeBloco, "blocos/bloco_%d.dat", i);

    // tenta abrir em read
    FILE *file = fopen(nomeBloco, "rb");
    if (file != NULL) {
        // se o arquivo já existe = mantem
        fclose(file);
        // testar
        // printf("Bloco %d no arquivo: %s.\n", i, nomeBloco);
        return;
    }

    // nao existe = cria
    file = fopen(nomeBloco, "wb");
    if (file != NULL) {
        fwrite(&bloco, sizeof(BLOCO), 1, file);
        fclose(file);
        // testar
        // printf("bloco %d salvo em: %s.\n", i, nomeBloco);
    } else {
        // printf("Erro ao criar bloco\n");
    }
}


// init 60 blocos
void inicializarBlocos() {
    // inicia bloco com endereco i e livre
    for (long int i = 1; i <= 60; i++) {
        BLOCO bloco = {0};
        criarDAT(bloco, i);
    }
}

// inicializar o sistema
void initSys(){
    // algoritmo pra verificar se existe a pasta blocos
    struct stat st = {0};

    if (stat("blocos/", &st) == -1) {
        mkdir("blocos/");
    }

    // inicializar os blocos
    inicializarBlocos();
}

int main(){
    int saida = 0;
    char *comando[10], destino[50], local[50] = "home";

    initSys();

    do {
        terminalChar(local);
        proximaAcao(comando, destino);
        printf("comando: %s, destino: %s", comando, destino);
        printf("\n");

        if(strcmp(comando, "cd") == 0){ // cd

            strcpy(local, destino);
        }
        else if (strcmp(comando, "exit") == 0){ // saida
            saida = 1;
        }
        else if(strcmp(comando, "clear") == 0) { // limpar tela
            system("cls");
        }
        else if(strcmp(comando, "ls") == 0) { // listar arquivos

        }
        else if(strcmp(comando, "cat") == 0) { // abrir arquivo

        }
        else if(strcmp(comando, "rm") == 0) { // remover arquivo

        }
        else if(strcmp(comando, "rmdir") == 0) { // remover diretorio

        }
        else if(strcmp(comando, "mv") == 0) { // mover arquivo

        }
        else if(strcmp(comando, "mkdir") == 0) { // criar diretorio

        }
    } while (saida != 1);
}
