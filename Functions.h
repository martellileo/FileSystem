#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Structs.h"

/*
    CORRIGIR:
    - conflitando o mapa de bits, ele nao cria ou inicializa se nao existir nada FEITO

    - alterar o cd para funcionar;

    - used e free nao estao resetando com o sistema sendo reiniciado FEITO

    FAZER:
    - DIRETORIO
    - ls
    - rm
    - rmdir
    - mkdir
    - documentacao
*/


/*
    Utilizado no initSys para criar diretorios auxiliares, como util/blocos e util/mapping
    algoritmo pra verificar se existe a pasta blocos -> entender no stackoverflow
*/
void createDirectory(const char path[]){
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        mkdir(path);
    }
}

/*
    Funcoes de call no init system:
 */
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

/*
    InitSys e algumas dependencias
*/
void reconstruirListaDeInodes(ListaINode **listaInodes, unsigned char *mapaDeBits) {
    // carregao arquivo
    FILE *file = fopen("./util/mapping/inodes.dat", "rb");
    if (file != NULL) {
        printf("Carregando lista de inodes de util/inodes.dat...\n");

        INode inodeTemp;
        while (fread(&inodeTemp, sizeof(INode), 1, file) == 1) {
            // cria um temp pra copiar do arquivo e depois salvar na lista
            INode *inode = (INode *)malloc(sizeof(INode));
            if (inode == NULL) {
                printf("Erro ao alocar memoria para inode.\n");
                fclose(file);
                return;
            }
            memcpy(inode, &inodeTemp, sizeof(INode));

            // salva o inode na lista
            ListaINode *novo = (ListaINode *)malloc(sizeof(ListaINode));
            if (novo == NULL) {
                printf("erro ao alocar memoria para no da lista de inodes.\n");
                free(inode);
                fclose(file);
                return;
            }

            novo->inode = inode;
            novo->next = *listaInodes;
            *listaInodes = novo;

            // atualiza o mapa de bits
            for (int i = 0; i < totalBlocks; i++) {
                if (inode->blocos[i].dados[0] != '\0') { // Bloco usado
                    mapaDeBits[i / 8] |= (1 << (i % 8));
                }
            }
        }

        fclose(file);
        printf("Lista de inodes carregada com sucesso.\n");
    } else {
        printf("Nenhum arquivo de inodes encontrado. Inicializando lista vazia.\n");
    }
}

// criar o bloco .dat
void criarDAT(Bloco bloco, int i) {
    char nomeBloco[30];
    sprintf(nomeBloco, "util/blocos/bloco_%d.dat", i);

    FILE *file = fopen(nomeBloco, "wb");
    if (file != NULL) {
        fwrite(&bloco, sizeof(Bloco), 1, file);
        fclose(file);
    } else {
        printf("erro ao criar o bloco %d em %s\n", i, nomeBloco);
    }
}

// inicializa os blocos se nao existirem
void inicializarBlocos() {
    for (long int i = 0; i < totalBlocks; i++) {
        char nomeBloco[30];
        sprintf(nomeBloco, "util/blocos/bloco_%ld.dat", i);

        struct stat st;
        if (stat(nomeBloco, &st) == 0) {
            continue; // ? nao sei pq funciona isso aqui
        }

        Bloco bloco = {0};
        criarDAT(bloco, i);
    }
    printf("blocos inicializados!\n");
}

