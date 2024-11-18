#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define qntd_blocos 10
#define tf_bloco 4096

// array de bits pra guardar o mapa
unsigned char mapa_de_bits[(qntd_blocos + 7) / 8];

typedef struct sBloco {
    // long int endereco;
    // int status; // 1 -> ocupado, 0 -> livre
    // alterar pra mapa/lista de bits
    char dados[tf_bloco];
} Bloco;

typedef struct lBlocos {
    // saber blocos livres
    Bloco *bloco;
    int status; // 1 -> ocupado, 0 -> livre
    struct lBlocos *next;
} ListaBlocos;

typedef struct sInode {
    int id;
    char tipo[1]; // D -> diretório, A -> arquivos diversos
    char descricao[100];
    // int num_blocos;
    Bloco blocos[qntd_blocos];
} INode;

typedef struct lINode {
    INode *inode;
    struct lINode *next;
} ListaINode;

typedef struct sDiretorio {

} Diretorio;

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
void criarDAT(Bloco bloco, int i) {
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
        fwrite(&bloco, sizeof(Bloco), 1, file);
        fclose(file);
        // testar
        // printf("bloco %d salvo em: %s.\n", i, nomeBloco);
    } else {
        // printf("Erro ao criar bloco\n");
    }
}

// init qntd_blocos
void inicializarBlocos() {
    // inicia bloco com endereco i e livre
    for (long int i = 1; i <= qntd_blocos; i++) {
        Bloco bloco = {0};
        criarDAT(bloco, i);
    }
}
// init mapa de bits, setando tudo livre
void inicializarMapaDeBits() {
    memset(mapa_de_bits, 0, sizeof(mapa_de_bits)); // Todos os bits serão 0 (blocos livres)
}

int alocarBloco() {
    for (int i = 0; i < qntd_blocos; i++) {
        // verifica se o bit ta livre -> == 0
        if ((mapa_de_bits[i / 8] & (1 << (i % 8))) == 0) {
            // aloca bloco como ocupado -> bit = 1
            mapa_de_bits[i / 8] |= (1 << (i % 8));
            return i; // retorna o i do bloco alocado
        }
    }
    return -1; // tudo ocupado
}

void liberarBloco(int indice) {
    if (indice >= 0 && indice < qntd_blocos) {
        // aloca bloco como livre -> bit = 0
        mapa_de_bits[indice / 8] &= ~(1 << (indice % 8));
    }
}

int statusBloco(int indice) {
    if (indice >= 0 && indice < qntd_blocos) {
        return (mapa_de_bits[indice / 8] & (1 << (indice % 8))) != 0;
    }
    return -1; // valor nao encontrado no mapa
}

// inicializar o sistema
void initSys(){
    // algoritmo pra verificar se existe a pasta blocos -> entender no stackoverflow
    struct stat st = {0};
    if (stat("blocos/", &st) == -1) {
        mkdir("blocos/");
    }

    // functions de init
    inicializarMapaDeBits();
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
