/*
TRABALHO 3:
    -VICTOR MARIANO ROCHA
    -PATRICK PERETE
*/
#include "header.h"

int main() {
    FILE *arquivo;

/* Registros a serem inseridos */

    arquivo = verificaArquivo("insere.bin");
    REGISTRO registrosInseridos[13];
    fread(registrosInseridos, sizeof(REGISTRO), 13, arquivo);
    fclose(arquivo);

/* Registros que serão pesquisados */

    arquivo = verificaArquivo("busca.bin");
    CHAVE_BUSCA buscaIndice[13];
    fread(buscaIndice, sizeof(CHAVE_BUSCA), 13, arquivo);
    fclose(arquivo);

/* Variáveis para usar posteriormente */

    arq_registro = fopen("arq_registro.bin", "a+b");
    BTPAGE pagina;
    int rrnRaiz;

    fclose(arq_registro);

/* Obtendo raiz || Criando índice */

    if ((indice = fopen("indice.bin", "r+b")) > 0) {
        rrnRaiz = recuperarRaiz();
    } else {        
        rrnRaiz = criarArvore();
    }
    fclose(indice);


    int opcao, i;

    do
    {
        opcao = menu_inicial();

        /* Rotina de cada opção */
        switch (opcao)
        {
            /* Inserção */
            case 1:
                while (1)
                {
                    printf("\nDigite '0' para sair.");
                    printf("\nInforme um número de 1 a 10: ");
                    scanf("%d", &i);

                    if (i == 0)
                        break;
                    if (i < 1 || i > 15)
                    {
                        printf("Opção inválida!");
                    }                    
                    else
                    {                        
                        bool foiPromovido;
                        int rrnPromovido;                    
                        CHAVE_BUSCA chavePrimaria;
                        strcpy(chavePrimaria.id_aluno, registrosInseridos[i-1].id_aluno);
                        strcpy(chavePrimaria.sigla_disc, registrosInseridos[i-1].sigla_disc);
                        CHAVE_PAGINA chavePromovida, chave;
                        chave.id = chavePrimaria;

                        indice = verificaArquivo("indice.bin");                        
                        int offSet = buscaRegistroArvore(rrnRaiz, chave);

                        if(offSet == (-1)) {
                            arq_registro = verificaArquivo("arq_registro.bin");
                            int offSet = insereRegistro(registrosInseridos[i-1]);
                            fclose(arq_registro);
                            chave.rrn = offSet;

                            foiPromovido = insereArvore(rrnRaiz, chave, &rrnPromovido, &chavePromovida);
                            if (foiPromovido)
                                rrnRaiz = criarRaiz(chavePromovida, rrnRaiz, rrnPromovido);
                        } else {
                            printf("-----Chave duplicada: %s%s\n", chave.id.id_aluno, chave.id.sigla_disc);
                        }
                        fclose(indice);
                    }
                }
                break;

            /* Busca */
            case 2:
                while (1)
                {
                    printf("\nDigite '0' para sair.");
                    printf("\nInforme um número de 1 a 4: ");
                    scanf("%d", &i);

                    if (i == 0)
                        break;
                    if (i < 1 || i > 4)
                    {
                        printf("Opção inválida!");
                    }                    
                    else
                    {                       
                        CHAVE_BUSCA chavePrimaria;
                        strcpy(chavePrimaria.id_aluno, buscaIndice[i-1].id_aluno);
                        strcpy(chavePrimaria.sigla_disc, buscaIndice[i-1].sigla_disc);
                        CHAVE_PAGINA chavePagina;
                        chavePagina.id = chavePrimaria;
                        
                        indice = verificaArquivo("indice.bin");
                        
                        int offSet = buscaRegistroArvore(rrnRaiz, chavePagina);
                        fclose(indice);

                        if(offSet != (-1)) {
                            arq_registro = verificaArquivo("arq_registro.bin");
                            buscaRegistroRRN(offSet, arq_registro);
                            fclose(arq_registro);
                        } else {
                            printf("--------Chave não encontrada no índice!\n");
                        }
                    }
                }
                break;

            /* Listagem */
            case 3:
                printf("\nListando todos os alunos.\n\n");
                indice = verificaArquivo("indice.bin");  
                arq_registro = verificaArquivo("arq_registro.bin");                            
                imprimeArvore(recuperarRaiz());
                printf("\n");
                fclose(arq_registro);
                fclose(indice);
                break;

            case 0:
                return 0;
                break;

            default:
                break;
        }
    } while (opcao != 0);
}

