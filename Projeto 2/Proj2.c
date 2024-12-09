/*Projeto 01 Estrutura de Dados 2 
 Amanda Reis
 Lucas Góes 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <locale.h>


struct Reg {
    char CodCli[12];
    char CodVei[8];
    char NomeCli[50];
    char NomeVei[50];
    char NumDias[4];
};
struct Index {
	char Pkey[20];
	int BOF;
};
struct Cabecalho {
    int regLidos_insere;
    int regLidos_remove;
    int BOF_dispo;
    int BOF_local;
};

struct Cabecalho_index {
	
	int fleg_att;
	int quant_index;
	
};

struct Chave{
    char CodCli[12];
    char CodVei[8];
};


bool existeArq() {
    if (access("arq_registros.bin", F_OK) == 0)
        return true;

    return false;
}
bool existeArqIndex() {
    if (access("arq_index.bin", F_OK) == 0)
        return true;

    return false;
}

void pega_Pkay(char *buffer, struct Index *vet_index, int quant_index){
	int i, cont_barra=0;
	for(i = 0; i < 20; i++){
		if(buffer[i] == '|'){
			cont_barra++;
			if(cont_barra == 2){
				vet_index[quant_index].Pkey[19] = '\0';
				printf("%s", vet_index[quant_index].Pkey);
				break;
			}
			if(cont_barra == 1){
				vet_index[quant_index].Pkey[i] = '|';
			}
		}
		else{
			
			vet_index[quant_index].Pkey[i] = buffer[i];
			
		}
	}
}

int menu_inicial() {
    int op;

    do {
        printf("\n----------------Menu------------------\n");
        printf("Inserir (1)\n");
        printf("Carregar/Atualizar indice na memoria (2)\n");
        printf("Pesquisa chave primária (3)\n");
        printf("Sair (4)\n");
        printf("Opcao: ");
        scanf(" %d", &op);
    } while (op != 1 && op != 2 && op != 3 && op != 4);

    return op;
}


void sort(struct Index *vet_index, int tam)
{
	int i, j;
	if(tam <= 1){
    	return;
	}
    for (i = 0; i < tam; i++)
    {
    	
        for (j = i; j < tam; j++)
        {
        	if(strcmp(vet_index[i].Pkey, vet_index[j].Pkey)==1){
        		char temp[20];
        		strcpy(temp, vet_index[i].Pkey);
        		strcpy(vet_index[i].Pkey, vet_index[j].Pkey);
        		strcpy(vet_index[j].Pkey, temp);
			}
            
        }
    }
}

void carregar_index(struct Index *vet_index){
	FILE *fp_index;
	FILE *fp_aux;
	struct Cabecalho_index header_index;
	
	if ((fp_index = fopen("arq_index.bin","r+b")) == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return;
    }
    if ((fp_aux = fopen("arq_auxiliar.bin", "w+b")) == NULL){
    	
   		printf("Nao foi possivel abrir o arquivo");
        return;
    }
    
    int i;
    fread(&header_index, sizeof(struct Cabecalho_index), 1, fp_index);
    
    
    
    for(i = 0;;i++){
    	if(fgetc(fp_index) == EOF){
    		
    		if(header_index.fleg_att == 0){
    			sort(vet_index, header_index.quant_index);
    			header_index.fleg_att = 1;
    			fwrite(&header_index, sizeof(struct Cabecalho_index),1, fp_aux);
    			
    			
    			fwrite(vet_index, sizeof(struct Index), header_index.quant_index, fp_aux);
    			
    			fclose(fp_index);
    			fclose(fp_aux);
    			
    			remove("arq_index.bin");
    			rename("arq_auxiliar.bin","arq_index.bin");
    			
    			fclose(fp_aux);
    			printf("\nArquivo index Carregado/atualizado\n");
			}
    		
    		return;
		}
    	fseek(fp_index, -1, SEEK_CUR);
    	
    	
    	fread(&vet_index[i], sizeof(struct Index), 1, fp_index);
	}
    
	
}


void inserir(int index, struct Index *vet_index) {
    /*Abertura dos arquivos*/

    FILE *fp_insere; 
    FILE *fp_arq;
    FILE *fp_index;
    char no_char;

    if ((fp_insere = fopen("insere.bin","r+b")) == NULL) {
        printf("Nao foi possivel abrir o arquivo insere");
        return;
    }

    if ((fp_arq = fopen("arq_registros.bin","r+b")) == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return;
    }
    
    if ((fp_index = fopen("arq_index.bin","r+b")) == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return;
    }
    

   /*Leitura do cabeçalho do arquivo de registros*/

    struct Reg registro;
    struct Cabecalho header;
    struct Cabecalho_index header_index;

    rewind(fp_arq);
    rewind(fp_index);
    fread(&header, sizeof(struct Cabecalho), 1, fp_arq);
    fread(&header_index, sizeof(struct Cabecalho_index), 1, fp_index);

	
	
    if(header_index.quant_index == NULL){
    	//printf("teste");
    	header_index.quant_index = 0;
	}
	if(header.BOF_local == 1){
		header.BOF_local = sizeof(struct Cabecalho);
	}

    /*Posicionamento no local apropriado para inserção (-1 porque com�e�a no 0 a indexa�ao da inser�ao, mesma coisa com o remove)*/

    fseek(fp_insere, (index-1) * sizeof(registro), SEEK_SET);
    
    if(fgetc(fp_insere) == EOF){
    	printf("\nNao ha essa insercao\n");
    	return;
	}
    fseek(fp_insere, -1, SEEK_CUR);
    
    /*Leitura do registro a ser inserido*/

    fread(&registro, sizeof(registro), 1, fp_insere);
    
    /*Preparação dos dados do registro em um buffer*/
    char buffer[512];
    sprintf(buffer, "%s|%s|%s|%s|%s|", registro.CodCli, registro.CodVei, registro.NomeCli, registro.NomeVei, registro.NumDias);
	


  	pega_Pkay(buffer, vet_index, header_index.quant_index);
  	
  		
  	vet_index[header_index.quant_index].BOF = header.BOF_local;
  	
  
   /*Inserção do registro no arquivo de registros, a partir dos espaços disponíveis e o cabeçalho do arquivo*/

    int tam_reg = strlen(buffer);
    header.BOF_local += tam_reg + 4;
    header_index.fleg_att = 0;

    
    fseek(fp_index, 0, SEEK_END);

    // reg anexado ao final do arquivo de registro
    fwrite(&vet_index[header_index.quant_index], sizeof(struct Index), 1, fp_index);
    header_index.quant_index++;
     
     
	fseek(fp_arq, 0, SEEK_END);// reg anexado ao final do arquivo de registro
    fwrite(&tam_reg, sizeof(int), 1, fp_arq);
    fwrite(buffer, tam_reg, 1, fp_arq);
    
	header.regLidos_insere++; 
	
	
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


