#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Structs.h"
#include "Functions.h"

// inicializar o sistema
void initSys(){
    // functions de init
    mkdirBlocos("blocos/");
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
            if(destino[0] == '>') { // modo de escrita
                char *nome_arquivo = destino + 1;
                while(*nome_arquivo == ' ')
                    nome_arquivo++;
                comandoCat(nome_arquivo, 1);
            } else {
                comandoCat(destino, 0); // modo de leitura
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
            mostrarMapaDeBits();
        }
    } while (saida != 1);
}
