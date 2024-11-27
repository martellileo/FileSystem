#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Structs.h"
#include "Functions.h"

// inicializar o sistema
void initSys(unsigned char **mapaDeBits, ListaINode **listaInodes) {
    createDirectory("util/");
    createDirectory("util/blocos/");
    createDirectory("util/mapping/");

    // cria o mapa de bits e se tudo der certo, recarrega ele de volta
    *mapaDeBits = (unsigned char *)malloc(tfMapaBits * sizeof(unsigned char));
    if (*mapaDeBits == NULL) {
        printf("erro ao alocar memoria para o mapa de bits\n");
    } else {
        inicializarMapaDeBits(mapaDeBits, tfMapaBits);

        struct stat st = {0};
        if (stat("util/blocos/bloco_0.dat", &st) != 0) {
            printf("Sistema rodando pela primeira vez. Inicializando...\n");
            inicializarBlocos();

            system("pause");
            system("cls");
        } 
        
        // puxa a lista de inodes
        reconstruirListaDeInodes(listaInodes, *mapaDeBits);
    } 
}

int main(){
    int saida = 0;
    char comando[10], destino[50], local[50] = "home";
    unsigned char *mapaDeBits = NULL;
    ListaINode *listaInodes = NULL;

    initSys(&mapaDeBits, &listaInodes);

    do {
        terminalChar(local);
        char *leitura[100];

        proximaAcao(comando, destino, leitura);
        // printf("comando: %s, destino: %s", comando, destino);

        if(strcmp(comando, "cd") == 0){ // cd

            strcpy(local, destino);
        }
        else if (strcmp(comando, "exit") == 0){ // saida
            saida = 1;

            if (mapaDeBits != NULL) {
                salvarMapaDeBits(mapaDeBits, tfMapaBits); // Salva o mapa de bits
                free(mapaDeBits);
            }

            liberarListaInodes(&listaInodes); // Libera a lista de inodes alocados
            free(listaInodes);

        } else if(strcmp(comando, "clear") == 0) { // limpar tela
            system("cls");
        }
        else if(strcmp(comando, "ls") == 0) { // listar arquivos

        }
        else if(strcmp(comando, "cat") == 0) { // abrir arquivo
            if(destino[0] == '>') { // modo de escrita
                char *nome_arquivo = destino + 1;
                sscanf(leitura, "%s > %s", comando, nome_arquivo);
                printf("comando: %s, nome_arquivo: %s.", comando, nome_arquivo);
                comandoCat(nome_arquivo, 1, &listaInodes, mapaDeBits);
            } else {
                comandoCat(destino, 0, &listaInodes, mapaDeBits); // modo de leitura
            }
        }
        else if(strcmp(comando, "rm") == 0) { // remover arquivo

        }
        else if(strcmp(comando, "rmdir") == 0) { // remover diretorio

        }
        else if(strcmp(comando, "mv") == 0) { // mover arquivo

        }
        else if(strcmp(comando, "mkdir") == 0) { // criar diretorio

        }
        else if(strcmp(comando, "show") == 0) { // criar diretorio
            mostrarMapaDeBits(mapaDeBits, tfMapaBits);
        } else if (strcmp(comando, "freeinodes") == 0) { // mostrar inodes livres
            mostrarInodesLivres(mapaDeBits, tfMapaBits);
        } else if (strcmp(comando, "usedinodes") == 0) { // mostrar inodes ocupados
            mostrarInodesOcupados(mapaDeBits, tfMapaBits);
        } else {
            printf("comando %s nao reconhecido no sistema!\n", comando);
        }
    } while (saida != 1);

    if (mapaDeBits != NULL) {
        free(mapaDeBits);
    }
}
