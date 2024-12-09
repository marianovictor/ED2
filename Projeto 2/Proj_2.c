#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define CLIENT_ID_LEN 12
#define VEHICLE_ID_LEN 8
#define NAME_LEN 50
#define KEY_LEN 19
#define INDEX_LEN 25

typedef enum {
	EXIT, 
	INPUT, 
	SEARCH,
} MenuEnum;

typedef struct {
	char client_id [CLIENT_ID_LEN];
	char vehicle_id [VEHICLE_ID_LEN];
	char client_name [NAME_LEN];
	char vehicle_name [NAME_LEN];
	int days;
} VehicleRental;

typedef struct {
	char key[KEY_LEN];
	char address;
} Index;

typedef struct {
	char client_id[CLIENT_ID_LEN];
	char vehicle_id[VEHICLE_ID_LEN];
} SearchOption;

/**
 * Faz a insercao no arquivo insere.bin
 * @param f_main referencia arquivo principal - main.bin
 * @param f_input referencia arquivo de entrada - insere.bin
 * @param f_index referencia arquivo de indices - indice.bin
 * @param index_arr referencia vetor de indices e memoria
 * @param index_position referencia posicao do vetor de indices
 */
void input_data(FILE *f_main, FILE *f_input, FILE *f_index, Index *index_arr, int *index_position); 

/**
 * Faz a busca
 * @param f_main referencia arquivo principal - main.bin
 * @param f_search referencia arquivo de busca - busca_p.bin
 * @param f_index referencia arquivo de indices - indice.bin
 * @param index_arr referencia vetor de indices e memoria
 * @param index_position referencia posicao do vetor de indices
 */
void search_data(FILE *f_main, FILE *f_search, FILE *f_indice, Index *index_arr, int index_position); 

/**
 * Faz a busca utilizando o arquivo de indice como base
 * @param f_index referencia arquivo de indices - indice.bin
 * @param f_main referencia arquivo principal - main.bin
 * @param search_option id cliente e id veiculo
 * @param index_arr referencia vetor de indices e memoria
 * @param index_position referencia posicao do vetor de indices
 * @return 1 se a busca foi bem sucedida e 0 caso contrario
 */
int search_index_file(FILE *f_indice, FILE *f_main, SearchOption search_option, Index *index_arr, int index_position); 

/**
 * Faz a busca direto no arquivo principal
 * @param f_main referencia arquivo principal - main.bin
 * @param search_option id cliente e id veiculo
 * @return 1 se a busca foi bem sucedida e 0 caso contrario
 */
int search_main_file(FILE *f_main, SearchOption search_option);

/**
 * Mostra na tela as opcoes de busca do arquivo busca_p.bin
 * @param f_search referencia do arquivo de busca - busca_p.bin
 */
void show_search_options (FILE *f_search);

/**
 * Mostra na tela as opcoes de insercao do arquivo insere.bin
 * @param file referencia do arquivo de insercao - insere.bin
 */
void show_input_data(FILE *file);

/**
 * Mostra as informacoes do objeto VehicleRental
 * @param vehicle_rental recebe objeto vehicle_rental
 */
void show_vehicle_rental_data(VehicleRental vehicle_rental);

/**
 * Atualiza o indice quando entra no app verificando o header
 * @param index referencia do vetor em memoria principal dos indices
 * @param position posicao do arquivo de indice
 */
void update_index(Index *index, int position);

/**
 * A brief description. A more elaborate class description
 * @param index referencia do vetor em memoria principal dos indices
 * @param f_index referencia arquivo de indices - indice.bin
 * @param index_position referencia posicao do vetor de indices
 * @param f_main referencia arquivo principal - main.bin
 */
void arr_insert (Index *index, FILE *f_index, int *index_position, FILE *f_main);

int main() {
	
	FILE *f_main, *f_input, *f_search, *f_index;
	Index index[INDEX_LEN];
	int option, index_position = 0;
	
	// abrir arquivos ou criar quando necessario
	f_main = fopen("data.bin", "ab+");
	f_index = fopen("indice.bin", "ab+");
	
	// abrir arquivos de entrada
	f_input = fopen("insere.bin", "rb");
	f_search = fopen("busca_p.bin", "rb");
	
	// adiciona o vetor em memoria
	arr_insert(index, f_index, &index_position, f_main);
	
	// fecha arquivos
	fclose(f_index);
	fclose(f_main);
	
	do {
		printf("Digite a opcao desejada: \n");
		printf("(1) Adicionar registro\n");
		printf("(2) Busca \n" );
		printf("(0) Sair \n");
		
		// opcao do menu
		scanf ("%d", &option);
		fflush(stdin);
		
		switch(option) {
			case INPUT:
				input_data(f_main, f_input, f_index, index, &index_position);
				break;
			case SEARCH:
				search_data(f_main, f_search, f_index, index, index_position);
				break;
			case 0:
				printf("Finalizando... \n");
				
				update_index(index, index_position);
				
				fclose(f_input);
				fclose(f_search);
				
				exit(0);
				break;
				
			default:
				printf ("Opcao nao encontrada\n");
		}
	} while(option != 0);
	
	return 0;
}