bool veri_att(){
	FILE *fp_index;
	struct Cabecalho_index header_index;
	if((fp_index = fopen("arq_index.bin","r+b")) == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return;
    }
    
    fread(&header_index, sizeof(struct Cabecalho_index), 1, fp_index);
    
    
    if(header_index.quant_index < 1){
    	fclose(fp_index);
    	return false;
	}
    
    if(header_index.fleg_att != 1){
    	printf("\narquivo index esta desatualizado\n");
    	
    	fclose(fp_index);
    	return true;
	}
	
	fclose(fp_index);
}

int buscaSequencialPrim(char *arr, struct Index *indexPrim, int quant_elem) {
    int i;

    

    for(i = 0; i < quant_elem; i++) {


       printf("%s==%s\n", indexPrim[i].Pkey, arr);

       if (strcmp(indexPrim[i].Pkey, arr) == 0){
            
            return indexPrim[i].BOF;

       }
           
    }

    printf("\nRegistro nao encontrado\n");
    return -1;
}

int pega_registro(FILE *fp, char *buffer) {
    int line_counter = 0;
    int tam_registro = 0;
    int i = 0;

    while (line_counter < 5) {
        buffer[i] = fgetc(fp);
        if (buffer[i] == '|')
            line_counter++;
        tam_registro++;
        i++;
    }
    buffer[i] = '\0'; // Adiciona '\0' no final do buffer para indicar o fim da string
    return tam_registro;
}

void buscaChavePrim(struct Index  *vet_index, int RRN) {
    FILE *fp, *fp_busca_prim;


    

    if ((fp = fopen("arq_registros.bin", "r+b")) == NULL) {
        printf("Erro ao abrir o arquivo resgitro");
        return;
    }

    if ((fp_busca_prim = fopen("busca_p.bin", "r+b")) == NULL) {
        printf("Erro ao abrir o arquivo busca");
        return;
    }
    
    char chave[20];
    struct Cabecalho header;
    int tam_reg;
    char stringReg[512];

    rewind(fp);
    fread(&header, sizeof(header), 1, fp);

    fseek(fp_busca_prim, RRN*20 , SEEK_SET);
    fread(&chave, 20, 1, fp_busca_prim);
    
    chave[11] = '|';

   // printf("%s\n", vet_index[1].Pkey);
    


    int BOF_Registro = buscaSequencialPrim(chave, vet_index, header.regLidos_insere);

   // printf("%d\n", BOF_Registro);

    if(BOF_Registro != -1) {
        fseek(fp, BOF_Registro, SEEK_SET);
        fread(&tam_reg, sizeof(int), 1, fp);
        pega_registro(fp, stringReg);

        printf("\n%s\n", stringReg);
    }

    header.regLidos_insere++;

    rewind(fp);
    fwrite(&header, sizeof(header), 1, fp);
    rewind(fp);
    fclose(fp);
    fclose(fp_busca_prim);
}




int main() {
    FILE *fp_arq;
    FILE *fp_index;
	int index;
    struct Cabecalho header;
    struct Cabecalho_index header_index;

	struct Index vet_index[20];

    header.regLidos_insere = 0;
    header.regLidos_remove = 0;
    header.BOF_dispo = -1;


    if (existeArq() == false) {
        if ((fp_arq = fopen("arq_registros.bin", "w+b")) == NULL) {
            printf("Erro ao abrir o arquivo");
            return 0;
        }
        fwrite(&header, sizeof(struct Cabecalho), 1, fp_arq);
        rewind(fp_arq);
        fclose(fp_arq);
    }
    if (existeArqIndex() == false) {
        if ((fp_index = fopen("arq_index.bin", "w+b")) == NULL) {
            printf("Erro ao abrir o arquivo");
            return 0;
        }
        fwrite(&header, sizeof(struct Cabecalho_index), 1, fp_index);
        rewind(fp_index);
        fclose(fp_index);
    }
    
    
    int op = 0;
    
    if(veri_att()){
    	carregar_index(vet_index);
	}
    else{
        carregar_index(vet_index);
    }
	
    while (op != 4) { 
        op = menu_inicial();

        switch (op) {
        case 1:
        	printf("index: ");
        	scanf("%d", &index);
            inserir(index, vet_index);
            break;
        case 2:
        	carregar_index(vet_index);
            break;
        case 3:
            scanf("%d", &index);
            buscaChavePrim(vet_index, index-1);
            break;
        default:
            carregar_index(vet_index);
            break;
        }
    }
}
