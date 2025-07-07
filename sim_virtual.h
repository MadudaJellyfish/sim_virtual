#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#define MAX_ACESSOS 1000000

struct sim_virtual
{
    unsigned endereco_fisico; // endereço lógico da página
    int pag_referenciada; // 1 se a página foi referenciada, 0 caso contrário
    int pag_modificada;   // 1 se a página foi modificada, 0
    int ultimo_acesso; // último acesso à página
    int em_uso;
};

struct param_sim
{
    int qtd_pag_mem;
    char nome_arquivo_acessos[50];
    int tam_page;
};

typedef struct param_sim Param_Sim;

typedef struct sim_virtual SimVirtual;

void lru_algoritmo(Param_Sim param, SimVirtual* buffer_mem_fisica, int* qtd_pgs_sujas, int* qtd_pgs_faults)

{
    FILE* arq_acessos;
    if(!(arq_acessos = fopen(param.nome_arquivo_acessos, "r")))
    {
        printf("Nao foi possivel abrir o arquivo %s!!!\n", param.nome_arquivo_acessos);
        exit(1);
    }

    unsigned addr;
    char r_or_w;

    unsigned shift_page = log2(param.tam_page*1024);
    int time = 0;
    
    while (fscanf(arq_acessos, "%x %c ", &addr, &r_or_w) == 2)
    {
        
        int page = addr>>shift_page;
       // printf("acesso %d: endereco=%x tipo=%c page=%d\n", time, addr, r_or_w, page);
        int is_page_in_mem = 0; //0 se não estiver, 1 c.c.
        for (int i = 0; i < param.qtd_pag_mem; i++)
        {
           // printf("   >> HIT na página %d (frame %d)\n", page, i);
            //Se a página estiver na memória
            if(buffer_mem_fisica[i].em_uso == 1 && page == buffer_mem_fisica[i].endereco_fisico)
            {
                if (r_or_w == 'W') buffer_mem_fisica[i].pag_modificada = 1; // Marca como suja 
                buffer_mem_fisica[i].pag_referenciada = 1; //hit no frame
                buffer_mem_fisica[i].ultimo_acesso = time;
                is_page_in_mem = 1;
                break;
            }
        }

        int was_page_inserted = 0;
        //caso a página não esteja na memória é gerado um page fault
        if(is_page_in_mem==0)
        {
            for (int i = 0; i < param.qtd_pag_mem; i++)
            {
                if(buffer_mem_fisica[i].em_uso == 0)
                {
                    buffer_mem_fisica[i].endereco_fisico = page;
                    if(r_or_w == 'W') buffer_mem_fisica[i].pag_modificada = 1;
                    else buffer_mem_fisica[i].pag_modificada = 0;
                    buffer_mem_fisica[i].pag_referenciada = 1;

                    buffer_mem_fisica[i].ultimo_acesso = time;
                    buffer_mem_fisica[i].em_uso = 1;

                    was_page_inserted = 1;
                    break;
                }
            }
            

            //alguma página precisa ser removida, neste caso, a com o acesso mais antigo
            if (was_page_inserted==0)
            {
                int uso_mais_antigo = INT_MAX;
                int i_older_page = 0;
                for (int i = 0; i < param.qtd_pag_mem; i++)
                {
                    if(buffer_mem_fisica[i].ultimo_acesso < uso_mais_antigo)
                    {
                        uso_mais_antigo = buffer_mem_fisica[i].ultimo_acesso;
                        i_older_page = i;
                    } 
                }

                if(buffer_mem_fisica[i_older_page].pag_modificada == 1) (*qtd_pgs_sujas)++;

                //carrega página nova
                buffer_mem_fisica[i_older_page].endereco_fisico = page;
                if (r_or_w == 'W') {
                    buffer_mem_fisica[i_older_page].pag_modificada = 1;
                } else {
                    buffer_mem_fisica[i_older_page].pag_modificada = 0;
                }
            
                buffer_mem_fisica[i_older_page].pag_referenciada = 1;
                buffer_mem_fisica[i_older_page].ultimo_acesso = time;

            }
            
            (*qtd_pgs_faults)++;
        }

        time++;
        
    }
    
    
}

