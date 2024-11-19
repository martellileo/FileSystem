#ifndef blocos_h
#define blocos_h

#include <stdio.h>
#include <stdlib.h>

#define CLUSTER_B   128                                 // tamanho do cluster em bytes
#define DISCO_B     8192                                // tamanho do disco em Bytes
#define TOTAL_BLOCOS (DISCO_B / CLUSTER_B)              // calcula a quantidade de blocos
#define MAX_ENTRADAS (CLUSTER_B /sizeof(Diretorio))     // calcula o maximo de entradas de diretório

typedef struct sBloco {
    // long int endereco;
    // int status; // 1 -> ocupado, 0 -> livre
    // alterar pra mapa/lista de bits
    char dados[CLUSTER_B];
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
    Bloco blocos[TOTAL_BLOCOS];
} INode;

typedef struct lINode {
    INode *inode;
    struct lINode *next;
} ListaINode;

typedef struct sDiretorio {

} Diretorio;

#endif