/* Programa Rede de Petri com threads */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <allegro.h>

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
}p;

void inserirlista(p **pet);
void desenhaauto(p *pet);
void pthreadpetri(void);

int main(void)
{
    p *pet=NULL;
    inserirlista(&pet);
    desenhaauto(pet);
    return EXIT_SUCCESS;
}

void inserirlista(p **pet)
{
    ;
}

void desenhaauto(p *pet)
{
    ;
}

void pthreadpetri(void)
{
    ;
}
