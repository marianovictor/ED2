#include<stdio.h>
#include<conio.h>
#include<string.h>

int main() {
    FILE *fd;
    
    //////////////////////////////   
    struct hist {
        char id_aluno[4];
        char sigla_disc[4];
        char nome_aluno[50];
        char nome_disc[50];
        float media;
        float freq;
    } vet[7] = { {"000", "001", "Nome-1", "Disc-111", 1, 1},
                 {"000", "014", "Nome-14", "Disc-222", 2, 2},
                 {"000", "027", "Nome-27", "Disc-333", 3, 3},
                 {"000", "040", "Nome-40", "Disc-444", 4, 4},
                 {"000", "053", "Nome-53", "Disc-555", 5, 5},
                 {"000", "066", "Nome-66", "Disc-666", 6, 6},
				 {"000", "053", "Nome-53", "Disc-777", 7, 7}};//chave duplicada
       
    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	struct busca {
        char id_aluno[4];
        char sigla_disc[4];
    } vet_b[5] = { {"000","014"},
                   {"000","040"},
                   {"000","053"},
                   {"000","066"},
                   {"000","066"}};
    
    fd = fopen("busca.bin", "w+b");
    fwrite(vet_b, sizeof(vet_b), 1, fd);
    fclose(fd);    
    
    //////////////////////////////
    struct remove {
        char id_aluno[4];
        char sigla_disc[4];
    } vet_r[2] = { {"000","014"},
                   {"000","040"}};
       
    fd = fopen("remove.bin", "w+b");
    fwrite(vet_r, sizeof(vet_r), 1, fd);
    fclose(fd);
}