//Verifica se um arquivo existe
FILE* verificaArquivo(char *arquivo) {
    FILE *fp = fopen(arquivo, "r+b");

    if (fp == NULL) {
        printf("O arquivo %s não existe.", arquivo);
        exit(0);
    }
    return fp;
}

//Mostra opções do menu
int menu_inicial()
{
    int opcao;

    do {
        printf("\n----------------Menu------------------\n");
        printf("(1) Inserir aluno.\n");
        printf("(2) Buscar por um aluno.\n");
        printf("(3) Listar os dados de todos os alunos.\n");
        printf("(0) Sair do programa.\n");
        printf("\n------------------------------------------\n");
        printf("Opção: ");
        scanf("%d", &opcao);
    } while (opcao != 1 && opcao != 2 && opcao != 3 && opcao != 0);

    return opcao;
}

//Insere um Registro no arquivo de registro
int insereRegistro(REGISTRO novoRegistro) {       
    char registro[130];
    sprintf(registro, "%s#%s#%s#%s#%.2f#%.2f#", novoRegistro.id_aluno, novoRegistro.sigla_disc, novoRegistro.nome_aluno, novoRegistro.nome_disc, novoRegistro.media, novoRegistro.freq );

    int tamRegistro = strlen(registro);
    
    fseek(arq_registro, 0, SEEK_END);
    int offSet = ftell(arq_registro);    

    fwrite(&tamRegistro, sizeof(int), 1, arq_registro);    
    fwrite(registro, sizeof(char), tamRegistro, arq_registro);
    return offSet;
}

// cria o arquivo da arvore-B
int criarArvore() {
    char chave;
    indice = fopen("indice.bin", "w+b"); 
    
    fseek(indice, 0, SEEK_SET);
    int header = -1;
    fwrite(&header, sizeof(int), 1, indice);

    return header;
}

// cria raiz
int criarRaiz(CHAVE_PAGINA chave, int esquerda, int direita) {
    BTPAGE page;
    int rrn = quantidadePagina();
    iniciarPagina(&page);
    page.chaves[0] = chave;
    page.filhos[0] = esquerda;
    page.filhos[1] = direita;
    page.quantidadeNos = 1;
    escrevePagina(rrn, &page);
    atualizarIndice(rrn);
    return(rrn);
}

//Insere o indice na Árvore-B
bool insereArvore (int rrn, CHAVE_PAGINA proximaChave, int *rrnPromovido, CHAVE_PAGINA *chavePromovida) {
    BTPAGE paginaAtual, novaPagina;
    bool encontrado, promovido;   
    int posicao, rrnPromovidoDeBaixo;
    CHAVE_PAGINA chavePromovidaDeBaixo;

    // Está em uma folha da árvore
    if (rrn == NIL) {
        *chavePromovida = proximaChave;
        *rrnPromovido = NIL;
        printf("----- Chave Inserida: %s%s\n", proximaChave.id.id_aluno, proximaChave.id.sigla_disc);
        return true;
    }

    // Lê a página e procura pela chave a ser inserida
    lePagina(rrn, &paginaAtual);
    encontrado = buscarNo (proximaChave, &paginaAtual, &posicao);
    if (encontrado) {
        
        return false;
    }

    // Recursão para a página filha
    promovido = insereArvore(paginaAtual.filhos[posicao], proximaChave, &rrnPromovidoDeBaixo, &chavePromovidaDeBaixo);

    // Não houve recursão
    if (!promovido) {
        return false;
    }

    // Inserção na página atual, pois ainda há espaço livre
    if(paginaAtual.quantidadeNos < MAXKEYS) {
        inserePagina(chavePromovidaDeBaixo, rrnPromovidoDeBaixo, &paginaAtual);
        escrevePagina(rrn, &paginaAtual);
        return false;
    } else { // Mas caso não haja espaço, é feito o split
        split(chavePromovidaDeBaixo, rrnPromovidoDeBaixo, &paginaAtual, chavePromovida, rrnPromovido, &novaPagina);
        escrevePagina(rrn, &paginaAtual);
        escrevePagina(*rrnPromovido, &novaPagina);
        return true;
    }
}

