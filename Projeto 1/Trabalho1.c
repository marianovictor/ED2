/*
TRABALHO 1:
    -VICTOR MARIANO ROCHA
    -PATRICK PERETE
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <locale.h>




struct hist {
    char id_aluno[4];
    char sigla_disc[4];
    char nome_aluno[50];
    char nome_disc[50];
    float media;
    float freq;
};
                

struct Cabecalho {
    int registrosLidos_insere;
    int registrosLidos_remove;
    int BOF_dispo;
};


struct Chave{
    char id_aluno[4];
    char sigla_disc[4];
};


bool existeArq() {
    if (access("arq_registros.bin", F_OK) == 0)
        return true;

    return false;
}

bool Chave_iguais(char *str1, char *str2) {
    int i;

    for(i=0; i<strlen(str2); i++) { // Compara os caracteres das duas strings na mesma posição
        if(str1[i] != str2[i]) return false;
    }

    return true;
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

int menu_inicial() {
    int op;

    do {
        printf("\n----------------Menu------------------\n");
        printf("Inserir (1)\n");
        printf("Remover (2)\n");
        printf("Compactar (3)\n");
        printf("Sair (4)\n");
        printf("Opcao: ");
        scanf(" %d", &op);
    } while (op != 1 && op != 2 && op != 3 && op != 4);

    return op;
}

void inserir(int index) {
    /*Abertura dos arquivos*/
    
    FILE *fp_insere; 
    FILE *fp_arq;


    if ((fp_insere = fopen("insere.bin","r+b")) == NULL) {
        printf("Nao foi possivel abrir o arquivo insere");
        return;
    }

    if ((fp_arq = fopen("arq_registros.bin", "r+b")) == NULL) {
        // Se o arquivo não existe, cria ele
        if ((fp_arq = fopen("arq_registros.bin", "w+b")) == NULL) {
            printf("Erro ao criar o arquivo\n");
            return;
        }
    }

   /*Leitura do cabeçalho do arquivo de registros*/

    struct hist registro;
    struct Cabecalho header;

    
    rewind(fp_arq);
    fread(&header, sizeof(struct Cabecalho), 1, fp_arq);

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
    sprintf(buffer, "%s|%s|%s|%s|%f|%f|", registro.id_aluno, registro.sigla_disc, registro.nome_aluno, registro.nome_disc, registro.media, registro.freq);

   /*Inserção do registro no arquivo de registros, a partir dos espaços disponíveis e o cabeçalho do arquivo*/

    int tam_registro = strlen(buffer);
    int tam_espaco;
     
    //Variáveis para o controle dos offsets (deslocamentos) para os espaços dispo no arquivo de registros.
    int BOF_ant_dispo = 8;
    int BOF_atual_dispo;
    int BOF_prox_dispo;
    
    if (header.BOF_dispo != -1) { //Verifica se há espaços disponíveis
        fseek(fp_arq, header.BOF_dispo, SEEK_SET);
        BOF_atual_dispo = header.BOF_dispo;

        /*Navegção pelos espaços dispo*/
        while (true) {
            fread(&tam_espaco, sizeof(int), 1, fp_arq); //Lê o tam do espaço dispo
            fgetc(fp_arq); //Lê o '|'
            fread(&BOF_prox_dispo, sizeof(int), 1, fp_arq); //Lê o próximo offset para o espaço dispo seguinte.

            /*Verifica o tam do espaço dispo*/
            if (tam_registro <= tam_espaco) {
                fseek(fp_arq, (-1) * sizeof(int) - 1, SEEK_CUR);
                fwrite(buffer, 1, tam_registro, fp_arq);// Escrita do reg no local apropriado 

                if (BOF_ant_dispo == 8) {  // Atualização do Cabeçalho por conta da remoção do espaço dispo
                    header.BOF_dispo = BOF_prox_dispo;
                    rewind(fp_arq);
                    fwrite(&header, sizeof(struct Cabecalho), 1, fp_arq);
                } else { 
                    fseek(fp_arq, BOF_ant_dispo + sizeof(int) + 1, SEEK_SET);
                    fwrite(&BOF_prox_dispo, 1, sizeof(int), fp_arq);
                }

                header.registrosLidos_insere++; // Incrementação dos registros inseridos no Cabeçalho 
                break;
            } else { // Atualização dos offsets para manter a lista de espaços dispo
                if (BOF_prox_dispo == -1) {
                    fseek(fp_arq, 0, SEEK_END);//
                    fwrite(&tam_registro, sizeof(int), 1, fp_arq);
                    fwrite(buffer, tam_registro, 1, fp_arq);
                    header.registrosLidos_insere++; 
                    break;
                } else {
                    if (BOF_ant_dispo == 8) {
                        header.BOF_dispo = BOF_atual_dispo;
                        rewind(fp_arq);
                        fwrite(&header, sizeof(struct Cabecalho), 1, fp_arq);
                    } else {
                        fseek(fp_arq, BOF_ant_dispo + sizeof(int) + 1, SEEK_SET);
                        fwrite(&BOF_atual_dispo, 1, sizeof(int), fp_arq);
                    }

                    fseek(fp_arq, BOF_atual_dispo + sizeof(int) + 1, SEEK_SET);
                    fwrite(&BOF_prox_dispo, sizeof(int), 1, fp_arq);

                    BOF_ant_dispo = BOF_atual_dispo;
                    BOF_atual_dispo = BOF_prox_dispo;
                    fseek(fp_arq, BOF_prox_dispo, SEEK_SET);
                }
            }
        }
    } else { // Se o tamanho do espaço dispo for menor do que o tam do registro, ou se não houver espaço dispo
        fseek(fp_arq, 0, SEEK_END);// reg anexado ao final do arquivo de registro
        fwrite(&tam_registro, sizeof(int), 1, fp_arq);
        fwrite(buffer, tam_registro, 1, fp_arq);
        header.registrosLidos_insere++; 
    }

    rewind(fp_arq);
    fwrite(&header, sizeof(struct Cabecalho), 1, fp_arq);
    printf("\nRegistro incluido com sucesso!\n");

    fclose(fp_insere);
    fclose(fp_arq);
}

