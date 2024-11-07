#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct sBloco {

} BLOCO;

typedef struct sInode {
    int numero;
    char descricao[100];
    BLOCO blocos[50];
} INODE;

typedef struct sFilaInode {
    INODE *next;
} FILA;

// preciso trocar isso aqui pra funcionar com a fila de inodes 
void terminalChar(char destino[]){ 
    printf("%s > ", destino);
}

void proximaAcao(char *comando[], char *destino[]){
    char leitura[50];
    scanf(" %[^\n]", &leitura);
    
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

int main(){
    int saida = 0;
    char *comando[10], destino[50], local[50] = "home";
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