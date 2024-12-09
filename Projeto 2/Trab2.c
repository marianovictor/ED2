/*
TRABALHO 2:
    -VICTOR MARIANO ROCHA
    -PATRICK PERETE
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <locale.h>
//#include <windows.h>
#include <stdbool.h>

struct hist
{
    char id_aluno[4];
    char sigla_disc[4];
    char nome_aluno[50];
    char nome_disc[50];
    float media;
    float freq;
};

struct Cabecalho
{
    int registrosLidos_insere;
    int BOF_local;
};
struct Index
{
    char Pkey[20];
    int BOF;
};
struct Chave
{
    char id_aluno[4];
    char sigla_disc[4];
};
struct Cabecalho_index
{

    int fleg_att;
    int quant_index;
};

bool existeArq(const char *filename);
int pega_registro(FILE *fp, char *buffer);
void pega_Pkay(char *buffer, struct Index *vet_index, int quant_index);
int menu_inicial();
void inserir(int index, struct Index *vet_index);
void carregar_index(struct Index *vet_index);
bool veri_att();
void sort(struct Index *vet_index, int tam);
int buscaSequencialPrim(char *chaveBuscada, struct Index *indexPrim, int quant_elem);
void buscaChavePrim(struct Index *vet_index, int RRN);
void buscaChaveSec(int indice);

int main()
{
    FILE *fp_index;
    FILE *fp_arq;
    int index;
    struct Cabecalho header;
    struct Index index_array[20];
    struct Cabecalho_index header_index;

    header.registrosLidos_insere = 0;
    int op = 0;

    if (existeArq("arq_registros.bin") == false)
    {
        if ((fp_arq = fopen("arq_registros.bin", "w+b")) == NULL)
        {
            printf("Erro ao abrir o arquivo de registros");
            return 0;
        }
        fwrite(&header, sizeof(struct Cabecalho), 1, fp_arq);
        rewind(fp_arq);
        fclose(fp_arq);
    }
    if (existeArq("arq_index.bin") == false)
    {
        if ((fp_index = fopen("arq_index.bin", "w+b")) == NULL)
        {
            printf("Erro ao abrir o arquivo de index");
            return 0;
        }
        fwrite(&header, sizeof(struct Cabecalho_index), 1, fp_index);
        rewind(fp_index);
        fclose(fp_index);
    }

    if (veri_att())
    {
        carregar_index(index_array);
    }
    else
    {
        carregar_index(index_array);
    }

    while (op != 4)
    {
        op = menu_inicial();

        switch (op)
        {
        case 1:
            printf("index para inserir: ");
            scanf("%d", &index);
            inserir(index, index_array);
            break;
        case 2:
            carregar_index(index_array);
            break;
        case 3:
            printf("index para pesquisar por chave primária: ");
            scanf("%d", &index);
            buscaChavePrim(index_array, index - 1);
            break;
        case 4:
            printf("index para pesquisar por chave secundária: ");
            scanf("%d", &index);
            buscaChaveSec(index - 1);
            break;
        case 5:
            carregar_index(index_array);
            exit(0);

            break;

        default:
            carregar_index(index_array);
            break;
        }
    }
}

bool existeArq(const char *filename)
{
    // Verifica se o arquivo existe e não é um diretório
    if (access(filename, F_OK) == 0)
    {
        return true;
    }
    return false;
    return access(filename, F_OK) == 0;
}

int pega_registro(FILE *fp, char *buffer)
{
    int line_counter = 0;
    int tam_registro = 0;
    int i = 0;

    while (line_counter < 5)
    {
        printf("Pega resgistro: %c\n", buffer[i]);
        buffer[i] = fgetc(fp);
        if (buffer[i] == '|')
            line_counter++;
        tam_registro++;
        i++;
    }
    buffer[i] = '\0'; // Adiciona '\0' no final do buffer para indicar o fim da string
    return tam_registro;
}

void pega_Pkay(char *buffer, struct Index *vet_index, int quant_index)
{
    int i, cont_barra = 0;
    for (i = 0; i < 20; i++)
    {
        if (buffer[i] == '|')
        {
            cont_barra++;
            if (cont_barra == 2)
            {
                vet_index[quant_index].Pkey[19] = '\0';
                printf("%s", vet_index[quant_index].Pkey);
                break;
            }
            if (cont_barra == 1)
            {
                vet_index[quant_index].Pkey[i] = '|';
            }
        }
        else
        {

            vet_index[quant_index].Pkey[i] = buffer[i];
        }
    }
}

int menu_inicial()
{
    int op;

    do
    {
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

void inserir(int index, struct Index *vet_index)
{
    /*Abertura dos arquivos*/

    FILE *fp_insere;
    FILE *fp_arq;
    FILE *fp_index;

    if ((fp_insere = fopen("insere.bin", "r+b")) == NULL)
    {
        printf("Nao foi possivel abrir o arquivo insere.bin");
        return;
    }

    if ((fp_arq = fopen("arq_registros.bin", "r+b")) == NULL)
    {
        // Se o arquivo não existe, cria ele
        if ((fp_arq = fopen("arq_registros.bin", "w+b")) == NULL)
        {
            printf("Erro ao criar o arquivo de registros\n");
            return;
        }
    }
    if ((fp_index = fopen("arq_index.bin", "r+b")) == NULL)
    {
        printf("Erro ao abrir o arquivo de index\n");
        return;
    }

    /*Leitura do cabeçalho do arquivo de registros*/

    struct hist registro;
    struct Cabecalho header;
    struct Cabecalho_index header_index;

    rewind(fp_arq);
    rewind(fp_index);

    fread(&header, sizeof(struct Cabecalho), 1, fp_arq);
    fread(&header_index, sizeof(struct Cabecalho_index), 1, fp_index);

    if (header_index.quant_index == NULL)
    {
        //printf("teste");
        header_index.quant_index = 0;
    }
    if (header.BOF_local == 1)
    {
        header.BOF_local = sizeof(struct Cabecalho);
    }
    /*Posicionamento no local apropriado para inserção (-1 porque com�e�a no 0 a indexa�ao da inser�ao, mesma coisa com o remove)*/

    fseek(fp_insere, (index - 1) * sizeof(registro), SEEK_SET);

    if (fgetc(fp_insere) == EOF)
    {
        printf("\nNão foi encontrada essa opção para inserir\n");
        return;
    }
    fseek(fp_insere, -1, SEEK_CUR);

    /*Leitura do registro a ser inserido*/

    fread(&registro, sizeof(registro), 1, fp_insere);

    /*Preparação dos dados do registro em um buffer*/
    char buffer[512];
    sprintf(buffer, "%s|%s|%s|%s|%f|%f|", registro.id_aluno, registro.sigla_disc, registro.nome_aluno, registro.nome_disc, registro.media, registro.freq);

    pega_Pkay(buffer, vet_index, header_index.quant_index);
    vet_index[header_index.quant_index].BOF = header.BOF_local;

    /*Inserção do registro no arquivo de registros, a partir dos espaços disponíveis e o cabeçalho do arquivo*/

    int tam_registro = strlen(buffer);
    printf("Tamanhpo registro: %d", tam_registro);
    header.BOF_local += tam_registro + 4;
    header_index.fleg_att = 0;

    fseek(fp_index, 0, SEEK_END);

    // reg anexado ao final do arquivo de registro
    fwrite(&vet_index[header_index.quant_index], sizeof(struct Index), 1, fp_index);
    header_index.quant_index++;

    fseek(fp_arq, 0, SEEK_END); // reg anexado ao final do arquivo de registro
    fwrite(&tam_registro, sizeof(int), 1, fp_arq);
    fwrite(buffer, tam_registro, 1, fp_arq);

    header.registrosLidos_insere++;

    rewind(fp_index);
    fwrite(&header_index, sizeof(struct Cabecalho_index), 1, fp_index);
    rewind(fp_arq);
    fwrite(&header, sizeof(struct Cabecalho), 1, fp_arq);
    printf("\nRegistro incluido com sucesso!\n");

    fclose(fp_insere);
    fclose(fp_arq);
    fclose(fp_index);
    sort(vet_index, header_index.quant_index);
}