// inicializa pagina vazia
void iniciarPagina(BTPAGE *pagina) {
    int j;

    for (j = 0; j < MAXKEYS; j++){
        pagina->chaves[j] = criaNo();
        pagina->filhos[j] = NIL;
    }

    pagina->filhos[MAXKEYS] = NIL;
}

//Insere registro na página
void inserePagina(CHAVE_PAGINA chave, int rrnPromovido, BTPAGE *pagina) {
    int j;
        
    for(j = pagina->quantidadeNos; compararChaves(chave, pagina->chaves[j-1]) < 0 && j > 0; j--){
        pagina->chaves[j] = pagina->chaves[j-1];
        pagina->filhos[j+1] = pagina->filhos[j];
    }

    pagina->quantidadeNos++;
    pagina->chaves[j] = chave;
    pagina->filhos[j+1] = rrnPromovido;
}

//Busca um registro na árvore
int buscaRegistroArvore(int rrn, CHAVE_PAGINA chave) {
    BTPAGE paginaAtual;
    bool encontrado;         
    int posicao;

    if (rrn == NIL) {        
        return -1;
    }
    
    lePagina(rrn, &paginaAtual);    
    encontrado = buscarNo(chave, &paginaAtual, &posicao);
    
    if (encontrado) {
        return paginaAtual.chaves[posicao].rrn;
    }

    return buscaRegistroArvore(paginaAtual.filhos[posicao], chave);
}

//Busca um registro em específico
void buscaRegistroRRN(int rrnArquivoResultado, FILE* arquivo) {
    int tamanhoRegistro;
    char registroBuscado[130];                            
    fseek(arquivo, rrnArquivoResultado, SEEK_SET);
    fread(&tamanhoRegistro, sizeof(int), 1, arquivo);
    fread(registroBuscado, sizeof(char), tamanhoRegistro, arquivo);
    registroBuscado[tamanhoRegistro] = '\0';
    puts(registroBuscado);
}

//Busca um nó na árvore
bool buscarNo(CHAVE_PAGINA chave, BTPAGE *pagina, int *posicao) {
    int i;
    for (i = 0; i < pagina->quantidadeNos && compararChaves(chave, pagina->chaves[i]) > 0; i++);
    *posicao = i;
    return (*posicao < pagina->quantidadeNos && compararChaves(chave, pagina->chaves[*posicao]) == 0);
}

//Atualiza a raiz da árvore
int recuperarRaiz() {
    int rrnRaiz;
    fseek(indice, 0, SEEK_SET); 
    fread(&rrnRaiz, sizeof(int), 1, indice);    
    return rrnRaiz;
}

//Compara chaves da árvore
int compararChaves(CHAVE_PAGINA chave1, CHAVE_PAGINA chave2) {
    char id1[20], id2[20];
    sprintf(id1, "%s%s", chave1.id.id_aluno, chave1.id.sigla_disc);
    sprintf(id2, "%s%s", chave2.id.id_aluno, chave2.id.sigla_disc);

    return strcmp(id1, id2);
}