void second_chance_algoritmo(Param_Sim param, SimVirtual* buffer_mem_fisica, int* qtd_pgs_sujas, int* qtd_pgs_faults)
{
    static int oldest_page  = 0; //está apontando para o primeiro frame que é o processo mais antigo
    
    FILE* arq_acessos;
    if(!(arq_acessos = fopen(param.nome_arquivo_acessos, "r")))
    {
        printf("Nao foi possivel abrir o arquivo %s!!!\n", param.nome_arquivo_acessos);
        exit(1);
    }

    unsigned addr;
    char r_or_w;

    unsigned shift_page = log2(param.tam_page*1024);
    int time = 0;
    
    while (fscanf(arq_acessos, "%x %c ", &addr, &r_or_w) == 2)
    {
        int page = addr>>shift_page;
        int is_page_in_mem = 0; //0 se não estiver, 1 c.c.
        for (int i = 0; i < param.qtd_pag_mem; i++)
        {
            //Se a página estiver na memória
            if(buffer_mem_fisica[i].em_uso == 1 && page == buffer_mem_fisica[i].endereco_fisico)
            {
                if (r_or_w == 'W') buffer_mem_fisica[i].pag_modificada = 1; // Marcar como suja imediatamente
                buffer_mem_fisica[i].pag_referenciada = 1;
                buffer_mem_fisica[i].ultimo_acesso = time;
                is_page_in_mem = 1;
                break;
            }
        }

        int was_page_inserted = 0;
        //caso a página não esteja na memória é gerado um page fault
        if(is_page_in_mem==0)
        {
            for (int i = 0; i < param.qtd_pag_mem; i++)
            {
                if(buffer_mem_fisica[i].em_uso == 0)
                {
                    buffer_mem_fisica[i].endereco_fisico = page;
                    if(r_or_w == 'W') buffer_mem_fisica[i].pag_modificada = 1;
                    else buffer_mem_fisica[i].pag_modificada = 0;
                    buffer_mem_fisica[i].pag_referenciada = 1;
                    buffer_mem_fisica[i].ultimo_acesso = time;
                    buffer_mem_fisica[i].em_uso = 1;

                    was_page_inserted = 1;
                    break;
                }
            }
            

            //aplicar o second chance caso não tenha sido possível inserir a página
            if (was_page_inserted==0)
            {   
                while (1) //enquanto não conseguir remover uma página continuar nesse loop
                {
                    if (buffer_mem_fisica[oldest_page].pag_referenciada == 0)
                    {
                        if(buffer_mem_fisica[oldest_page].pag_modificada == 1) (*qtd_pgs_sujas)++;

                        buffer_mem_fisica[oldest_page].endereco_fisico = page;
                        if (r_or_w == 'W') {
                        buffer_mem_fisica[oldest_page].pag_modificada = 1;
                        } else {
                        buffer_mem_fisica[oldest_page].pag_modificada = 0;
                        }
                        
                        buffer_mem_fisica[oldest_page].pag_referenciada = 1;
                        buffer_mem_fisica[oldest_page].ultimo_acesso = time;
                        break;
                    }
                    else //continuo a busca
                    {
                        //0 o bit R da atual e coloco ela no fim do buffer
                        buffer_mem_fisica[oldest_page].pag_referenciada = 0;
                        SimVirtual aux_page = buffer_mem_fisica[oldest_page];
                        for (int i = 0; i < param.qtd_pag_mem - 1; i++)
                        {
                            buffer_mem_fisica[i] = buffer_mem_fisica[i+1];
                        }

                        buffer_mem_fisica[param.qtd_pag_mem - 1] = aux_page;
 
                    }
                }

            }
            
            (*qtd_pgs_faults)++;
        }

        time++;
        
    }
}

