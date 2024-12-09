/*
TRABALHO 2:
    -VICTOR MARIANO ROCHA
    -PATRICK PERETE
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<assert.h>
#include<locale.h>

struct hist
{
    char id_aluno[4];
    char sigla_disc[4];
    char nome_aluno[50];
    char nome_disc[50];
    float media;
    float freq;
};

struct Indice_P {
    char id_aluno[4];
    char sigla_disc[4];
    int BOF;
};

struct Indice_S {
    char nome_aluno[50];
    int BOF_cabeca;
	
};

struct Cabecalho
{
    int regLidos_insere;
    int regLidos_busca_P;
    int regLidos_busca_S;
    int BOF_dispo;
    int BOF_local;

};

struct CabecalhoIndice {
    bool updateFlag;
    int qtde_registros;
};

bool existeArq(const char *filename)
{
    // Verifica se o arquivo existe e não é um diretório
    if(access(filename, F_OK) == 0){
        return true;
    }
    return false;
   return access(filename, F_OK) == 0;
}

int menu_inicial()
{
    int op;

    do {
        printf("\n----------------Menu------------------\n");
        printf("Inserir (1)\n");
        printf("Carregar/Atualizar indice na memoria (2)\n");
        printf("Pesquisa chave primária (3)\n");
        printf("Pesquisa chave secundária (4)\n");
        printf("Sair (5)\n");
        printf("Opcao: ");
        scanf(" %d", &op);
    } while (op != 1 && op != 2 && op != 3 && op != 4 && op != 5);

    return op;
}


int pega_registro(FILE *fp, char *buffer) {
    int pipeline_counter = 0;
    int tam_registro = 0;
    int i = 0;

    while(pipeline_counter < 5) {
        buffer[i] = fgetc(fp);
        if(buffer[i] == '|') pipeline_counter++;
        tam_registro++;
        i++;
    }

    buffer[i] = '\0';
    return tam_registro;
}


void ordenarVetorIndice_P(struct Indice_P *index, int qtde_registros) {
    int i, j;
    struct Indice_P temp;
    
    for(i=0; i<qtde_registros-1; i++) {
        for(j=1; j<qtde_registros-i; j++) {
            
            if(index[j].id_aluno < index[j-1].id_aluno) {
                temp = index[j];
                index[j] = index[j-1];
                index[j-1] = temp;
            }

            else if(index[j].id_aluno == index[j-1].id_aluno) {
                if(index[j].sigla_disc < index[j-1].sigla_disc) {
                    temp = index[j];
                    index[j] = index[j-1];
                    index[j-1] = temp;
                }
            }
        }
    }
}

bool stringsIguais(char *str1, char *str2) {
    int i=0, biggestLenght;

    if(strlen(str1) > strlen(str2))
        biggestLenght = strlen(str1);

    else biggestLenght = strlen(str2);

    while(i < biggestLenght) {
        if(str1[i] != str2[i]) {
            return false;
        }
        i++;
    }

    return true;
}

int buscaSequencial_P(struct Indice_P *arr, struct Indice_P key, int qtde_elementos) {
    int i;

    for(i=0; i<qtde_elementos; i++) {
        if(key.id_aluno == arr[i].id_aluno && key.sigla_disc == arr[i].sigla_disc)
            return arr[i].BOF;
    }

    printf("\nNão há registros correspondentes a busca\n");
    return -1;
}

void buscaSequencial_S(struct Indice_S *arr, struct Indice_P *arr2, struct Indice_S key, int qtde_elementos, FILE *fp_lista_invertida, FILE *fp_arq) {
    int i, BOF_cabeca, BOF_prox, tam_reg;
    char registro[512];

    for(i=0; i<qtde_elementos; i++) {

        if(stringsIguais(key.nome_aluno, arr[i].nome_aluno)) {
            BOF_cabeca = arr[i].BOF_cabeca;
            struct Indice_P primaryKey;

            fseek(fp_lista_invertida, BOF_cabeca, SEEK_SET);
			char id_aluno[5];
			fread(id_aluno, sizeof(char), 4, fp_lista_invertida);
            id_aluno[4] = '\0'; // Adicionar o terminador de string
            do {
				strcpy(primaryKey.id_aluno, id_aluno);

                 
                //fgetc(fp_lista_invertida);
                //.id_aluno = fgetc(fp_lista_invertida) - 48;

                fread(&BOF_prox, sizeof(int), 1, fp_lista_invertida);

                if(BOF_prox == -1) break;

                int BOF_Registro = buscaSequencial_P(arr2, primaryKey, qtde_elementos);
                fseek(fp_arq, BOF_Registro, SEEK_SET);
                fread(&tam_reg, sizeof(int), 1, fp_arq);
                pega_registro(fp_arq, registro);

                printf("\n%s\n", registro);

                fseek(fp_lista_invertida, BOF_prox, SEEK_SET);
            }while(BOF_prox != -1);
        }
    }
}

void buscaChave_P(FILE *fp_arq, FILE **fp_busca_P, struct Indice_P *indexArray) {
    if ((*fp_busca_P = fopen("busca_p.bin", "r+b")) == NULL) {
        printf("N„o foi possÌvel abrir o arquivo");
        return;
    }
    
    struct Indice_P key;
    struct Cabecalho header;
    int tam_reg;
    char registro[512];

    rewind(fp_arq);
    fread(&header, sizeof(header), 1, fp_arq);

    fseek(*fp_busca_P, header.regLidos_busca_P * (2 * sizeof(int)), SEEK_SET);
    fread(&key.id_aluno, sizeof(int), 1, *fp_busca_P);
    fread(&key.sigla_disc, sizeof(char), 1, *fp_busca_P);
    
    int BOF_Registro = buscaSequencial_P(indexArray, key, header.regLidos_insere);

    if(BOF_Registro != -1) {
        fseek(fp_arq, BOF_Registro, SEEK_SET);
        fread(&tam_reg, sizeof(int), 1, fp_arq);
        pega_registro(fp_arq, registro);

        printf("\n%s\n", registro);
    }

    header.regLidos_busca_P++;

    rewind(fp_arq);
    fwrite(&header, sizeof(header), 1, fp_arq);
    rewind(fp_arq);
}

void buscaChave_S(FILE *fp_arq, FILE **fp_busca_S, struct Indice_S *indexArray, struct Indice_P *indexArray2, FILE *fp_lista_invertida) {
    if ((*fp_busca_S = fopen("busca_s.bin", "r+b")) == NULL) {
        printf("N„o foi possÌvel abrir o arquivo de busca secundária");
        return;
    }

    struct Indice_S key;
    struct Cabecalho header;
    int newMoviesCounter;

    rewind(fp_arq);
    fread(&header, sizeof(header), 1, fp_arq);

    fseek(*fp_busca_S, (header.regLidos_busca_S * 50), SEEK_SET);
    fread(key.nome_aluno, sizeof(char), 50, *fp_busca_S);

    rewind(fp_lista_invertida);
    fread(&newMoviesCounter, sizeof(int), 1, fp_lista_invertida);

    buscaSequencial_S(indexArray, indexArray2, key, header.regLidos_insere, fp_lista_invertida, fp_arq);

    header.regLidos_busca_S++;

    rewind(fp_arq);
    fwrite(&header, sizeof(header), 1, fp_arq);
    rewind(fp_arq);
}

// Atualiza o arquivo Indice_P a partir do arquivo dos registros:
void recriarIndice_P(FILE *fp_arq, FILE *fp_indice_P, struct Indice_P *indexArray) {
    fclose(fp_indice_P);

    if ((fp_indice_P = fopen("indice_P.bin", "w+b")) == NULL) {
        printf("Não foi possível abrir o arquivo de indices");
        return;
    }

    char id_aluno;
    char sigla_disc;
    int BOF;
    int tam_reg;
    int i = 0, qtde_registros = 0;
    char ch_aux;
    struct Cabecalho header;

    bool updateFlag = false;
    fwrite(&updateFlag, sizeof(bool), 1, fp_indice_P);

    fseek(fp_arq, 0, SEEK_SET);
    fread(&header, sizeof(header), 1, fp_arq);

    do {
        ch_aux = fgetc(fp_arq);
        fseek(fp_arq, -1, SEEK_CUR);

        if (ch_aux != EOF) {
            BOF = ftell(fp_arq);
            fread(&tam_reg, sizeof(int), 1, fp_arq);

            id_aluno = fgetc(fp_arq) - 48;
            fgetc(fp_arq); // Leitura do caractere separador
            sigla_disc = fgetc(fp_arq) - 48;

            // Converter id_aluno e sigla_disc para strings
            char id_aluno_str[2];
            char sigla_disc_str[2];
            sprintf(id_aluno_str, "%c", id_aluno);
            sprintf(sigla_disc_str, "%c", sigla_disc);

            strcpy(indexArray[i].id_aluno, id_aluno_str);
            strcpy(indexArray[i].sigla_disc, sigla_disc_str);            
            indexArray[i].BOF = BOF;

            i++;
            qtde_registros++;

            fseek(fp_arq, tam_reg - (3 * sizeof(char)), SEEK_CUR);
        }
    } while (ch_aux != EOF);

    ordenarVetorIndice_P(indexArray, qtde_registros);

    updateFlag = true;
    rewind(fp_indice_P);
    fwrite(&updateFlag, sizeof(bool), 1, fp_indice_P);
    fwrite(&qtde_registros, sizeof(int), 1, fp_indice_P);

    for (i = 0; i < qtde_registros; i++) {
        char buffer[20]; // Aumentar o buffer para garantir que caibam todos os dados
        sprintf(buffer, "%s|%s", indexArray[i].id_aluno, indexArray[i].sigla_disc);
        fwrite(buffer, strlen(buffer), 1, fp_indice_P);
        fwrite(&indexArray[i].BOF, sizeof(int), 1, fp_indice_P);
    }

    updateFlag = true;
    rewind(fp_indice_P);
    fwrite(&updateFlag, sizeof(bool), 1, fp_indice_P);
    rewind(fp_indice_P);

    fclose(fp_indice_P);

    if ((fp_indice_P = fopen("indice_P.bin", "r+b")) == NULL) {
        printf("Não foi possível abrir o arquivo");
        return;
    }
}

// Carrega o Indice_P em memÛria
void carregarIndice_P(FILE *fp_arq, FILE *fp_indice_P, struct Indice_P *indexArray) {
    struct CabecalhoIndice header;
    int i;
    
    rewind(fp_indice_P);
    fread(&header.updateFlag, sizeof(bool), 1, fp_indice_P);
    fread(&header.qtde_registros, sizeof(int), 1, fp_indice_P);

    for (i = 0; i < header.qtde_registros; i++) {
        // Ler o caractere e converter para string
        char id_aluno_char = (char) (fgetc(fp_indice_P) - 48);
        indexArray[i].id_aluno[0] = id_aluno_char;
        indexArray[i].id_aluno[1] = '\0';  // Terminar a string

        // Ler o caractere separador
        fgetc(fp_indice_P);

        // Ler a sigla do índice
        fscanf(fp_indice_P, "%s", indexArray[i].sigla_disc);

        // Ler o BOF
        fread(&indexArray[i].BOF, sizeof(int), 1, fp_indice_P);
    }

    // Atualizar o header
    rewind(fp_indice_P);
    header.updateFlag = false;
    fwrite(&header.updateFlag, sizeof(bool), 1, fp_indice_P);
    rewind(fp_indice_P);
}


void inserir(FILE **fp_insere, FILE *fp_arq, FILE *fp_indice_P, FILE *fp_indice_S, FILE *fp_lista_invertida, struct Indice_P *index_P_Array, struct Indice_S *index_S_Array) {
    if ((*fp_insere = fopen("insere.bin", "r+b")) == NULL) {
        printf("N„o foi possÌvel abrir o arquivo");
        return;
    }

    struct hist registro;
    struct Cabecalho header;
    int i, counter=0;


    int BOF_ant_elem;
    int BOF_atual_elem;


    rewind(fp_arq);
    fread(&header, sizeof(header), 1, fp_arq);

    fseek(*fp_insere, header.regLidos_insere * sizeof(registro), SEEK_SET);
    fread(&registro, sizeof(registro), 1, *fp_insere);

    char buffer[512];
    sprintf(buffer, "%s|%s|%s|%s|%f|%f", registro.id_aluno, registro.sigla_disc, registro.nome_aluno, registro.nome_disc, registro.media, registro.freq);
    int tam_reg = strlen(buffer);

    // Adicionando a nova inserÁ„o no arquivo de registros (arq_registros):
    fseek(fp_arq, 0, SEEK_END);
    int BOF = ftell(fp_arq);


    /**/
    printf("BOF: %d", BOF);
    //return;
    /**/


    fwrite(&tam_reg, sizeof(int), 1, fp_arq);
    fwrite(buffer, tam_reg, 1, fp_arq);

    // Adicionando a nova inserÁ„o em array_indice_P:
	strcpy(index_P_Array[header.regLidos_insere].id_aluno, registro.id_aluno);
    index_P_Array[header.regLidos_insere].BOF = BOF;
    strcpy(index_P_Array[header.regLidos_insere].sigla_disc, registro.sigla_disc);

    // Adicionando a nova inserÁ„o em array_indice_S e no arquivo ListaInvertida:
    for(i=0; i<header.regLidos_insere; i++) {
        if(stringsIguais(registro.nome_aluno, index_S_Array[i].nome_aluno)) {
            BOF_ant_elem = index_S_Array[i].BOF_cabeca;
            counter++;
            break;
        }
    }

    fseek(fp_lista_invertida, 0, SEEK_END);
    BOF_atual_elem = ftell(fp_lista_invertida);

    char primaryKey[9];
    sprintf(primaryKey, "%s|%s", registro.id_aluno, registro.sigla_disc);

    fwrite(primaryKey, sizeof(char), 3, fp_lista_invertida);

    // N„o tinha o nome do aluno:
    if(counter == 0) {
        int menosum = -1;
        fwrite(&menosum, sizeof(int), 1, fp_lista_invertida);
        
        int newMoviesCounter;
        rewind(fp_lista_invertida);
        fread(&newMoviesCounter, sizeof(int), 1, fp_lista_invertida);

        strcpy(index_S_Array[newMoviesCounter].nome_aluno, registro.nome_aluno);
        index_S_Array[newMoviesCounter].BOF_cabeca = BOF_atual_elem;

        newMoviesCounter++;
        rewind(fp_lista_invertida);
        fwrite(&newMoviesCounter, sizeof(int), 1, fp_lista_invertida);
    }

    // J· tinha o nome do aluno:
    else {
        index_S_Array[i].BOF_cabeca = BOF_atual_elem;
        fwrite(&BOF_ant_elem, sizeof(int), 1, fp_lista_invertida);
    }

    header.regLidos_insere++;

    ordenarVetorIndice_P(index_P_Array, header.regLidos_insere);

    // Atualizando o arquivo Indice_S:
    fseek(fp_indice_S, 0, SEEK_END);
    fwrite(&registro.nome_aluno, strlen(registro.nome_aluno), 1, fp_indice_S);
    fwrite(&BOF_atual_elem, sizeof(int), 1, fp_indice_S);
    rewind(fp_indice_S);

    rewind(fp_arq);
    fwrite(&header, sizeof(header), 1, fp_arq);

    printf("\nRegistro incluÌdo com sucesso!\n");

    fclose(*fp_insere);
}

