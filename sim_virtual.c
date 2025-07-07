#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim_virtual.h"


int main(int argc, char* argv[])
{
    if(argc != 5) {
        printf("Quantida de argumentos inválida!!!\n");
        exit(1);
    }
    //serão recebidos ao total 4 argumentos na chamada do programa
    //o primeiro é o tipo od algoritmo a ser utilizado
    char tipo_algoritmo[50];
    strcpy(tipo_algoritmo, argv[1]);

    //o segundo é o nome do arquivo com os acessos à memória
    char nome_arquivo_acessos[50];
    strcpy(nome_arquivo_acessos, argv[2]);

    //o terceiro é o tamanho de cada página
    unsigned tam_pag_mem = atoi(argv[3]);

    //o quarto é o tamanho da memória física
    unsigned tam_memoria_fisica = atoi(argv[4]);

    if(tam_pag_mem != 8 && tam_pag_mem != 32)
    {
        printf("Tamanho de páginas inválido!!! Ou 8KB ou 32 KB\n");
        exit(1);
    }

    if(tam_memoria_fisica != 1 && tam_memoria_fisica != 2)
    {
        printf("Tamanho de memória inválido!!! Ou 1MB ou 2MB\n");
        exit(1);
    }

    //cálculo da quantidade de páginas
    int qtd_pag_mem = 0;
    qtd_pag_mem = (tam_memoria_fisica*1024) / tam_pag_mem;   
    //alocação do vetor de páginas físicas
    SimVirtual buffer_mem_fisica[qtd_pag_mem]; 
    //memset(buffer_mem_fisica, 0, sizeof(buffer_mem_fisica));
    for (int i = 0; i < qtd_pag_mem; i++)
    {
        buffer_mem_fisica[i].em_uso = 0;
    }
    
    int qtd_pgs_sujas = 0;
    int qtd_pgs_faults = 0;

    Param_Sim params;
    strcpy(params.nome_arquivo_acessos, nome_arquivo_acessos);
    params.qtd_pag_mem = qtd_pag_mem;
    params.tam_page = tam_pag_mem;
    
    printf("[Executando simulador...]\n");
    printf("Arquivo de entrada: %s\n", nome_arquivo_acessos);
    printf("Tamanho memoria fisica: %d MB\n", tam_memoria_fisica);
    printf("Tamanho das paginas: %d KB\n", tam_pag_mem);
    printf("Algoritmo de Substituicao: %s\n", tipo_algoritmo);

    if(strcmp(tipo_algoritmo, "LRU")==0)
    {
        lru_algoritmo(params, buffer_mem_fisica, &qtd_pgs_sujas, &qtd_pgs_faults);
    }
    else if(strcmp(tipo_algoritmo, "2nd_chance")==0)
    {
        second_chance_algoritmo(params, buffer_mem_fisica, &qtd_pgs_sujas, &qtd_pgs_faults);
    }
    else if(strcmp(tipo_algoritmo, "clock")==0)
    {
        clock_algoritmo(params, buffer_mem_fisica, &qtd_pgs_sujas, &qtd_pgs_faults);
    }
    else //algoritmo ótimo
    {
        algoritmo_otimo(params, buffer_mem_fisica, &qtd_pgs_sujas, &qtd_pgs_faults);
    }

    printf("Numero de faltas de páginas: %d\n", qtd_pgs_faults);
    printf("Numero de páginas escritas: %d\n", qtd_pgs_sujas);

}
