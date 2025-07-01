#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct sim_virtual
{
    int pag_referenciada; // 1 se a página foi referenciada, 0 caso contrário
    int pag_modificada;   // 1 se a página foi modificada, 0
    int ultimo_acesso; // último acesso à página
};

typedef struct sim_virtual SimVirtual;

unsigned find_largest_adress(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    unsigned largest_address = 0;
    unsigned addr;
    char c;

    while (fscanf(file, "%x %c", &addr, c) == 1) {
        if (addr > largest_address) {
            largest_address = addr;
        }
    }

    fclose(file);
    return largest_address;
}

int main(int argc, char* argv[])
{
    if(argc != 5) {
        printf("Quantida de argumentos inválida!!!\n");
        exit(1);
    }
    //serão recebidos ao total 4 argumentos na chamada do programa
    //o primeiro é o tipo od algoritmo a ser utilizado
    char* tipo_algoritmo;
    strcpy(tipo_algoritmo, argv[1]);

    //o segundo é o nome do arquivo com os acessos à memória
    char* nome_arquivo_acessos;
    strcpy(nome_arquivo_acessos, argv[2]);

    //o terceiro é o tamanho de cada página
    unsigned tam_pag_mem = atoi(argv[3]);

    //o quarto é o tamanho da memória física
    unsigned tam_memoria_fisica = atoi(argv[4]);

    if(tam_pag_mem != 8 || tam_pag_mem != 32)
    {
        printf("Tamanho de páginas inválido!!!\n");
        exit(1);
    }

    if(tam_memoria_fisica != 1 || tam_memoria_fisica != 2)
    {
        printf("Tamanho de páginas inválido!!!\n");
        exit(1);
    }

    //cálculo da quantidade de páginas
    int qtd_pag_mem = 0;
    qtd_pag_mem = (tam_memoria_fisica*1024) / tam_pag_mem;   

    //cálculo da quantidade de páginas virtuais
    unsigned largest_address = find_largest_adress(nome_arquivo_acessos);
    int qtd_pag_virtuais = 0;
    qtd_pag_virtuais = (largest_address / tam_pag_mem) + 1;

    //alocação do vetor de páginas virtuais
    SimVirtual* paginas_virtuais = (SimVirtual*)malloc(qtd_pag_virtuais * sizeof(SimVirtual));
    if(!paginas_virtuais) {
        perror("Erro ao alocar memória para páginas virtuais");
        exit(1);
    }

    //alocação do vetor de páginas físicas
    SimVirtual* paginas_físicas = (SimVirtual*)malloc(qtd_pag_virtuais * sizeof(SimVirtual));
    if(!paginas_físicas) {
        perror("Erro ao alocar memória para paginas físicas");
        exit(1);
    }

   

}