//Faz a divisão de uma árvore em uma sub-árvore
void split(CHAVE_PAGINA chave, int filhoDireita, BTPAGE *paginaDividida, CHAVE_PAGINA *chavePromovida, int *filhoDireitaChavePromovida, BTPAGE *novaPagina) {
    printf("Divisão de Nó\n");
    
    int j;
    CHAVE_PAGINA auxChaves[MAXKEYS+1];
    int auxFilhos[MAXKEYS+2];

    for (j = 0; j < MAXKEYS; j++){
        auxChaves[j] = paginaDividida->chaves[j];
        auxFilhos[j] = paginaDividida->filhos[j];
    }
    auxFilhos[MAXKEYS] = paginaDividida->filhos[MAXKEYS];

    // Deslocando as chaves para inserir uma nova
    for (j = MAXKEYS; (compararChaves(chave, auxChaves[j-1]) < 0) && j > 0; j--){
        auxChaves[j] = auxChaves[j-1];
        auxFilhos[j+1] = auxFilhos[j];
    }
    auxChaves[j] = chave;
    auxFilhos[j+1] = filhoDireita;

    *filhoDireitaChavePromovida = quantidadePagina();    
    iniciarPagina(novaPagina);

    for (j = 0; j < MINKEYS; j++){
        paginaDividida->chaves[j] = auxChaves[j]; // posições: 0 e 1
        paginaDividida->filhos[j] = auxFilhos[j]; // posições: 0 e 1

        novaPagina->chaves[j] = auxChaves[j + MINKEYS]; // posições: 2 e 3
        novaPagina->filhos[j] = auxFilhos[j + MINKEYS]; // posições: 2 e 3

        if((j + MINKEYS) < MAXKEYS) {
            paginaDividida->chaves[j + MINKEYS] = criaNo(); // posições: 2 e 3
        }
        paginaDividida->filhos[j + MINKEYS] = NIL; // posições: 2 e 3
    }
    novaPagina->filhos[MINKEYS] = auxFilhos[j + MINKEYS]; // posição: 4
        
    paginaDividida->chaves[1] = criaNo();

    novaPagina->quantidadeNos = (MAXKEYS + 1) - MINKEYS;
    paginaDividida->quantidadeNos = MINKEYS - 1;

    *chavePromovida = auxChaves[1];

    printf("Chave Promovida: %s%s\n", chavePromovida->id.id_aluno, chavePromovida->id.sigla_disc);
}

//Imprime os registros da árvore em ordem
void imprimeArvore(int rrn) {
    BTPAGE paginaAtual;

    lePagina(rrn, &paginaAtual);

    // Se a página for uma folha
    if (paginaAtual.filhos[0] == NIL) {
        // Percorre as chaves da página e imprime os registros associados a essas chaves
        for (int i = 0; i < paginaAtual.quantidadeNos; i++) {
            buscaRegistroRRN(paginaAtual.chaves[i].rrn, arq_registro);
        }
        // Retorna da função, encerrando a execução para esse nó
        return;
    }
    else {
        // Se a página não for uma folha, chama recursivamente a função para o primeiro filho
        imprimeArvore(paginaAtual.filhos[0]);
    }

    // Percorre as chaves da página e chama recursivamente a função para os filhos
    for (int i = 0; i < paginaAtual.quantidadeNos; i++) {
        // Imprime o registro associado à chave na posição i
        buscaRegistroRRN(paginaAtual.chaves[i].rrn, arq_registro);
        // Chama a função recursivamente para o filho da direita da chave i
        imprimeArvore(paginaAtual.filhos[i + 1]);
    }
}

int quantidadePagina() {
    long addr; 
    fseek(indice, 0, SEEK_END);
    addr = ftell(indice) - sizeof(int);
    
    return ((int) addr / PAGESIZE);
}

// Lê a página e procura pela chave a ser inserida
void lePagina(int rrn, BTPAGE *pagina) {
    long addr;
    addr = (long)rrn * (long)PAGESIZE + sizeof(int);
    fseek(indice, addr, SEEK_SET);
    fread(pagina, sizeof(BTPAGE), 1, indice);
}

//Escreve o indice na página 
void escrevePagina(int rrn, BTPAGE *pagina) {
    long addr;
    addr = (long)rrn * (long)PAGESIZE + sizeof(int);
    fseek(indice, addr, SEEK_SET);
    fwrite(pagina, sizeof(BTPAGE), 1, indice);
}

//Atualiza o indice
void atualizarIndice(int rrnRaiz) {
    fseek(indice, 0, SEEK_SET); 
    fwrite(&rrnRaiz, sizeof(int), 1, indice);
}

CHAVE_PAGINA criaNo() {
    CHAVE_PAGINA noKey;
    CHAVE_BUSCA nullKey;
    char nullableKey[4] = "@@@\0"; //OBS
    char nullableKey2[4] = "###\0";
    strcpy(nullKey.id_aluno, nullableKey);
    strcpy(nullKey.sigla_disc, nullableKey2);

    noKey.id = nullKey;
    noKey.rrn = -1;
    
    return noKey;
}