int main() {
    setlocale(LC_ALL, "");
    FILE *fp_arq, *fp_insere, *fp_indice_P, *fp_indice_S, *fp_busca_P, *fp_busca_S, *fp_lista_invertida;

    struct Cabecalho headerArqRegistros;
    struct CabecalhoIndice headerIndice_P;
    struct CabecalhoIndice headerIndice_S;

    struct Indice_P registrosIndice_P[512];
    struct Indice_S registrosIndice_S[512];

    if (existeArq("arq_registros.bin") == false) {
        if ((fp_arq = fopen("arq_registros.bin", "w+b")) == NULL) {
            printf("N„o foi possÌvel abrir o arquivo");
            return 0;
        }

        headerArqRegistros.regLidos_insere = 0;
        headerArqRegistros.regLidos_busca_P = 0;
        headerArqRegistros.regLidos_busca_S = 0;
        fwrite(&headerArqRegistros, sizeof(headerArqRegistros), 1, fp_arq);
        rewind(fp_arq);


        if ((fp_indice_P = fopen("indice_P.bin", "w+b")) == NULL) {
            printf("N„o foi possÌvel abrir o arquivo");
            return 0;
        }

        headerIndice_P.updateFlag = false;
        headerIndice_P.qtde_registros = 0;
        fwrite(&headerIndice_P, sizeof(headerIndice_P), 1, fp_indice_P);
        rewind(fp_indice_P);


        // Abrindo Indice_S para escrita:
        if ((fp_indice_S = fopen("indice_S.bin", "w+b")) == NULL) {
            printf("N„o foi possÌvel abrir o arquivo");
            return 0;
        }

        headerIndice_S.updateFlag = false;
        headerIndice_S.qtde_registros = 0;
        fwrite(&headerIndice_S.updateFlag, sizeof(bool), 1, fp_indice_S);
        fwrite(&headerIndice_S.qtde_registros, sizeof(int), 1, fp_indice_S);
        rewind(fp_indice_S);


        // Abrindo ListaInvertida para escrita:
        if ((fp_lista_invertida = fopen("ListaInvertida.bin", "w+b")) == NULL) {
            printf("N„o foi possÌvel abrir o arquivo");
            return 0;
        }

        int newRegCounter = 0;
        fwrite(&newRegCounter, sizeof(int), 1, fp_lista_invertida);
        rewind(fp_lista_invertida);
    }

    else {
        if ((fp_arq = fopen("arq_registros.bin", "r+b")) == NULL) {
            printf("N„o foi possÌvel abrir o arquivo");
            return 0;
        }

        if ((fp_indice_P = fopen("indice_P.bin", "r+b")) == NULL) {
            printf("N„o foi possÌvel abrir o arquivo");
            return 0;
        }

        fread(&headerIndice_P.updateFlag, sizeof(bool), 1, fp_indice_P);
        rewind(fp_indice_P);

        if(headerIndice_P.updateFlag == false)
            recriarIndice_P(fp_arq, fp_indice_P, registrosIndice_P);

        carregarIndice_P(fp_arq, fp_indice_P, registrosIndice_P);


        // Abrindo Indice_S para leitura:
        if ((fp_indice_S = fopen("indice_S.bin", "r+b")) == NULL) {
            printf("N„o foi possÌvel abrir o arquivo");
            return 0;
        }
        
        carregarIndice_P(fp_arq, fp_indice_P, registrosIndice_P);

        // Abrindo ListaInvertida para leitura:
        if ((fp_lista_invertida = fopen("ListaInvertida.bin", "r+b")) == NULL) {
            printf("N„o foi possÌvel abrir o arquivo");
            return 0;
        }
    }

    int op = 0;

    while (op < 4) {
        op = menu_inicial();

        switch (op) {
        case 1:
            inserir(&fp_insere, fp_arq, fp_indice_P, fp_indice_S, fp_lista_invertida, registrosIndice_P, registrosIndice_S);
            break;
        case 2:
            buscaChave_P(fp_arq, &fp_busca_P, registrosIndice_P);
            break;
        case 3:
            buscaChave_S(fp_arq, &fp_busca_S, registrosIndice_S, registrosIndice_P, fp_lista_invertida);
            break;
        default:
            recriarIndice_P(fp_arq, fp_indice_P, registrosIndice_P);
            break;
        }
    }
}