void inicializarMapaDeBits(unsigned char **mapaDeBits, size_t tamanho) {
    // Alocar espaço para o mapa de bits na memória
    *mapaDeBits = (unsigned char *)calloc(tamanho, sizeof(unsigned char));
    if (*mapaDeBits == NULL) {
        printf("Erro ao alocar memória para o mapa de bits.\n");
        return;
    }

    // verifica se o mapa de bits existe
    FILE *file = fopen("./util/mapping/mapaDeBits.txt", "r");
    if (file == NULL) {
        printf("Mapa de bits nao existe. Criando um novo.\n");

        // se ele nao existir, cria um novo e seta tudo em 0
        salvarMapaDeBits(*mapaDeBits, tamanho);
    } else {
        printf("Mapa de bits carregado.\n");

        // copia do txt para memória
        char linha[256];
        size_t bitIndex = 0;

        while (fgets(linha, sizeof(linha), file) != NULL && bitIndex < tamanho * 8) {
            for (int i = 0; linha[i] != '\0' && linha[i] != '\n'; i++) {
                if (linha[i] == '0' || linha[i] == '1') {
                    if (linha[i] == '1') {
                        (*mapaDeBits)[bitIndex / 8] |= (1 << (bitIndex % 8));
                    }
                    bitIndex++;
                }
            }
        }

        fclose(file);

        // mapa de bits ta quebrado ->
        if (bitIndex < tamanho * 8) {
            printf("warning: mapaDeBits possui menos bits que o esperado, verifique seus dados.\n", bitIndex, tamanho * 8);
        }
    }
}
/*
    Lista de diretorios
*/
void salvarListaDeDiretorios(ListaDiretorio *listaDiretorios) {
    FILE *file = fopen("./util/mapping/listdirectorys.dat", "wb");
    if (file == NULL) {
        printf("Erro ao abrir arquivo para salvar lista de diretórios.\n");
        return;
    }

    ListaDiretorio *atual = listaDiretorios;
    while (atual != NULL) {
        // Grava o campo `nome` no arquivo
        fwrite(atual->nome, sizeof(char), sizeof(atual->nome), file);
        atual = atual->next;
    }

    fclose(file);
    printf("Lista de diretórios salva com sucesso.\n");
}

// Função para reconstruir a lista de diretórios a partir do arquivo
void reconstruirListaDeDiretorios(ListaDiretorio **listaDiretorios) {
    FILE *file = fopen("./util/mapping/listdirectorys.dat", "rb");
    if (file == NULL) {
        printf("Lista de diretórios não existe. Inicializando uma nova lista.\n");
        return;
    }

    while (1) {
        char nome[50];
        // Lê um nome de diretório do arquivo
        if (fread(nome, sizeof(char), sizeof(nome), file) != sizeof(nome)) {
            // Sai do loop ao atingir o final do arquivo ou em caso de erro
            break;
        }

        // Cria um novo nó para a lista
        ListaDiretorio *novoDiretorio = (ListaDiretorio *)malloc(sizeof(ListaDiretorio));
        if (novoDiretorio == NULL) {
            printf("Erro ao alocar memória para o diretório.\n");
            fclose(file);
            return;
        }

        // Copia o nome lido para o novo nó
        strcpy(novoDiretorio->nome, nome);
        novoDiretorio->next = *listaDiretorios;

        // Caso seja lista duplamente encadeada
        // novoDiretorio->prev = NULL;
        // if (*listaDiretorios != NULL) {
        //     (*listaDiretorios)->prev = novoDiretorio;
        // }

        *listaDiretorios = novoDiretorio;
    }

    fclose(file);
    printf("Lista de diretórios carregada com sucesso.\n");
}


/*
    Se for a primeira vez rodando o programa, ele cria o root para a navegacao entre diretórios
*/
void construirHome(ListaDiretorio **listaDiretorios, ListaINode **listaInodes, unsigned char *mapaDeBits) {
    if (*listaDiretorios == NULL) {
        printf("Criando root 'home'...\n");

        INode *inodeHome = criarINode("home", "D");
        if (inodeHome == NULL) {
            printf("Erro ao criar inode para o root.\n");
            return;
        }

        // Atualizar mapa de bits para o inode "home"
        int blocoHome = alocarBloco(mapaDeBits);
        if (blocoHome == -1) {
            printf("warning: sem espaco no mapa de bits para o inode 'home'.\n");
            free(inodeHome);
            return;
        }
        inodeHome->blocos[0] = (Bloco){0}; // Inicializar o primeiro bloco do diretório

        // Adicionar inode à lista de inodes
        ListaINode *novoINode = (ListaINode *)malloc(sizeof(ListaINode));
        if (novoINode == NULL) {
            printf("Erro ao alocar memória para a lista de inodes.\n");
            free(inodeHome);
            return;
        }
        novoINode->inode = inodeHome;
        novoINode->next = *listaInodes;
        *listaInodes = novoINode;

        // Adicionar diretório à lista de diretórios
        ListaDiretorio *novoDiretorio = (ListaDiretorio *)malloc(sizeof(ListaDiretorio));
        if (novoDiretorio == NULL) {
            printf("Erro ao alocar memória para a lista de diretórios.\n");
            free(inodeHome);
            return;
        }
        strcpy(novoDiretorio->nome, "home");
        novoDiretorio->inode = inodeHome;
        novoDiretorio->next = *listaDiretorios;
        novoDiretorio->prev = NULL;
        *listaDiretorios = novoDiretorio;

        printf("Diretorio raiz 'home' criado com sucesso.\n");
    } else {
        printf("Diretorio raiz 'home' já existe.\n");
    }
}

