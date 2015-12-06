/* Programa Rede de Petri com threads */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <allegro.h>

#define FNAME "entrada-petri-1.txt"

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
}petri;

void desenhaauto(petri p);
void simupetri(void);
void inserirlutk(lugartoken **cabeca,int lu,int tk);
void inserirlutra(lugartransicao **cabeca,int lu,int tk,int trans);
void inserirtralu(transicaolugar **cabeca,int trans,int tk,int lu);

int main(void)
{
    int i,lu,trans,tk;
    FILE *fl= fopen(FNAME,"r+");
    lugartoken *pt = NULL;
    lugartransicao *plt = NULL;
    transicaolugar *ptl = NULL;
    petri p;
    p.lntk.prox = NULL;
    p.lutra.prox = NULL;
    p.tralu.prox = NULL;
    fscanf(fl,"%d",&(p.ql));
    fscanf(fl,"%d",&(p.qt));
    fscanf(fl,"%d",&(p.qk));
    fscanf(fl,"%d",&(p.al));
    fscanf(fl,"%d",&(p.at));
    for(i=0;i<p.qk;i++)
    {
        fscanf(fl,"%d %d",&lu,&tk);
        pt = &(p.lntk);
        inserirlutk(&pt,lu,tk);
    }
    for(i=0;i<p.al;i++)
    {
        fscanf(fl,"%d %d %d",&lu,&tk,&trans);
        plt = &(p.lutra);
        inserirlutra(&plt,lu,tk,trans);
    }
    for(i=0;i<p.at;i++)
    {
        fscanf(fl,"%d %d %d",&trans,&tk,&lu);
        ptl = &(p.tralu);
        inserirtralu(&ptl,trans,tk,lu);
    }
    desenhaauto(p);
    return EXIT_SUCCESS;
}


void desenhaauto(petri p)
{
    ;
}

void simupetri(void)
{
    ;
}

void inserirlutk(lugartoken **cabeca,int lu,int tk)
{
    ;
}

void inserirlutra(lugartransicao **cabeca,int lu,int tk,int trans)
{
    ;
}

void inserirtralu(transicaolugar **cabeca,int trans,int tk,int lu)
{
    ;
}
