#ifndef blocos_h
#define blocos_h

#include <stdio.h>
#include <stdlib.h>

#define tfCluster   128                                 // tfCluster em bytes
#define tfDisco    8192                                 // tfDisco em Bytes
#define totalBlocks (tfDisco / tfCluster)               // conta de max blocos
#define entradas (tfCluster /sizeof(Diretorio))         // calcula o maximo de entradas de diretório
#define tfMapaBits ((totalBlocks +7)/8)

/*
    Blocos             
*/
typedef struct sBloco {
    char dados[tfCluster];
} Bloco;

/*
    A lista de blocos vai ser usada para saber quais blocos estão ocupados e quais estão livres.
*/
typedef struct lBlocos {
    Bloco *bloco;
    int status; // 1 -> ocupado, 0 -> livre
    struct lBlocos *next;
} ListaBlocos;


/*
    Inodes                                                            
*/

typedef struct sInode {
    int id;
    char tipo[1]; // D -> diretório, A -> arquivos diversos
    char descricao[100];
    Bloco blocos[totalBlocks];
} INode;

/*
    A lista de inodes é utilizada para navegacao e ls
*/
typedef struct lINode {
    INode *inode;
    struct lINode *next;
    struct lINode *prev;
} ListaINode;


/*
    Diretórios                                             
*/
typedef struct lDiretorio {
    INode *inode;
    char nome[100];
    struct lDiretorio *next;
    struct lDiretorio *prev;
} ListaDiretorio;

#endif