void clock_algoritmo(Param_Sim param, SimVirtual* buffer_mem_fisica, int* qtd_pgs_sujas, int* qtd_pgs_faults)
{
    int ponteiro_relogio = 0;

    FILE* arq_acessos;
    if(!(arq_acessos = fopen(param.nome_arquivo_acessos, "r")))
    {
        printf("Nao foi possivel abrir o arquivo %s!!!\n", param.nome_arquivo_acessos);
        exit(1);
    }
    unsigned addr;
    char r_or_w;
    unsigned shift_page = log2(param.tam_page*1024);
    int time = 0;

    while (fscanf(arq_acessos, "%x %c ", &addr, &r_or_w) == 2)
    {
        int page = addr >> shift_page;
        int is_page_in_mem = 0;

        for (int i = 0; i < param.qtd_pag_mem; i++)
        {
            if (buffer_mem_fisica[i].em_uso == 1 && page == buffer_mem_fisica[i].endereco_fisico)
            {
                buffer_mem_fisica[i].pag_referenciada = 1;
                buffer_mem_fisica[i].ultimo_acesso = time;
                if (r_or_w == 'W') buffer_mem_fisica[i].pag_modificada = 1;
                is_page_in_mem = 1;
                break;
            }
        }

        if (!is_page_in_mem)
        {
            int inserida = 0;
            for (int i = 0; i < param.qtd_pag_mem; i++)
            {
                if (buffer_mem_fisica[i].em_uso == 0)
                {
                    buffer_mem_fisica[i].endereco_fisico = page;
                    buffer_mem_fisica[i].pag_modificada = (r_or_w == 'W') ? 1 : 0;
                    buffer_mem_fisica[i].pag_referenciada = 1;
                    buffer_mem_fisica[i].ultimo_acesso = time;
                    buffer_mem_fisica[i].em_uso = 1;
                    inserida = 1;
                    break;
                }
            }

            if (!inserida)
            {
                while (1)
                {
                    if (buffer_mem_fisica[ponteiro_relogio].pag_referenciada == 0)
                    {
                        if (buffer_mem_fisica[ponteiro_relogio].pag_modificada)
                            (*qtd_pgs_sujas)++;

                        buffer_mem_fisica[ponteiro_relogio].endereco_fisico = page;
                        buffer_mem_fisica[ponteiro_relogio].pag_modificada = (r_or_w == 'W') ? 1 : 0;
                        buffer_mem_fisica[ponteiro_relogio].pag_referenciada = 1;
                        buffer_mem_fisica[ponteiro_relogio].ultimo_acesso = time;

                        ponteiro_relogio = (ponteiro_relogio + 1) % param.qtd_pag_mem;
                        break;
                    }
                    else
                    {
                        buffer_mem_fisica[ponteiro_relogio].pag_referenciada = 0;
                        ponteiro_relogio = (ponteiro_relogio + 1) % param.qtd_pag_mem;
                    }
                }
            }

            (*qtd_pgs_faults)++;
        }

        time++;
    }

    fclose(arq_acessos);
}

void algoritmo_otimo(Param_Sim param, SimVirtual* buffer_mem_fisica, int* qtd_pgs_sujas, int* qtd_pgs_faults)
{
    FILE* arq_acessos;
    if (!(arq_acessos = fopen(param.nome_arquivo_acessos, "r")))
    {
        printf("Nao foi possivel abrir o arquivo %s!!!\n", param.nome_arquivo_acessos);
        exit(1);
    }

    unsigned addr_list[MAX_ACESSOS];
    char rw_list[MAX_ACESSOS];
    int total_acessos = 0;
    unsigned shift_page = log2(param.tam_page * 1024);
    
    while (fscanf(arq_acessos, "%x %c ", &addr_list[total_acessos], &rw_list[total_acessos]) == 2)
    {
        total_acessos++;
    }

    fclose(arq_acessos);

    for (int atual = 0; atual < total_acessos; atual++)
    {
        unsigned page = addr_list[atual] >> shift_page;
        char tipo = rw_list[atual];
        int is_page_in_mem = 0;

        for (int i = 0; i < param.qtd_pag_mem; i++)
        {
            if (buffer_mem_fisica[i].em_uso == 1 && buffer_mem_fisica[i].endereco_fisico == page)
            {
                buffer_mem_fisica[i].pag_referenciada = 1;
                if (tipo == 'W') buffer_mem_fisica[i].pag_modificada = 1;
                is_page_in_mem = 1;
                break;
            }
        }

        if (!is_page_in_mem)
        {
            int inserida = 0;
            for (int i = 0; i < param.qtd_pag_mem; i++)
            {
                if (!buffer_mem_fisica[i].em_uso)
                {
                    buffer_mem_fisica[i].endereco_fisico = page;
                    buffer_mem_fisica[i].pag_modificada = (tipo == 'W') ? 1 : 0;
                    buffer_mem_fisica[i].pag_referenciada = 1;
                    buffer_mem_fisica[i].em_uso = 1;
                    inserida = 1;
                    break;
                }
            }

            if (!inserida)
            {
                int mais_distante = -1, indice_vitima = -1;

                for (int i = 0; i < param.qtd_pag_mem; i++)
                {
                    int distancia = INT_MAX;
                    for (int j = atual + 1; j < total_acessos; j++)
                    {
                        if ((addr_list[j] >> shift_page) == buffer_mem_fisica[i].endereco_fisico)
                        {
                            distancia = j;
                            break;
                        }
                    }

                    if (distancia > mais_distante)
                    {
                        mais_distante = distancia;
                        indice_vitima = i;
                    }
                }

                if (buffer_mem_fisica[indice_vitima].pag_modificada)
                    (*qtd_pgs_sujas)++;

                buffer_mem_fisica[indice_vitima].endereco_fisico = page;
                buffer_mem_fisica[indice_vitima].pag_modificada = (tipo == 'W') ? 1 : 0;
                buffer_mem_fisica[indice_vitima].pag_referenciada = 1;
            }

            (*qtd_pgs_faults)++;
        }
    }
}