void input_data(FILE *f_main, FILE *f_input, FILE *f_index, Index *index_arr, int *index_position) {

	VehicleRental vehicle_rental; 
	int index, data_len, main_len;
	char data[sizeof (VehicleRental)], key[KEY_LEN];
	int is_not_updated = -1;
	
	f_main = fopen("data.bin", "r+b");
	f_index = fopen("indice.bin","r+b");
	
	// usuario escolhe qual opcao deseja inserir
	show_input_data(f_input);
	printf ("Indice que deseja inserir: \n"); 
	scanf ("%d", &index);
	
	fseek(f_input, 0, SEEK_SET);
	fseek(f_input, (index - 1) * sizeof(VehicleRental), SEEK_SET);
	fread (&vehicle_rental, sizeof (VehicleRental), 1, f_input);
	
	sprintf (data,"%s|%s|%s|%s|%d|", vehicle_rental.client_id, vehicle_rental.vehicle_id, vehicle_rental.client_name, vehicle_rental.vehicle_name, vehicle_rental.days);
	data_len = strlen (data);
	
	fseek(f_main, 0, SEEK_END); 
	fwrite(&data_len, 1, 1, f_main);
	fwrite(data, sizeof(char), data_len, f_main);
	printf("Inserido_com sucesso!\n");
	
	// atualizar o header do idice para desatualizado
	fseek(f_index, 0, SEEK_SET);
	fwrite(&is_not_updated, sizeof(int), 1, f_index);
	
	// adicionando valores no vetor
	strcat (strcpy (key, vehicle_rental.client_id), vehicle_rental.vehicle_id);
	
	main_len = ftell(f_main);
	strcpy (index_arr[*index_position].key, key);
	index_arr[*index_position].address = main_len - data_len;
	*index_position = *index_position + 1;
	fclose(f_main); 
	fclose(f_index);
}

void search_data(FILE *f_main, FILE *f_search, FILE *f_indice, Index *index_arr, int index_position) {
	SearchOption search_option;
	int option;
	
	f_main = fopen("data.bin", "r+b");
	show_search_options(f_search);
	
	printf ("Posicao desejada:\n");
	scanf ("%d", &option); 
	printf("\n");
	
	fseek(f_search, (option - 1) * sizeof(SearchOption) , SEEK_SET);
	fread (&search_option, sizeof(SearchOption), 1, f_search);
		
//	if(search_index_file(f_indice, f_main, search_option, index_arr, index_position) && !search_main_file(f_main, search_option)) {
	if (!search_index_file(f_indice, f_main, search_option, index_arr, index_position)) {
		printf ("Registro nao encontrado! \n");
	} else {
		printf ("Registro encontrado com sucesso! \n");
	}

	fclose(f_main);
}

int search_index_file(FILE *f_indice, FILE *f_main, SearchOption search_option, Index *index_arr, int index_position) {
	
	VehicleRental vehicle_rental;
	int data_len, days;
	char key_aux [KEY_LEN], buffer[sizeof(Index)], *pipe, mat[5][50];
	
	strcat (strcpy(key_aux, search_option.client_id), search_option.vehicle_id);
	fseek(f_indice, sizeof(int), SEEK_SET);
	Index test;
	fread(&test, sizeof(Index), 19, f_indice);
	int i;

	for (i = 0; 1 < index_position; i++) {
		
		fseek(f_main, index_arr[i].address-1, SEEK_SET);
		fread (&data_len, 1, 1, f_main);
		
		fread(&buffer, data_len + 1, 1, f_main);
		pipe = strtok(buffer, "|");
		
		for (i = 0; i < 5; i++) {
			if (i == 4)
				days = atoi(pipe);
			else {
				strcpy(mat[i], pipe);
				pipe = strtok(NULL, "|");
			}
		}
		
		strcpy(vehicle_rental.client_id, mat[0]); 
		strcpy(vehicle_rental.vehicle_id, mat[1]); 
		strcpy(vehicle_rental.client_name, mat[2]); 
		strcpy(vehicle_rental.vehicle_name, mat[3]);
		vehicle_rental.days = days;
		printf("---------------\n");
		show_vehicle_rental_data(vehicle_rental);
		printf("---------------\n");
		
		return 1;
	}
	
	return 0;
}

