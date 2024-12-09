
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



#define MAXKEYS 3
#define MINKEYS 2
#define NIL (-1)
#define PAGESIZE sizeof(BTPAGE)


typedef struct {
    char id_aluno[4];
    char sigla_disc[4];
    char nome_aluno[50];
    char nome_disc[50];
    float media;
    float freq;
} REGISTRO;

typedef struct {
    char id_aluno[4];
    char sigla_disc[4];
} CHAVE_BUSCA;

typedef struct 
{
    CHAVE_BUSCA id;
    int rrn;
} CHAVE_PAGINA;



typedef struct {
    int quantidadeNos;
    CHAVE_PAGINA chaves[MAXKEYS];
    int filhos[MAXKEYS+1];
} BTPAGE;

FILE* verificaArquivo(char *arquivo);

int menu_inicial();
int insereRegistro(REGISTRO novoRegistro);

int criarArvore();
int criarRaiz(CHAVE_PAGINA chave, int esquerda, int direita);
bool insereArvore (int rrn, CHAVE_PAGINA proximaChave, int *rrnPromovido, CHAVE_PAGINA *chavePromovida);


void iniciarPagina(BTPAGE *pagina);
void inserePagina(CHAVE_PAGINA chave, int rrnPromovido, BTPAGE *pagina);

int buscaRegistroArvore(int rrn, CHAVE_PAGINA chave);
void buscaRegistroRRN(int buscaRegistroRRN, FILE* arquivo);
bool buscarNo (CHAVE_PAGINA chave, BTPAGE *pagina, int *posicao);
int recuperarRaiz();


int compararChaves(CHAVE_PAGINA chave1, CHAVE_PAGINA chave2);
void split(CHAVE_PAGINA chave, int filhoDireita, BTPAGE *paginaDividida, CHAVE_PAGINA *chavePromovida, int *filhoDireitaChavePromovida, BTPAGE *novaPagina);
void imprimeArvore(int rrn);
int quantidadePagina();
void lePagina(int rrn, BTPAGE *pagina);
void escrevePagina(int rrn, BTPAGE *pagina);
void atualizarIndice(int rrnRaiz);
CHAVE_PAGINA criaNo();


FILE* indice;
FILE* arq_registro;