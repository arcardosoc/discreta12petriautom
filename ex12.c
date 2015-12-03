/* Programa Rede de Petri com threads */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <allegro.h>

void inserirlista(petri **pet);
void desenhaauto(petri *pet);
void pthreadpetri(void);
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

typedef struct st_petri
{ 
    int k, alf, si;
    fim lfim;
    transicao trans;
}petri;

int main(void)
{
    petri pet;
    inserirlista(&pet);
    desenhaauto(pet);
    return EXIT_SUCCESS;
}

void inserirlista(petri **pet)
{
    ;
}

void desenhaauto(petri *pet)
{
    ;
}

void pthreadpetri(void)
{
    ;
}