void carregar_index(struct Index *vet_index)
{
    FILE *fp_index;
    FILE *fp_aux;
    struct Cabecalho_index header_index;

    if ((fp_index = fopen("arq_index.bin", "r+b")) == NULL)
    {
        printf("Erro ao abrir o arquivo de index\n");
        return;
    }
    if ((fp_aux = fopen("arq_auxiliar.bin", "w+b")) == NULL)
    {

        printf("Não foi possivel abrir o arquivo de index auxiliar");
        return;
    }

    int i;
    fread(&header_index, sizeof(struct Cabecalho_index), 1, fp_index);

    for (i = 0;; i++)
    {
        if (fgetc(fp_index) == EOF)
        {

            if (header_index.fleg_att == 0)
            {
                sort(vet_index, header_index.quant_index);
                header_index.fleg_att = 1;
                fwrite(&header_index, sizeof(struct Cabecalho_index), 1, fp_aux);

                fwrite(vet_index, sizeof(struct Index), header_index.quant_index, fp_aux);

                fclose(fp_index);
                fclose(fp_aux);

                remove("arq_index.bin");
                rename("arq_auxiliar.bin", "arq_index.bin");

                fclose(fp_aux);
                printf("\nArquivo index Carregado/atualizado\n");
            }

            return;
        }
        fseek(fp_index, -1, SEEK_CUR);

        fread(&vet_index[i], sizeof(struct Index), 1, fp_index);
    }
}

