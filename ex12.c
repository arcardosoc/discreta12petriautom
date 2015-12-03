/* Programa Rede de Petri com threads */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <allegro.h>

void inserirlista((void *)petri);
void desenhaauto((void *)petri);
void pthreadpetri((void *)petri);
typedef struct st_fim
{
    int tf;
    struct st_fim *prox;
}fim;

typedef struct st_transicao
{
    int de,meio,para;
    struct st_transicao *prox;
}transicao;

typedef struct st_automato
{ 
    int k, alf, si;
    fim lfim;
    transicao trans;
}petri;

int main(void)
{
    petri petri;
    inserirlista(&petri);
    desenhaauto(petri);
    return EXIT_SUCCESS;
}

void inserirlista((void *)petri)
{
    ;
}

void desenhaauto((void *)petri)
{
    ;
}

void pthreadpetri((void *)petri)
{
    ;
}
