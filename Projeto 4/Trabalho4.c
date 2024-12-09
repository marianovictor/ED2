/*
    Victor Mariano Rocha
    Patrick Perete
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSO 0
#define VERDADEIRO 1
#define TAMANHO_HASH 13
#define VAZIO_FALSO 0
#define VAZIO_VERDADEIRO 1
#define OCUPADO 2

typedef struct {
    char id_aluno[4];
    char sigla_disc[4];
    char nome_aluno[50];
    char nome_disc[50];
    float media;
    float freq;
} s_registro;

typedef struct {
    char id_aluno[4];
    char sigla_disc[4];
} s_chave;

typedef struct {
    int posicao_insere, 
    posicao_remocao, 
    posicao_busca;
} s_cabecalho;

typedef struct {
    int status, 
    chave, 
    offset; 
} s_endereco;

s_registro * carrega_insere(void);
s_chave * carrega_remove(void);
s_chave * carrega_busca(void);
void carrega_dados(void);
void carrega_hash(void);
void insercao(s_registro *registro);
int transforma_chave(char *codigo_cliente, char *codigo_filme);
int busca_duplicado(int chave);
void remocao(s_chave *chave_remocao);
void busca(s_chave *chave_busca);
int menu_inicial();

int main(void)
{
    
    s_chave *chave_busca, *chave_remocao;
    s_registro *registro;
    int opcao;
    printf("Carregando arquivos...\n");
    registro = carrega_insere();
    chave_remocao = carrega_remove();
    chave_busca = carrega_busca();
    carrega_dados();
    carrega_hash();
    printf("Arquivos carregados\n\n");

    do {
        opcao = menu_inicial();
        switch(opcao) {
            case 1:
                insercao(registro);
                break;
            case 2:
                busca(chave_busca);
                break;
            case 3:
                remocao(chave_remocao);
                
                break;
            case 0:
                break;
            default:
                printf("ERRO! Opção inválida\n\n");
        }
    } while(opcao != 0);

    free(registro);
    free(chave_remocao);
    free(chave_busca);

    return 0;
}
int menu_inicial()
{
    int opcao;

    do {
        printf("\n----------------Menu------------------\n");
        printf("(1) Inserir um aluno.\n");
        printf("(2) Buscar por um aluno.\n");
        printf("(3) Remover um aluno.\n");
        printf("(0) Sair do programa.\n");
        printf("\n------------------------------------------\n");
        printf("Opção: ");
        scanf("%d", &opcao);
    } while (opcao != 1 && opcao != 2 && opcao != 3 && opcao != 0);

    return opcao;
}
s_registro * carrega_insere(void) {
    int tamanho = 0;
    FILE *arquivo_insere;
    s_registro *registro, registro_auxiliar;

    if((arquivo_insere = fopen("insere.bin", "rb")) == NULL) {
        printf("ERRO! - ao abrir o arquivo insere.bin\n\n");
        return NULL;
    }

    while(fread(&registro_auxiliar, sizeof(s_registro), 1, arquivo_insere)) {
        tamanho++;
    }

    registro = malloc(tamanho*sizeof(s_registro));
    fseek(arquivo_insere, 0, 0);
    fread(registro, sizeof(s_registro), tamanho, arquivo_insere);

    fclose(arquivo_insere);

    return registro;
}

s_chave * carrega_remove(void) {
    int tamanho = 0;
    FILE *arquivo_remove;
    s_chave *chave, chave_auxiliar;

    if((arquivo_remove = fopen("remove.bin", "rb")) == NULL) {
        printf("ERRO! - Arquivo\n\n");
        return NULL;
    }

    while(fread(&chave_auxiliar, sizeof(s_chave), 1, arquivo_remove)) {
        tamanho++;
    }

    chave = malloc(tamanho*sizeof(s_chave));
    fseek(arquivo_remove, 0, 0);
    fread(chave, sizeof(s_chave), tamanho, arquivo_remove);

    fclose(arquivo_remove);

    return chave;
}

s_chave * carrega_busca(void) {
    int tamanho = 0;
    FILE *arquivo_busca;
    s_chave *chave, chave_auxiliar;

    if((arquivo_busca = fopen("busca.bin", "rb")) == NULL) {
        printf("ERRO! - ao abrir o arquivo busca.bin\n\n");
        return NULL;
    }

    while(fread(&chave_auxiliar, sizeof(s_chave), 1, arquivo_busca)) {
        tamanho++;
    }

    chave = malloc(tamanho*sizeof(s_chave));
    fseek(arquivo_busca, 0, 0);
    fread(chave, sizeof(s_chave), tamanho, arquivo_busca);

    fclose(arquivo_busca);

    return chave;
}

void carrega_dados(void) {
    FILE *arquivo_dados;
    s_cabecalho cabecalho = {0, 0, 0};

    if((arquivo_dados = fopen("arq_registros.bin", "rb")) == NULL) {
        if((arquivo_dados = fopen("arq_registros.bin", "wb")) == NULL) {
            printf("ERRO! - ao abrir o arquivo arq_registros.bin\n\n");

            return;
        }

        fwrite(&cabecalho, sizeof(s_cabecalho), 1, arquivo_dados);
        fflush(stdin);
    }

    fclose(arquivo_dados);

    return;
}

void carrega_hash(void) {
    FILE *arquivo_hash;
    s_endereco endereco = {VAZIO_VERDADEIRO, -1, -1};

    if((arquivo_hash = fopen("hash.bin", "rb")) == NULL) {
        if((arquivo_hash = fopen("hash.bin", "wb")) == NULL) {
            printf("ERRO! - ao abrir o arquivo hash.bin\n\n");

            return;
        }

        for(int i = 0; i < TAMANHO_HASH; i++) {
            fwrite(&endereco, sizeof(s_endereco), 1, arquivo_hash);
        }

        fflush(stdin);
    }

    fclose(arquivo_hash);

    return;
}

void insercao(s_registro *registro) {
    int chave, colisao = FALSO, contador = 0, duplicado, endereco_casa, offset, tentativa = 0;
    FILE *arquivo_dados, *arquivo_hash;
    s_cabecalho cabecalho;
    s_endereco endereco;

    if((arquivo_dados = fopen("arq_registros.bin", "r+b")) == NULL) {
        printf("ERRO! - ao abrir o arquivo arq_registros.bin\n\n");
        return;
    }

    if((arquivo_hash = fopen("hash.bin", "r+b")) == NULL) {
        printf("ERRO! - ao abrir o arquivo hash.bin\n\n");
        return;
    }

    fread(&cabecalho, sizeof(s_cabecalho), 1, arquivo_dados);
    fseek(arquivo_dados, 0, 2);
    offset = ftell(arquivo_dados);

    printf("%s%s\n", registro[cabecalho.posicao_insere].id_aluno, registro[cabecalho.posicao_insere].sigla_disc);
    chave = transforma_chave(registro[cabecalho.posicao_insere].id_aluno, registro[cabecalho.posicao_insere].sigla_disc);
    //printf("(%d)\n", chave);

    if(chave == 0){
        printf("Chave não encontrada\n\n");
        return;
    }
    endereco_casa = chave%TAMANHO_HASH;
    printf("Endereco %d\n", endereco_casa);

    fseek(arquivo_hash, endereco_casa*sizeof(s_endereco), 0);

    do {
        fread(&endereco, sizeof(s_endereco), 1, arquivo_hash);

        if(endereco.status == VAZIO_FALSO || endereco.status == VAZIO_VERDADEIRO) {
            if(tentativa > 0) {
                printf("Tentativa %d\n", tentativa);
            }

            endereco_casa = endereco_casa+contador;
            break;
        }

        if(endereco.status == OCUPADO && colisao == FALSO) {
            printf("Colisao\n");
            colisao = VERDADEIRO;
        }

        contador++;
        tentativa++;

        if(endereco_casa+contador == TAMANHO_HASH) {
            contador = 0;
            endereco_casa = 0;
            fseek(arquivo_hash, 0, 0);
        }
    } while(tentativa < TAMANHO_HASH);

    duplicado = busca_duplicado(chave);

    if(duplicado) {
        printf("Chave %d duplicada\n\n", chave);
        return;
    }
    else if(tentativa == TAMANHO_HASH) {
        printf("Indice hash cheio\n\n");
        return;
    }   
    else {
        endereco.status = OCUPADO;
        endereco.chave = chave;
        endereco.offset = offset;
        fseek(arquivo_hash, endereco_casa*sizeof(s_endereco), 0);
        fwrite(&endereco, sizeof(s_endereco), 1, arquivo_hash);
        fwrite(&registro[cabecalho.posicao_insere].id_aluno, sizeof(char), 4, arquivo_dados);
        fwrite(&registro[cabecalho.posicao_insere].sigla_disc, sizeof(char), 4, arquivo_dados);
        fwrite(&registro[cabecalho.posicao_insere].nome_aluno, sizeof(char), 50, arquivo_dados);
        fwrite(&registro[cabecalho.posicao_insere].nome_disc, sizeof(char), 50, arquivo_dados);
        fwrite(&registro[cabecalho.posicao_insere].media, sizeof(float), 1, arquivo_dados);
        fwrite(&registro[cabecalho.posicao_insere].freq, sizeof(float), 1, arquivo_dados);
        printf("Chave %s%s inserida com sucesso\n\n", registro[cabecalho.posicao_insere].id_aluno, registro[cabecalho.posicao_insere].sigla_disc);
    }

    (cabecalho.posicao_insere)++;
    fseek(arquivo_dados, 0, 0);
    fwrite(&cabecalho, sizeof(s_cabecalho), 1, arquivo_dados);
    fflush(stdin);

    fclose(arquivo_dados);
    fclose(arquivo_hash);

    return;
}


int transforma_chave(char *id_aluno, char *sigla_disc) {
    
    char id_aluno_digla_disc[8] = "\0";
    int chave_transformada;
    

    strcat(id_aluno_digla_disc, id_aluno);
    strcat(id_aluno_digla_disc, sigla_disc);
    //printf("Chave antes de transf: %s\n", id_aluno_digla_disc);
    chave_transformada = atoi(id_aluno_digla_disc);
    //printf("Chave depois de transf: %d\n", chave_transformada);
    return chave_transformada;
}

int busca_duplicado(int chave) {
    int contador = 0, duplicado = FALSO, endereco_casa, tentativa = 0;
    FILE *arquivo_hash;
    s_endereco endereco;

    if((arquivo_hash = fopen("hash.bin", "rb")) == NULL) {
        printf("ERRO! - ao abrir o arquivo hash.bin\n\n");
        return 0;
    }

    endereco_casa = chave%TAMANHO_HASH;
    fseek(arquivo_hash, endereco_casa*sizeof(s_endereco), 0);

    do {
        fread(&endereco, sizeof(s_endereco), 1, arquivo_hash);

        if(chave == endereco.chave) {
            duplicado = VERDADEIRO;
            break;
        }

        contador++;
        tentativa++;

        if(endereco_casa+contador == TAMANHO_HASH) {
            contador = 0;
            endereco_casa = 0;
            fseek(arquivo_hash, 0, 0);
        }
    } while(endereco.status != VAZIO_VERDADEIRO && tentativa < TAMANHO_HASH);

    fclose(arquivo_hash);

    return duplicado;
}

void remocao(s_chave *chave_remocao) {
    int ausente = VERDADEIRO, chave, contador = 0, endereco_casa, tentativa = 0;
    FILE *arquivo_dados, *arquivo_hash;
    s_cabecalho cabecalho ;
    s_endereco endereco;

    if((arquivo_dados = fopen("arq_registros.bin", "r+b")) == NULL) {
        printf("ERRO! - ao abrir o arquivo arq_registros.bin\n\n");
        return;
    }

    if((arquivo_hash = fopen("hash.bin", "r+b")) == NULL) {
        printf("ERRO! - ao abrir o arquivo hash.bin\n\n");
        return;
    }

    fread(&cabecalho, sizeof(cabecalho), 1, arquivo_dados);

    printf("%s%s\n", chave_remocao[cabecalho.posicao_remocao].id_aluno, chave_remocao[cabecalho.posicao_remocao].sigla_disc);
    chave = transforma_chave(chave_remocao[cabecalho.posicao_remocao].id_aluno, chave_remocao[cabecalho.posicao_remocao].sigla_disc);
    //printf("(%d)\n", chave);

    endereco_casa = chave%TAMANHO_HASH;

    fseek(arquivo_hash, endereco_casa*sizeof(s_endereco), 0);

    do {
        fread(&endereco, sizeof(s_endereco), 1, arquivo_hash);

        if(chave == endereco.chave) {
            ausente = FALSO;
            endereco_casa = endereco_casa+contador;
            break;
        }

        contador++;
        tentativa++;

        if(endereco_casa+contador == TAMANHO_HASH) {
            contador = 0;
            endereco_casa = 0;
            fseek(arquivo_hash, 0, 0);
        }
    } while(endereco.status != VAZIO_VERDADEIRO && tentativa < TAMANHO_HASH);

    if(ausente) {
        printf("Chave %s%s nao encontrada\n\n", chave_remocao[cabecalho.posicao_remocao].id_aluno, chave_remocao[cabecalho.posicao_remocao].sigla_disc);
    }
    else {
        if(tentativa+1 == 1) {
            printf("Chave %s%s encontrada, endereco %d, %d acesso\n", chave_remocao[cabecalho.posicao_remocao].id_aluno, chave_remocao[cabecalho.posicao_remocao].sigla_disc, endereco_casa, tentativa+1);
        }
        else {
            printf("Chave %s%s encontrada, endereco %d, %d acessos\n", chave_remocao[cabecalho.posicao_remocao].id_aluno, chave_remocao[cabecalho.posicao_remocao].sigla_disc, endereco_casa, tentativa+1);
        }

        endereco.status = VAZIO_FALSO;
        endereco.chave = -1;
        endereco.offset = -1;
        fseek(arquivo_hash, endereco_casa*sizeof(s_endereco), 0);
        fwrite(&endereco, sizeof(s_endereco), 1, arquivo_hash);
        printf("Chave %s%s removida com sucesso\n\n", chave_remocao[cabecalho.posicao_remocao].id_aluno, chave_remocao[cabecalho.posicao_remocao].sigla_disc);
    }

    (cabecalho.posicao_remocao)++;
    fseek(arquivo_dados, 0, 0);
    fwrite(&cabecalho, sizeof(s_cabecalho), 1, arquivo_dados);
    fflush(stdin);

    fclose(arquivo_dados);
    fclose(arquivo_hash);

    return;
}

void busca(s_chave *chave_busca) {
    int ausente = VERDADEIRO, chave, contador = 0, endereco_casa, tentativa = 0;
    FILE *arquivo_dados, *arquivo_hash;
    s_cabecalho cabecalho;
    s_endereco endereco;
    s_registro registro;
    //printf("Cabeçalho: %d\n", cabecalho.posicao_busca);
    if((arquivo_dados = fopen("arq_registros.bin", "r+b")) == NULL) {
        printf("ERRO! - ao abrir o arquivo arq_registros.bin\n\n");
        return;
    }

    if((arquivo_hash = fopen("hash.bin", "rb")) == NULL) {
        printf("ERRO! - ao abrir o arquivo hash.bin\n\n");
        return;
    }

    fread(&cabecalho, sizeof(cabecalho), 1, arquivo_dados);
    
    if((strlen(chave_busca[cabecalho.posicao_busca].id_aluno) == 0) || (strlen(chave_busca[cabecalho.posicao_busca].sigla_disc) == 0)){
        printf("Não existe mais chaves para busca\n");
        
        return;
    }
    fflush(stdin);
    //printf("\"%s\"\n ", chave_busca[cabecalho.posicao_busca].id_aluno);
    //printf("\"%s\"\n ", chave_busca[cabecalho.posicao_busca].sigla_disc);
    
    printf("%s%s\n", chave_busca[cabecalho.posicao_busca].id_aluno, chave_busca[cabecalho.posicao_busca].sigla_disc);
    chave = transforma_chave(chave_busca[cabecalho.posicao_busca].id_aluno, chave_busca[cabecalho.posicao_busca].sigla_disc);
    //printf("(%d)\n", chave);

    endereco_casa = chave%TAMANHO_HASH;

    fseek(arquivo_hash, endereco_casa*sizeof(s_endereco), 0);

    do {
        fread(&endereco, sizeof(s_endereco), 1, arquivo_hash);

        if(chave == endereco.chave) {
            ausente = FALSO;
            endereco_casa = endereco_casa+contador;
            fseek(arquivo_dados, endereco.offset, 0);
            fread(&registro, sizeof(s_registro), 1, arquivo_dados);
            break;
        }

        contador++;
        tentativa++;

        if(endereco_casa+contador == TAMANHO_HASH) {
            contador = 0;
            endereco_casa = 0;
            fseek(arquivo_hash, 0, 0);
        }
    } while(endereco.status != VAZIO_VERDADEIRO && tentativa < TAMANHO_HASH);

    if(ausente) {
        printf("Chave %s%s nao encontrada\n\n", chave_busca[cabecalho.posicao_busca].id_aluno, chave_busca[cabecalho.posicao_busca].sigla_disc);
    }
    else {
        if(tentativa+1 == 1) {
            printf("Chave %s%s encontrada, endereco %d, %d acesso\n", chave_busca[cabecalho.posicao_busca].id_aluno, chave_busca[cabecalho.posicao_busca].sigla_disc, endereco_casa, tentativa+1);
        }
        else {
            printf("Chave %s%s encontrada, endereco %d, %d acesso\n", chave_busca[cabecalho.posicao_busca].id_aluno, chave_busca[cabecalho.posicao_busca].sigla_disc, endereco_casa, tentativa+1);
        }

        printf("Id do aluno: %s\n", registro.id_aluno);
        printf("Sigla da disciplina: %s\n", registro.sigla_disc);
        printf("Nome do aluno: %s\n", registro.nome_aluno);
        printf("Nome da disciplina: %s\n", registro.nome_disc);
        printf("Média do aluno: %.2f\n\n", registro.media);
        printf("Frequência do aluno: %.2f\n\n", registro.freq);
    }

    (cabecalho.posicao_busca)++;
    fseek(arquivo_dados, 0, 0);
    fwrite(&cabecalho, sizeof(s_cabecalho), 1, arquivo_dados);
    fflush(stdin);

    fclose(arquivo_dados);
    fclose(arquivo_hash);

    return;
}