bool veri_att()
{

    FILE *fp_index;
    struct Cabecalho_index header_index;
    if ((fp_index = fopen("arq_index.bin", "r+b")) == NULL)
    {
        printf("Erro ao abrir o arquivo de index\n");
        return false;
    }

    fread(&header_index, sizeof(struct Cabecalho_index), 1, fp_index);

    if (header_index.quant_index < 1)
    {
        fclose(fp_index);
        return false;
    }

    if (header_index.fleg_att != 1)
    {
        printf("\nArquivo index esta desatualizado\n");

        fclose(fp_index);
        return true;
    }

    fclose(fp_index);
    return false;
}

void sort(struct Index *vet_index, int tam)
{
    int i, j;
    if (tam <= 1)
    {
        return;
    }
    for (i = 0; i < tam; i++)
    {

        for (j = i; j < tam; j++)
        {
            if (strcmp(vet_index[i].Pkey, vet_index[j].Pkey) == 1)
            {
                char temp[20];
                strcpy(temp, vet_index[i].Pkey);
                strcpy(vet_index[i].Pkey, vet_index[j].Pkey);
                strcpy(vet_index[j].Pkey, temp);
            }
        }
    }
}

void buscaChavePrim(struct Index *vet_index, int RRN)
{

    FILE *fp, *fp_busca_prim;
    struct Chave chave_primaria;
    struct hist registro;

    if ((fp = fopen("arq_registros.bin", "r+b")) == NULL)
    {
        printf("Erro ao abrir o arquivo de registro");
        return;
    }

    if ((fp_busca_prim = fopen("busca_p.bin", "r+b")) == NULL)
    {
        printf("Erro ao abrir o arquivo de busca primária");
        return;
    }

    //char chave[40];
    struct Cabecalho header;
    int tam_reg;
    char stringReg[512];

    rewind(fp);
    fread(&header, sizeof(header), 1, fp);

    fseek(fp_busca_prim, RRN * sizeof(struct Chave), SEEK_SET);
    fread(&chave_primaria, sizeof(struct Chave), 1, fp_busca_prim);

    char auxiliar[10];
    sprintf(auxiliar, "%s|%s", chave_primaria.id_aluno, chave_primaria.sigla_disc);

    int BOF_Registro = buscaSequencialPrim(auxiliar, vet_index, header.registrosLidos_insere);

    //printf("BOF: %d\n", BOF_Registro);

    if (BOF_Registro != -1)
    {
        fseek(fp, (BOF_Registro + 8), SEEK_SET);

        fread(&tam_reg, sizeof(int), 1, fp);

        fread(stringReg, sizeof(char), tam_reg, fp);

        //stringReg[tam_reg] = '\0';

        //pega_registro(fp, stringReg);

        printf("\nRegistro encontrado:\n");
        printf("%s\n", stringReg);
    }

    header.registrosLidos_insere++;

    rewind(fp);
    fwrite(&header, sizeof(header), 1, fp);
    rewind(fp);
    fclose(fp);
    fclose(fp_busca_prim);
}