void listarDiretorios(ListaDiretorio *listaDiretorios) {
    if (listaDiretorios == NULL) {
        printf("nenhum diretorio encontrado.\n");
        return;
    } else {
        printf("lista de diretorios:\n");
        ListaDiretorio *atual = listaDiretorios;
        while (atual != NULL) {
            printf("- %s\t", atual->nome);
            atual = atual->next;
        }
    }
    printf("\n");
}

/*
    Funções para tratar os inodes
*/
void listarInodes(ListaINode *listaInodes) {
    ListaINode *atual = listaInodes;
    printf("Lista de Arquivos:\n");
    while (atual != NULL) {
        printf("- %s", atual->inode->descricao);
        atual = atual->next;
    }
}

void liberarListaInodes(ListaINode **listaInodes) {
    ListaINode *atual = *listaInodes;
    while (atual != NULL) {
        ListaINode *temp = atual;
        atual = atual->next;
        free(temp->inode);
        free(temp);
    }
    *listaInodes = NULL;
}

void mostrarInodesLivres(const unsigned char *mapaDeBits, size_t tamanho) {
    printf("inodes livres:\n\t");
    int encontrouLivres = 0;

    for (size_t i = 0; i < tamanho * 8; i++) {
        if (((mapaDeBits[i / 8] >> (i % 8)) & 1) == 0) {
            printf("Bloco |%zu| ->\t", i);
            encontrouLivres = 1;
        }
    }
    printf("\n");

    if (!encontrouLivres) {
        printf("todos os inodes estao ocupados!\n");
    }
}

void mostrarInodesOcupados(const unsigned char *mapaDeBits, size_t tamanho) {
    printf("inodes ocupados:\n\t");
    int encontrouOcupados = 0;

    for (size_t i = 0; i < tamanho * 8; i++) {
        if ((mapaDeBits[i / 8] >> (i % 8)) & 1) {
            printf("Bloco |%zu| -> \t", i);
            encontrouOcupados = 1;
        }
    }
    printf("\n");
    
    if (!encontrouOcupados) {
        printf("todos os inodes estao livres!\n");
    }
}

void salvarListaDeInodes(ListaINode *listaInodes) {
    FILE *file = fopen("./util/mapping/inodes.dat", "wb");
    if (file == NULL) {
        printf("Erro ao abrir arquivo para salvar inodes.\n");
        return;
    }

    ListaINode *atual = listaInodes;
    while (atual != NULL) {
        fwrite(atual->inode, sizeof(INode), 1, file); // salva cada inode no arquivo
        atual = atual->next;
    }

    fclose(file);
    printf("Lista de inodes salva com sucesso.\n");
}

/*
    as funções terminalChar e proximaAcao são responsáveis por tratar o visual da bash
    terminalChar -> printa onde estamos no sistema
    proximaAcao -> separa a leitura em comando e destino, para facilitar a execução dos outros comandos
*/
// preciso trocar isso aqui pra funcionar com a fila de inodes
void terminalChar(char destino[]){
    printf("%s $ ", destino);
}

void proximaAcao(char comando[], char destino[], char leitura[]){
    char aux[50];
    scanf(" %[^\n]", &aux);
    strcpy(leitura, aux);

    // separa a string em tokens
    char *token = strtok(aux, " ");
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

void salvarMapaDeBits(const unsigned char *mapaDeBits, size_t tamanho) {
    FILE *file = fopen("./util/mapping/mapaDeBits.txt", "w");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo mapaDeBits.txt para escrita.\n");
        return;
    }

    for (size_t i = 0; i < tamanho * 8; i++) {
        // Escreve cada bit no arquivo
        fprintf(file, "%d", (mapaDeBits[i / 8] >> (i % 8)) & 1);
        if ((i + 1) % 8 == 0) {
            fprintf(file, " "); // Insere espaço entre bytes para legibilidade
        }
    }
    fprintf(file, "\n");

    fclose(file);
    printf("Mapa de bits salvo com sucesso em mapaDeBits.txt.\n");
}

void mostrarMapaDeBits(const unsigned char *mapaDeBits, size_t tamanho) {
    printf("Mapa de Bits:\n");
    for (size_t i = 0; i < tamanho * 8; i++) {
        // Imprime cada bit
        printf("%d", (mapaDeBits[i / 8] >> (i % 8)) & 1);
        if ((i + 1) % 8 == 0) printf(" ");
    }
    printf("\n");
}

