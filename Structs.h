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
     ____  _                     
    |  _ \| |                    
    | |_) | | ___   ___ ___  ___ 
    |  _ <| |/ _ \ / __/ _ \/ __|
    | |_) | | (_) | (_| (_) \__ \
    |____/|_|\___/ \___\___/|___/
                                                
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
      _____                 _           
     |_   _|               | |          
       | |  _ __   ___   __| | ___  ___ 
       | | | '_ \ / _ \ / _` |/ _ \/ __|
      _| |_| | | | (_) | (_| |  __/\__ \
     |_____|_| |_|\___/ \__,_|\___||___/
                                                                  
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
  _____  _          _    __       _           
 |  __ \(_)        | |  /_/      (_)          
 | |  | |_ _ __ ___| |_ ___  _ __ _  ___  ___ 
 | |  | | | '__/ _ \ __/ _ \| '__| |/ _ \/ __|
 | |__| | | | |  __/ || (_) | |  | | (_) \__ \
 |_____/|_|_|  \___|\__\___/|_|  |_|\___/|___/                                     
                                              
*/
typedef struct sDiretorio {

} Diretorio;

#endif