int buscaSequencialPrim(char *chaveBuscada, struct Index *indexPrim, int quant_elem)
{
    int i;

    for (i = 0; i < quant_elem; i++)
    {

        //printf("%s==%s\n", indexPrim[i].Pkey, chaveBuscada);

        if (strcmp(indexPrim[i].Pkey, chaveBuscada) == 0)
        {

            return indexPrim[i].BOF;
        }
    }

    printf("\nRegistro não encontrado\n");
    return -1;
}

void buscaChaveSec(int indice)
{
    FILE *fp_busca_s, *fp_arq;
    struct hist registro;
    char nome[50];
    int encontrado = 0;

    // Abrindo o arquivo 'busca_s.bin' que contém os nomes
    if ((fp_busca_s = fopen("busca_s.bin", "r+b")) == NULL)
    {
        printf("Erro ao abrir o arquivo de busca de secundária\n");
        return;
    }

    // Posiciona no índice dado (cada nome ocupa 50 bytes)
    fseek(fp_busca_s, indice * 50, SEEK_SET);

    // Lê o nome da posição indicada pelo índice
    fread(nome, sizeof(char), 50, fp_busca_s);
    nome[49] = '\0'; // Garante que a string está terminada

    fclose(fp_busca_s);

    printf("Nome encontrado no índice %d: %s\n", indice, nome);

    // Agora busca o registro no arquivo 'arq_registros.bin' com o nome encontrado
    if ((fp_arq = fopen("arq_registros.bin", "r+b")) == NULL)
    {
        printf("Erro ao abrir o arquivo de registros\n");
        return;
    }

    // Pule o cabeçalho do arquivo
    struct Cabecalho header;
    fread(&header, sizeof(struct Cabecalho) + 4, 1, fp_arq);
    char registroAnalisado[512];
    char *token;
    //int i=0;
    // Percorre os registros até encontrar o nome correspondente
    while (fread(&registroAnalisado, sizeof(struct hist), 1, fp_arq) == 1)
    {
        //printf("%d\n", i);

        //printf("Token: %s\n", registroAnalisado);
        //printf("\n----------------------------\n");
        token = strtok(registroAnalisado, "|");
        if (token != NULL)
        {
            strncpy(registro.id_aluno, token, 3);
            registro.id_aluno[3] = '\0'; // Garantir a terminação da string
        }

        // Extrai a sigla da disciplina
        token = strtok(NULL, "|");
        if (token != NULL)
        {
            strncpy(registro.sigla_disc, token, 3);
            registro.sigla_disc[3] = '\0'; // Garantir a terminação da string
        }

        // Extrai o nome do aluno
        token = strtok(NULL, "|");
        if (token != NULL)
        {
            strncpy(registro.nome_aluno, token, 49);
            registro.nome_aluno[49] = '\0'; // Garantir a terminação da string
        }

        // Extrai o nome da disciplina
        token = strtok(NULL, "|");
        if (token != NULL)
        {
            strncpy(registro.nome_disc, token, 49);
            registro.nome_disc[49] = '\0'; // Garantir a terminação da string
        }

        // Extrai a média (converte de string para float)
        token = strtok(NULL, "|");
        if (token != NULL)
        {
            registro.media = atof(token);
        }

        // Extrai a frequência (converte de string para float)
        token = strtok(NULL, "|");
        if (token != NULL)
        {
            registro.freq = atof(token);
        }

        if (strcmp(registro.nome_aluno, nome) == 0)
        {
            encontrado = 1;
            printf("\nRegistro encontrado: \n");

            printf("ID Aluno: %s\n", registro.id_aluno);
            printf("Sigla Disciplina: %s\n", registro.sigla_disc);
            printf("Nome Aluno: %s\n", registro.nome_aluno);
            printf("Nome Disciplina: %s\n", registro.nome_disc);
            printf("Média: %.2f\n", registro.media);
            printf("Frequência: %.2f\n", registro.freq);
            break;
        }
        //i++;
    }

    if (!encontrado)
    {
        printf("\nRegistro não encontrado para o nome: %s\n", nome);
    }

    fclose(fp_arq);
}