/*
    Funções para tratar os blocos
*/
int alocarBloco(unsigned char *mapaDeBits) {
    for (int i = 0; i < totalBlocks; i++) {
        if (((mapaDeBits[i / 8] >> (i % 8)) & 1) == 0) { // verifica se ta livre
            mapaDeBits[i / 8] |= (1 << (i % 8)); // seta ocupado
            return i;
        }
    }
    return -1; //sem espaco
}

void liberarBloco(int indice, unsigned char *mapaDeBits) {
    if (indice >= 0 && indice < totalBlocks) {
        mapaDeBits[indice / 8] &= ~(1 << (indice % 8)); // seta livre
    }
}

int statusBloco(int indice, unsigned char *mapaDeBits) {
    if (indice >= 0 && indice < totalBlocks) {
        // Retorna o status do bloco (0 = livre, 1 = ocupado)
        return (mapaDeBits[indice / 8] & (1 << (indice % 8))) != 0;
    }
    return -1; // invalido
}


/*
    Comando cat, ler e escrever
*/
void escreverArquivo(char *arquivo_txt, ListaINode **listaInodes, unsigned char *mapaDeBits) {
    printf("Digite o texto para o arquivo (Ctrl+D para finalizar):\n");

    INode *inode = criarINode(arquivo_txt, "A");
    if (inode == NULL) {
        printf("Erro ao criar inode para o arquivo.\n");
        return;
    }

    char buffer[tfCluster];
    int bloco_index = 0;

    // le o texto do usuario e divide em blocos
    while (fgets(buffer, tfCluster, stdin)) {
        int bloco_id = alocarBloco(mapaDeBits);
        if (bloco_id == -1) {
            printf("Sem espaço disponível.\n");
            liberarBloco(bloco_id, mapaDeBits); // Desfaz a alocação em caso de erro
            return;
        }

        // salva no bloco
        Bloco bloco;
        memset(bloco.dados, 0, tfCluster);
        strncpy(bloco.dados, buffer, tfCluster);
        criarDAT(bloco, bloco_id);

        // linka com o inode
        inode->blocos[bloco_index++] = bloco;

        // verifica se ta lotado
        if (bloco_index >= totalBlocks) {
            printf("Limite de blocos do arquivo atingido.\n");
            break;
        }
    }

    // seta na lista
    ListaINode *novo = (ListaINode *)malloc(sizeof(ListaINode));
    novo->inode = inode;
    novo->next = *listaInodes;
    *listaInodes = novo;

    printf("Arquivo %s salvo com sucesso.\n", arquivo_txt);
}

void lerArquivo(char *arquivo_txt, ListaINode *listaInodes) {
    ListaINode *atual = listaInodes;

    // procura o inode do arquivo 
    while (atual != NULL) {
        if (strcmp(atual->inode->descricao, arquivo_txt) == 0) {
            INode *inode = atual->inode;

            printf("Conteudo do arquivo %s:\n", arquivo_txt);
            for (int i = 0; i < totalBlocks && inode->blocos[i].dados[0] != '\0'; i++) {
                printf("%s", inode->blocos[i].dados);
            }
            return;
        }
        atual = atual->next;
    }

    printf("Arquivo %s nao encontrado.\n", arquivo_txt);
}

void comandoCat(char *arquivo_txt, int escrever, ListaINode **listaInodes, unsigned char *mapaDeBits) {
    if (escrever) {
        escreverArquivo(arquivo_txt, listaInodes, mapaDeBits);
    } else {
        ListaINode *atual = *listaInodes;
        while (atual != NULL) {
            if (strcmp(atual->inode->descricao, arquivo_txt) == 0) {
                lerArquivo(arquivo_txt, *listaInodes);
                return;
            }
            atual = atual->next;
        }
        printf("Arquivo %s não encontrado.\n", arquivo_txt);
    }
}

/*
    Comando mkdir
*/
void comandoMkdir(char *nome_diretorio, ListaINode **listaInodes, unsigned char *mapaDeBits, ListaDiretorio **listaDiretorio) {
    INode *inode = criarINode(nome_diretorio, "D");
    if (inode == NULL) {
        printf("Erro ao criar inode para o diretório.\n");
        return;
    }

    ListaINode *novo = (ListaINode *)malloc(sizeof(ListaINode));
    novo->inode = inode;
    novo->next = *listaInodes;
    *listaInodes = novo;

    printf("Diretório %s criado com sucesso.\n", nome_diretorio);
}