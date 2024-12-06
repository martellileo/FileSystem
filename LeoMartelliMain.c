#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Structs.h"
#include "Functions.h"

// inicializar o sistema
void initSys(unsigned char **mapaDeBits, ListaINode **listaInodes, ListaDiretorio **listaDiretorios) {
    createDirectory("util/");
    createDirectory("util/blocos/");
    createDirectory("util/mapping/");

    // cria o mapa de bits e se tudo der certo, recarrega ele de volta
    *mapaDeBits = (unsigned char *)calloc(tfMapaBits, sizeof(unsigned char));
    if (*mapaDeBits == NULL) {
        printf("erro ao alocar memoria para o mapa de bits\n");
    } else {
        struct stat st = {0};
        if (stat("util/blocos/bloco_0.dat", &st) != 0) {
            printf("Sistema rodando pela primeira vez. Inicializando blocos...\n");
            inicializarBlocos(); 
            salvarMapaDeBits(*mapaDeBits, tfMapaBits);   
        }  else {
            printf("Sistema ja inicializado. Carregando mapa de bits.\n");
            inicializarMapaDeBits(mapaDeBits, tfMapaBits);
        }
        
        reconstruirListaDeDiretorios(listaDiretorios);

        // se for a primeira vez que o sistema rodar, inicializa o root 'home'
        construirHome(listaDiretorios, listaInodes, *mapaDeBits);

        // puxa a lista de inodes
        reconstruirListaDeInodes(listaInodes, *mapaDeBits);
        
        // aguarda confirmacao do usuario
        printf("\nSistema pronto para uso.\n\n");
        system("pause");
        system("cls");
    } 
}

int main(){
    int saida = 0;
    char comando[10], destino[50], local[50] = "home";
    unsigned char *mapaDeBits = NULL;
    ListaINode *listaInodes = NULL;
    ListaDiretorio *listaDiretorios = NULL;

    initSys(&mapaDeBits, &listaInodes, &listaDiretorios);

    do {
        terminalChar(local);
        char *leitura[100];

        proximaAcao(comando, destino, leitura);
        // printf("comando: %s, destino: %s", comando, destino);

        if(strcmp(comando, "cd") == 0){ // cd
            // strcpy(local, destino);
        } else if (strcmp(comando, "exit") == 0) {
            saida = 1;

            if (mapaDeBits != NULL) {
                salvarMapaDeBits(mapaDeBits, tfMapaBits); // Salva o mapa de bits
                free(mapaDeBits);
            }

            salvarListaDeInodes(listaInodes);
            liberarListaInodes(&listaInodes);
            salvarListaDeDiretorios(listaDiretorios);
            free(listaDiretorios);
            free(listaInodes);
        } else if(strcmp(comando, "clear") == 0) { // limpar tela
            system("cls");
        } else if(strcmp(comando, "cat") == 0) { // abrir arquivo
            if(destino[0] == '>') { // modo de escrita
                char *nome_arquivo = destino + 1;
                sscanf(leitura, "%s > %s", comando, nome_arquivo);
                printf("comando: %s, nome_arquivo: %s.", comando, nome_arquivo);
                comandoCat(nome_arquivo, 1, &listaInodes, mapaDeBits);
            } else {
                comandoCat(destino, 0, &listaInodes, mapaDeBits); // modo de leitura
            }
        }
        else if(strcmp(comando, "show") == 0) { // mostrar mapa de bits
            mostrarMapaDeBits(mapaDeBits, tfMapaBits);
        } else if (strcmp(comando, "freeinodes") == 0) { // mostrar inodes livres
            mostrarInodesLivres(mapaDeBits, tfMapaBits);
        } else if (strcmp(comando, "usedinodes") == 0) { // mostrar inodes ocupados
            mostrarInodesOcupados(mapaDeBits, tfMapaBits);
        } else if (strcmp(comando, "directorylist") == 0) { // mostrar lista de diretorios
            listarDiretorios(listaDiretorios);
        } else if(strcmp(comando, "mkdir") == 0) { // criar diretorio
            comandoMkdir(destino, &listaDiretorios, &listaInodes, mapaDeBits);
        } else if(strcmp(comando, "mv") == 0) { // mover arquivo

        } else if(strcmp(comando, "rm") == 0) { // remover arquivo

        } else if(strcmp(comando, "rmdir") == 0) { // remover diretorio

        } else if(strcmp(comando, "ls") == 0) { // listar arquivos
            comandoLs(listaDiretorios, listaInodes);
        } else {
            printf("comando %s nao reconhecido no sistema!\n", comando);
        }
    } while (saida != 1);
}