int search_main_file(FILE *f_main, SearchOption search_option) {

	VehicleRental buffer;
	int is_correct_client_id, is_correct_vehicle_id;
	
	fseek(f_main, 0, SEEK_SET);
	
	while (fread(&buffer, sizeof(VehicleRental), 1, f_main) != 0) {
		is_correct_client_id = strcmp(buffer.client_id, search_option.client_id) == 0;
		is_correct_vehicle_id = strcmp(buffer.vehicle_id, search_option.vehicle_id) == 0;
		
		if(is_correct_client_id && is_correct_vehicle_id) {
			
			printf("Registro encontrado! \n"); 
			printf("-----------\n");
			show_vehicle_rental_data(buffer);
			printf("-----------\n");
			
			return 1; // retorna sucesso na insercao.
		} 
	}
		
	return 0; // retorna falha na insercao.
}

void show_search_options(FILE *f_search) {
	SearchOption buffer;
	int i = 0;
	fseek(f_search, 0, 0);
	while(fread(&buffer, sizeof(SearchOption), 1, f_search) != 0) {
		
		i++;
		
		printf("-----------\n");
		printf("Posicao: %d\n", i);
		printf ("Cod Cliente: %s\n", buffer.client_id); 
		printf ("Cod Veiculo: %s\n", buffer.vehicle_id);
		printf("-----------\n");
	}
}

void show_input_data(FILE *file) {

	VehicleRental buffer;
	int i = 0;
		
	fseek(file, 0, 0);
	
	while (fread(&buffer, sizeof (VehicleRental), 1, file) != 0){
		i++;
		printf("-----------\n");
		printf("Indice: %d\n", i); 
		show_vehicle_rental_data(buffer);
	
	}
	printf("-----------\n");

}

void show_vehicle_rental_data(VehicleRental vehicle_rental) {

	printf("Cod Cliente: %s\n", vehicle_rental.client_id);
	printf("Cod Veiculo: %s\n", vehicle_rental.vehicle_id);
	printf ("Nome Cliente: %s\n", vehicle_rental.client_name); 
	printf ("Nome Veiculo: %s\n", vehicle_rental.vehicle_name);
	printf ("Dias: %d\n", vehicle_rental.days);
		
}

void update_index(Index *index, int position) {
	FILE *f_index;
	int i = 0, is_updated = 0;
	
	f_index = fopen("indice.bin", "r+b");
	
	fseek(f_index, sizeof(int), SEEK_SET);
	
	for(i = 0; i < position; i++) {
		fwrite(&index[i].key, sizeof(char), KEY_LEN, f_index);
		fwrite(&index[i].address, sizeof(int), 1, f_index);
	}
	
	fseek (f_index, 0, SEEK_SET);
	fwrite(&is_updated, sizeof(int), 1, f_index);
	fclose(f_index);
}

void arr_insert(Index *index, FILE *f_index, int *index_position, FILE *f_main) {
	
	int file_len = 1, len = 0, header;
	char client_id [CLIENT_ID_LEN], vehicle_id[VEHICLE_ID_LEN - 1];
	char key[KEY_LEN];
	
	fseek(f_index, 0, SEEK_SET);
	fread(&header, sizeof(int), 1, f_index);
	
	if (header == -1) {
		fseek(f_main, 0, SEEK_SET);
		
		while (fread(&len, 1, 1, f_main)) {
			
			fread(&client_id, sizeof(char), CLIENT_ID_LEN - 1, f_main);
			client_id[11] = '\0';
			fseek(f_main, 1, SEEK_CUR);
			fread(&vehicle_id, sizeof(char), VEHICLE_ID_LEN - 1, f_main);
			
			strcat(strcpy(key, client_id), vehicle_id);
			
			strcpy(index [*index_position].key, key);
			
			index[*index_position].address = file_len;
			
			file_len += len + 1;
			
			fseek(f_main, -(KEY_LEN + 1), SEEK_CUR);
			fseek(f_main, len + 1, SEEK_CUR);
			
			*index_position = *index_position + 1;
		}
	} else {
		while (fread(&index[*index_position].key, sizeof(char), KEY_LEN, f_index)) {
		
			fread(&index[*index_position].address, sizeof(int), 1, f_index);
			*index_position = *index_position + 1;
		}
	}
}


