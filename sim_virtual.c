#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct sim_virtual
{
    unsigned endereco_fisico; // endereço lógico da página
    int pag_referenciada; // 1 se a página foi referenciada, 0 caso contrário
    int pag_modificada;   // 1 se a página foi modificada, 0
    int ultimo_acesso; // último acesso à página
};

typedef struct sim_virtual SimVirtual;

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

    //alocação do vetor de páginas físicas
    SimVirtual* buffer_mem_fisica = (SimVirtual*)malloc(qtd_pag_mem * sizeof(SimVirtual));
    if(!buffer_mem_fisica) {
        perror("Erro ao alocar memória para paginas físicas");
        exit(1);
    }

    for (int i = 0; i < qtd_pag_mem; i++)
    {
        buffer_mem_fisica[i].endereco_fisico = 0; // inicializa o endereço físico da página
        buffer_mem_fisica[i].pag_referenciada = 0; // inicializa a página como não referenciada
        buffer_mem_fisica[i].pag_modificada = 0; // inicializa a página como não modificada
        buffer_mem_fisica[i].ultimo_acesso = 0; // inicializa o último
    }
    

}