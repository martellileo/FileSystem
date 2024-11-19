#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Structs.h"

//variaveis de controle global
unsigned char mapa_de_bits[(TOTAL_BLOCOS + 7) / 8]; // array de bits pra guardar o mapa
ListaINode *lista_inodes = NULL; // lista global (sera q da bo)?

void mkdirBlocos(const char path[]){
    // algoritmo pra verificar se existe a pasta blocos -> entender no stackoverflow
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        mkdir(path);
    }
}

INode *criarINode(char *nome, char *tipo) {
    INode *novo_inode = (INode *)malloc(sizeof(INode));
    if (novo_inode == NULL) {
        printf("Erro ao alocar memória para o inode.\n");
        return NULL;
    }
    static int id_contador = 1; // ID único para cada inode
    novo_inode->id = id_contador++;
    strcpy(novo_inode->tipo, tipo);
    snprintf(novo_inode->descricao, sizeof(novo_inode->descricao), "%s", nome);

    return novo_inode;
}

void addINodeLista(INode *inode) {
    ListaINode *novo = (ListaINode *)malloc(sizeof(ListaINode));
    novo->inode = inode;
    novo->next = lista_inodes;
    lista_inodes = novo;
}

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

// salvar bloco em .dat
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

// init vlocos definidos pelo cluster
void inicializarBlocos() {
    // inicia bloco com endereco i e livre
    for (long int i = 1; i <= TOTAL_BLOCOS; i++) {
        Bloco bloco = {0};
        criarDAT(bloco, i);
    }
}
// init mapa de bits, setando tudo livre
void inicializarMapaDeBits() {
    memset(mapa_de_bits, 0, sizeof(mapa_de_bits)); // Todos os bits serão 0 (blocos livres)
}

int alocarBloco() {
    for (int i = 0; i < TOTAL_BLOCOS; i++) {
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
    if (indice >= 0 && indice < TOTAL_BLOCOS) {
        // aloca bloco como livre -> bit = 0
        mapa_de_bits[indice / 8] &= ~(1 << (indice % 8));
    }
}

int statusBloco(int indice) {
    if (indice >= 0 && indice < TOTAL_BLOCOS) {
        return (mapa_de_bits[indice / 8] & (1 << (indice % 8))) != 0;
    }
    return -1; // valor nao encontrado no mapa
}

void mostrarMapaDeBits() {
    printf("Mapa de Bits:\n");
    for (int i = 0; i < TOTAL_BLOCOS; i++) {
        printf("%d", (mapa_de_bits[i / 8] & (1 << (i % 8))) != 0);
        if ((i + 1) % 8 == 0) {
            printf(" ");
        }
    }
    printf("\n");
}

void escreverArquivo(char *arquivo_txt) {
    printf("Reading text...(Ctrl+D para finalizar):\n");

    INode *inode = criarINode(arquivo_txt, "A");
    if (inode == NULL) {
        printf("Erro ao criar inode para o arquivo.\n");
        return;
    }

    char buffer[CLUSTER_B];
    int bloco_index = 0;

    // getText e salvar em blocos
    while (fgets(buffer, CLUSTER_B, stdin)) {
        int bloco_id = alocarBloco();
        if (bloco_id == -1) {
            printf("sem espaço disponivel em bloco.\n");
            liberarBloco(bloco_id); // rowback
            return;
        }

        Bloco bloco;
        memset(bloco.dados, 0, CLUSTER_B);
        strncpy(bloco.dados, buffer, CLUSTER_B);

        criarDAT(bloco, bloco_id); // salvar em um .dat
        inode->blocos[bloco_index++] = bloco; // link bloco e inode
    }

    addINodeLista(inode); // adicionar a lista global
    printf("Arquivo %s salvo com sucesso.\n", arquivo_txt);
}

void lerArquivo(char *arquivo_txt) {
    ListaINode *atual = lista_inodes;

    // busca o inode
    while (atual != NULL) {
        if (strcmp(atual->inode->descricao, arquivo_txt) == 0) {
            INode *inode = atual->inode;

            printf("Conteúdo do arquivo %s:\n", arquivo_txt);
            for (int i = 0; i < TOTAL_BLOCOS && inode->blocos[i].dados[0] != '\0'; i++) {
                printf("%s", inode->blocos[i].dados);
            }
            return;
        }
        atual = atual->next;
    }

    printf("Arquivo %s não encontrado.\n", arquivo_txt);
}

void comandoCat(char *arquivo_txt, int escrever) {
    if (escrever) {
        escreverArquivo(arquivo_txt);
    } else {
        lerArquivo(arquivo_txt);
    }
}
