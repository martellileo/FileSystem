#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Structs.h"

/*
    CORRIGIR:
    - conflitando o mapa de bits, ele nao cria ou inicializa se nao existir nada;
    - alterar o cd para funcionar;
    - used e free nao estao resetando com o sistema sendo reiniciado

    FAZER:
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

void reconstruirListaDeInodes(ListaINode **listaInodes, unsigned char *mapaDeBits) {
    for (int i = 0; i < totalBlocks; i++) {
        char nomeBloco[30];
        sprintf(nomeBloco, "util/blocos/bloco_%d.dat", i);

        FILE *file = fopen(nomeBloco, "rb");
        if (file != NULL) {
            Bloco bloco;
            fread(&bloco, sizeof(Bloco), 1, file);
            fclose(file);

            // Verificar se o bloco está ocupado
            if (strlen(bloco.dados) > 0) {
                mapaDeBits[i / 8] |= (1 << (i % 8));

                // Adicionar inode à lista
                INode *inode = criarINode(nomeBloco, "A");
                ListaINode *novo = (ListaINode *)malloc(sizeof(ListaINode));
                novo->inode = inode;
                novo->next = *listaInodes;
                *listaInodes = novo;
            }
        }
    }
}

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
            printf("Bloco |%zu| -> \t", i);
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

// salvar bloco em .dat
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

// init vlocos definidos pelo cluster
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

// // init mapa de bits, setando tudo livre
void inicializarMapaDeBits(unsigned char **mapaDeBits, size_t tamanho) {
    // Alocar espaço para o mapa de bits na memória
    *mapaDeBits = (unsigned char *)calloc(tamanho, sizeof(unsigned char)); // Inicializa com 0
    if (*mapaDeBits == NULL) {
        printf("Erro ao alocar memória para o mapa de bits.\n");
        return;
    }

    // Verifica se o arquivo do mapa de bits existe
    FILE *file = fopen("./util/mapaDeBits.txt", "r");
    if (file == NULL) {
        printf("Arquivo mapaDeBits.txt não encontrado. Criando um novo...\n");

        // Cria e inicializa o mapa de bits com todos os bits livres (0)
        salvarMapaDeBits(*mapaDeBits, tamanho); // Salva o novo mapa de bits no arquivo
    } else {
        printf("Carregando mapa de bits existente...\n");

        // Lê o conteúdo do arquivo para o mapa de bits na memória
        char linha[256];
        size_t bitIndex = 0;

        while (fgets(linha, sizeof(linha), file) != NULL && bitIndex < tamanho * 8) {
            for (int i = 0; linha[i] != '\0' && linha[i] != '\n'; i++) {
                if (linha[i] == '0' || linha[i] == '1') {
                    if (linha[i] == '1') {
                        // Define o bit correspondente no mapa
                        (*mapaDeBits)[bitIndex / 8] |= (1 << (bitIndex % 8));
                    }
                    bitIndex++;
                }
            }
        }

        fclose(file);

        // Verifica se o arquivo estava incompleto
        if (bitIndex < tamanho * 8) {
            printf("Aviso: Arquivo mapaDeBits.txt possui menos bits que o esperado (%zu/%zu). Completando com zeros...\n", bitIndex, tamanho * 8);
        }
    }
}


void salvarMapaDeBits(const unsigned char *mapaDeBits, size_t tamanho) {
    FILE *file = fopen("./util/mapaDeBits.txt", "w");
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

int alocarBloco(unsigned char *mapaDeBits) {
    for (int i = 0; i < totalBlocks; i++) {
        // Verifica se o bloco está livre (bit = 0)
        if ((mapaDeBits[i / 8] & (1 << (i % 8))) == 0) {
            // Marca como ocupado (bit = 1)
            mapaDeBits[i / 8] |= (1 << (i % 8));
            return i; // Retorna o índice do bloco alocado
        }
    }
    return -1; // Sem espaço disponível
}

void liberarBloco(int indice, unsigned char *mapaDeBits) {
    if (indice >= 0 && indice < totalBlocks) {
        // Marca o bloco como livre (bit = 0)
        mapaDeBits[indice / 8] &= ~(1 << (indice % 8));
    }
}

int statusBloco(int indice, unsigned char *mapaDeBits) {
    if (indice >= 0 && indice < totalBlocks) {
        // Retorna o status do bloco (0 = livre, 1 = ocupado)
        return (mapaDeBits[indice / 8] & (1 << (indice % 8))) != 0;
    }
    return -1; // Índice inválido
}

void escreverArquivo(char *arquivo_txt, ListaINode **listaInodes, unsigned char *mapaDeBits) {
    printf("Digite o texto para o arquivo (Ctrl+D para finalizar):\n");

    // Criação do inode
    INode *inode = criarINode(arquivo_txt, "A");
    if (inode == NULL) {
        printf("Erro ao criar inode para o arquivo.\n");
        return;
    }

    char buffer[tfCluster];
    int bloco_index = 0;

    // Ler entrada do usuário e salvar em blocos
    while (fgets(buffer, tfCluster, stdin)) {
        int bloco_id = alocarBloco(mapaDeBits);
        if (bloco_id == -1) {
            printf("Sem espaço disponível.\n");
            liberarBloco(bloco_id, mapaDeBits); // Desfaz a alocação em caso de erro
            return;
        }

        // Escrever os dados no bloco
        Bloco bloco;
        memset(bloco.dados, 0, tfCluster);
        strncpy(bloco.dados, buffer, tfCluster);
        criarDAT(bloco, bloco_id);

        // Associar o bloco ao inode
        inode->blocos[bloco_index++] = bloco;

        // Verifica se o inode está cheio
        if (bloco_index >= totalBlocks) {
            printf("Limite de blocos do arquivo atingido.\n");
            break;
        }
    }

    // Adicionar inode à lista
    ListaINode *novo = (ListaINode *)malloc(sizeof(ListaINode));
    novo->inode = inode;
    novo->next = *listaInodes;
    *listaInodes = novo;

    printf("Arquivo %s salvo com sucesso.\n", arquivo_txt);
}

void lerArquivo(char *arquivo_txt, ListaINode *listaInodes) {
    ListaINode *atual = listaInodes;

    // Procurar o inode correspondente
    while (atual != NULL) {
        if (strcmp(atual->inode->descricao, arquivo_txt) == 0) {
            INode *inode = atual->inode;

            printf("Conteúdo do arquivo %s:\n", arquivo_txt);
            for (int i = 0; i < totalBlocks && inode->blocos[i].dados[0] != '\0'; i++) {
                printf("%s", inode->blocos[i].dados);
            }
            return;
        }
        atual = atual->next;
    }

    printf("Arquivo %s não encontrado.\n", arquivo_txt);
}

void comandoCat(char *arquivo_txt, int escrever, ListaINode **listaInodes, unsigned char *mapaDeBits) {
    if (escrever) {
        escreverArquivo(arquivo_txt, listaInodes, mapaDeBits);
    } else {
        lerArquivo(arquivo_txt, *listaInodes);
    }
}