void remover(int index) {
  
    /*Abertura dos arquivos*/
    FILE *fp_remove;
    FILE *fp_arq;

    if ((fp_remove = fopen("remove.bin","r+b")) == NULL) {
        printf("Nao foi possivel abrir o arquivo");
        return;
	}

    if ((fp_arq = fopen("arq_registros.bin","r+b")) == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    /*Leitura da chave e cabeçalho*/
    struct Chave key;
    struct Cabecalho header;

    int tam_registro = 0;
    int acum_BOF = sizeof(struct Cabecalho);
    int registros_checados = 0;
    char buffer_key[256];
    char buffer_registro[512];

    rewind(fp_arq); //Posiciona a leitura/escrita no início do arq_registros
    fread(&header, sizeof(struct Cabecalho), 1, fp_arq);
    fseek(fp_remove, (index - 1)*sizeof(key), SEEK_SET);
    
    /*Verifica se ainda tem reg para ser eliminado*/
    if(fgetc(fp_remove) == EOF){
    	printf("\nNao ha essa remocao\n");
    	return;
	}
	fseek(fp_remove, -1, SEEK_CUR);
	
    fread(&key, sizeof(key), 1, fp_remove);
    sprintf(buffer_key, "%s|%s", key.id_aluno, key.sigla_disc);

    /*Leitura dos registros do arquivo de registros*/
    while(true) {
        fread(&tam_registro, sizeof(int), 1, fp_arq); // Lê o tamanho do registro no arquivo de registros
        fread(&buffer_registro, tam_registro, 1, fp_arq);// Lê o registro do arquivo de registros e armazena em um buffer

      if(!Chave_iguais(buffer_registro,buffer_key)) {
            acum_BOF += tam_registro; //Incrementação do acumulador do deslocamento
            registros_checados++; 

            fgetc(fp_arq);
            if(feof(fp_arq)) {
                header.registrosLidos_remove++; // Incrementa o contador de registros removidos no cabeçalho
               
                rewind(fp_arq);// Reposiciona o cursor no início do arquivo de registros e atualiza o cabeçalho
                fwrite(&header, sizeof(struct Cabecalho), 1, fp_arq);
				
                printf("\nNao ha nenhum registro correspondente a essa chave no arquivo\n");

                fclose(fp_remove);

                return;
            }

            else fseek(fp_arq, -1, SEEK_CUR); 
        }

        else break; // Se a chave do reg atual correponde a chave a ser removida, sai do loop
    }

    fseek(fp_arq, (-1)*tam_registro, SEEK_CUR);//Reposiciona o cursor para antes do reg a ser removido
    // Marca o registro como removido, usando um caractere '*' para indicar a remoção
    char estrela = '*';
    fwrite(&estrela, 1, 1, fp_arq);
    fwrite(&header.BOF_dispo, sizeof(int), 1, fp_arq);//Escreve o novo valor do offset no arquivo de registros

    acum_BOF += registros_checados*sizeof(int); // Atualiza o cabeçalho com o novo valor do acumulador do deslocamento
    header.BOF_dispo = acum_BOF;
    header.registrosLidos_remove++;


    rewind(fp_arq);
    
    
    fwrite(&header, sizeof(struct Cabecalho), 1, fp_arq); // Escreve o cabeçalho atualizado no arquivo de registros
    
    printf("\nRegistro excluido com sucesso!\n");
	fclose(fp_arq);
    fclose(fp_remove);
}

void compactar(){
    int tam_espaco, tam_registro;
    char buffer[512];
    char ch_aux;

    struct Cabecalho header;
     
    FILE *fp_arq;
    FILE *fp_aux;

    if ((fp_aux = fopen("arq_auxiliar.bin", "w+b")) == NULL)
    {
        printf("Nao foi possivel abrir o arquivo");
        return;
    }

    if ((fp_arq = fopen("arq_registros.bin","r+b")) == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    rewind(fp_arq);
    fread(&header, sizeof(struct Cabecalho), 1, fp_arq);
    header.BOF_dispo = -1;
    fwrite(&header, sizeof(struct Cabecalho), 1, fp_aux);

    while (true) /*Processamento de cada registro no arquivo de registros*/
    {
        fread(&tam_espaco, sizeof(int), 1, fp_arq);//// Lê o tamanho do espaço disponível para o registro
        ch_aux = fgetc(fp_arq); //  Lê o próximo caractere do arquivo de registros

        if (ch_aux == '*')// Verifica se o caractere indica que o registro foi marcado como removido ('*')
        {	

            fseek(fp_arq, (tam_espaco - 1), SEEK_CUR);
            continue;
        }	

        if (ch_aux == EOF)
        {	
            break;
        }

        fseek(fp_arq, -1, SEEK_CUR);
        tam_registro = pega_registro(fp_arq, buffer);
		
		
        fwrite(&tam_registro, sizeof(int), 1, fp_aux);// Escreve o tamanho do registro no arquivo auxiliar
        fwrite(buffer, strlen(buffer), 1, fp_aux);// Escreve o registro no arquivo auxiliar

        fseek(fp_arq, tam_espaco - tam_registro, SEEK_CUR);
        ch_aux = fgetc(fp_arq);

        if (ch_aux != EOF) // Verifica se não é o final do arquivo de registros
        {
			
            fseek(fp_arq, -1, SEEK_CUR);
        }
        else{
        	
            break; // Se for o final do arquivo, sai do loop
		}
        	
    }

    fclose(fp_arq);
    fclose(fp_aux);

    /*Gerenciamento dos Arquivos*/

    char arquivo_original[100] = "arq_registros.bin";
    char arquivo_auxiliar[100] = "arq_auxiliar.bin";

    if (remove(arquivo_original) == -1)
        printf("Erro ao excluir arquivo\n");
    if (rename(arquivo_auxiliar, arquivo_original) != 0)
        printf("Erro ao renomear arquivo\n");
    remove(arquivo_auxiliar);

    printf("\nArquivo compactado com sucesso!\n");

    if ((fp_arq = fopen("arq_registros.bin", "r+b")) == NULL)
    {
        printf("Nao foi possivel abrir o arquivo");
        return;
    }
    fclose(fp_arq);
    
}


int main(){
    FILE *fd;
    FILE *fp_arq;
	int index;
    struct Cabecalho header;
    header.registrosLidos_insere = 0;
    header.registrosLidos_remove = 0;
    header.BOF_dispo = -1;

    int op = 0;
    
    if (existeArq() == false) {
        if ((fp_arq = fopen("arq_registros.bin", "w+b")) == NULL) {
            printf("Erro ao abrir o arquivo");
            return 0;
        }
        fwrite(&header, sizeof(struct Cabecalho), 1, fp_arq);
        rewind(fp_arq);
        fclose(fp_arq);
    }

    //////////////////////////////
    struct hist {
        char id_aluno[4];
        char sigla_disc[4];
        char nome_aluno[50];
        char nome_disc[50];
        float media;
        float freq;
    } vet[6] = {{"001", "AAA", "Jo�o da Silva", "AAAA", 7.5, 72.3},
                {"001", "BBB", "Jo�o da Silva", "BBB", 8.2, 80.4},
                {"001", "CCC", "Jo�o da Silva", "CCCCC", 5.4, 79.7},
                
                {"001", "CCC", "Paulo Souza", "CCCCC", 6.8, 91.3},
                {"001", "HHH", "Paulo Souza", "HHH", 7.3, 82.7},
                
                {"002", "HHH", "Pedro Silva Silva", "HHH", 9.5, 92.7}};
       
    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	struct remove {
        char id_aluno[4];
        char sigla_disc[4];
    } vet_r[5] = {{"001","CCC"},
                  {"001","BBB"},
                  {"002","HHH"},
                  {"001","HHH"},
                  {"001","AAA"}};
       
    fd = fopen("remove.bin", "w+b");
    fwrite(vet_r, sizeof(vet_r), 1, fd);
    fclose(fd);
    //////////////////////////////
    
    while (op != 4) { 
        op = menu_inicial();

        switch (op) {
        case 1:
        	printf("index: ");
        	scanf("%d", &index);
            inserir(index);
            break;
        case 2:
        	printf("index: ");
        	scanf("%d", &index);
            remover(index);
            break;
        case 3:
            compactar();
            break;
        default:
            break;
        }
    }
}