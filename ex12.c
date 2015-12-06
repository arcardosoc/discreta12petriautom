/* Programa Rede de Petri com threads */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <allegro.h>

typedef struct st_lugartransicao
{
    int li, tf;
    struct st_lugartransicao *prox;
}lugartransicao;
typedef struct st_transicaolugar
{
    int lf, ti;
    struct st_transicaolugar *prox;
}transicaolugar;

typedef struct st_lugartoken
{
    int lu,tk;
    struct st_lugartoken *prox;
}lugartoken;

typedef struct st_petri
{
    int ql, qt, qk, al, at;
    lugartoken lntk;
    lugartransicao lutra;
    transicaolugar tralu;
}p;

void inserirlista(p **pet);
void desenhaauto(p *pet);
void simupetri(void);

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
