/* Programa Rede de Petri com threads */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <allegro.h>
#include <time.h>
#include <math.h>

#define FNAME "entrada-petri-1.txt"
#define VAZIO 0
#define Xtela 800
#define Ytela 800

#ifndef NMAX
#define NMAX 3
#endif

#ifndef PAT
#define PAT 50
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

typedef struct st_lugartransicao
{
    int li, tf, tkp, key;
    struct st_lugartransicao *prox;
}lugartransicao;

typedef struct st_transicaolugar
{
    int lf, ti, tkg;
    struct st_transicaolugar *prox;
}transicaolugar;

typedef struct st_lugartoken
{
    int lu,tk;
    struct st_lugartoken *prox;
}lugartoken;

typedef struct st_petri
{
    int ql, qt, qk, al, at,key;
    lugartoken *lntk;
    lugartransicao *lutra;
    transicaolugar *tralu;
}petri;

static struct st_petri *p = NULL;
void desenhaauto(petri p);
void *simupetri(void *i);
void inserirlutk(lugartoken **cabeca,int lu,int tk);
void inserirlutra(lugartransicao **cabeca,int lu,int tk,int trans,int key);
void inserirtralu(transicaolugar **cabeca,int trans,int tk,int lu);
void ativacaotransicao(transicaolugar *cabeca,lugartoken *pt,int ti);
int retiratoken(lugartoken **cabeca, int lu, int tk);
void addtoken(lugartoken **cabeca, int lu, int tk);
void adicionai(petri **cabeca,int i);

int main(void)
{
    int i,k=0,lu,trans,tk;
    FILE *fl= fopen(FNAME,"r+");
    pthread_t pthread[NMAX];
    p = malloc(sizeof(petri));
    srand(time(NULL));
    p->lntk  = NULL;
    p->lutra = NULL;
    p->tralu = NULL;
    fscanf(fl,"%d",&(p->ql));
    fscanf(fl,"%d",&(p->qt));
    fscanf(fl,"%d",&(p->qk));
    fscanf(fl,"%d",&(p->al));
    fscanf(fl,"%d",&(p->at));
    printf("Quantidade de Lugares:%d\nQuantidade de Transicoes:%d\nQuantidade de Lugares com Tokens:%d\nQuantidade de Arcos Lugares:%d\nQuantidade de Arcos Transicoes:%d\n",p->ql,p->qt,p->qk,p->al,p->at);
    for(i = 0;i < p->qk;i++)
    {
        fscanf(fl,"%d %d",&lu,&tk);
        for(trans = k; trans < p->ql ;trans++)
        {
            if(trans == lu)
            {
                if(DEBUG > 0)
                    printf("Lugar:%d/Tokens:%d\n",lu,tk);
                inserirlutk(&p->lntk,lu,tk);
                break;
            }
            else
            {
                if(DEBUG > 0)
                    printf("Lugar:%d/Tokens:0\n",trans);
                inserirlutk(&p->lntk,trans,VAZIO);
            }
        }
        k = trans+1;

    }
    if(k < p->ql)
        for(i = k;i < p->ql; i++)
        {
            if(DEBUG > 0)
                printf("Lugar:%d/Tokens:0\n",i);
            inserirlutk(&p->lntk,i,VAZIO);
        }
    for(i=0;i<p->al;i++)
    {
        fscanf(fl,"%d %d %d",&lu,&tk,&trans);
        if(DEBUG > 0)
            printf("Lugar:%d---Tokens Perdidos:%d--->Transicao:%d\n",lu,tk,trans);
        inserirlutra(&p->lutra,lu,tk,trans,i);
    }
    for(i=0;i<p->at;i++)
    {
        fscanf(fl,"%d %d %d",&trans,&tk,&lu);
        if(DEBUG > 0)
            printf("Transicao:%d---Tokens Ganhos:%d--->Lugar:%d\n",trans,tk,lu);
        inserirtralu(&p->tralu,trans,tk,lu);
    }
    //desenhaauto(*p);
    printf("\n|============INICIO SIMULACAO============|\n");
    for(i=0;i < p->al;i++)
    {
        if(pthread_create(&pthread[i], NULL, simupetri, (void *)&i))
        {
            printf("\nFalha ao criar thread!");
            return -1;
        }
        printf("Pthread[%d]: Criado com Sucesso\n",i);
    }
    for(i = 0;i < p->al;i++)
    {
        pthread_join(pthread[i],NULL);
        printf("Pthread[%d]: Fechado com Sucesso\n",i);
    }
    printf("|============FIM DA SIMULACAO============|\n\n");
    fclose(fl);
    return EXIT_SUCCESS;
}

/*void desenhaauto(petri p)
  {
  int i;
  float rc,yi,xi,xc,yc;
  rc=5;
  yi=20;
  xi=30;
  BITMAP *img;
  PALETTE pal;
  if(allegro_init() !=0)
  exit(1);
  set_color_depth(16);
  get_palette(pal);
  img = create_bitmap(Xtela,Ytela);
  if(img == (NULL))
  {
  printf("Nao pode criar a imagem");
  exit(1);
  }
  for(i=0; i < p.ql; i++)
  {
  yc=yi+rc*sin(((2*M_PI)/p.ql)*i);
  xc=xi+rc*cos(((2*M_PI)/p.ql)*i);
  }

  save_bitmap("petri.bmp",img,pal);
  destroy_bitmap(img);
  allegro_exit();
  return ;
  ;
  }*/

void *simupetri(void *i)
{
    int k,flag,key =*((int *)i);
    printf("key:%d\n",key);
    while(p->lutra->key != key)
    {
        p->lutra = p->lutra->prox;
        printf("p->lutra:%d//key:%d\n",p->lutra->key,key);
    }
    for(k = 0;k < NMAX;k++)
    {
        if(DEBUG > 4)
            printf("Pthread[%d]//Interacao[%d]:retirada de token\n",p->lutra->key,k);
        flag=retiratoken(&p->lntk,p->lutra->li,p->lutra->tkp);
        if(DEBUG > 4 && !flag)
            printf("Nao houve retirada de token\n");
        if(rand()%100+1 < PAT && flag)
        {
            if(DEBUG > 4)
                printf("Pthread[%d]//Interacao[%d]:Transicao Ativada com Sucesso\n",p->lutra->key,k);
            ativacaotransicao(p->tralu,p->lntk,p->lutra->tf);
        }
    }
    pthread_exit(0);
}

void ativacaotransicao(transicaolugar *cabeca,lugartoken *pt,int ti)
{
    transicaolugar *pl=cabeca;
    while(pl!=NULL)
    {
        if(pl->ti == ti)
        {
            if(DEBUG > 4)
                printf("Ativado Transicao:%d--->Lugar:%d\n",pl->ti,pl->lf);
            addtoken(&pt,pl->lf,pl->tkg);
        }
        pl=pl->prox;
    }
    return;
}

int retiratoken(lugartoken **cabeca, int lu, int tk)
{
    lugartoken *pl=*cabeca;
    while(pl!=NULL)
    {
        if(pl->lu == lu && pl->tk >= tk)
        {
            if(DEBUG > 2)
                printf("Tko:%d - ",pl->tk);
            pl->tk-=tk;
            if(DEBUG > 2)
                printf("Tk:%d = Tkf:%d\n",tk,pl->tk);
            return 1;
        }
        pl=pl->prox;
    }
    return 0;
}

void addtoken(lugartoken **cabeca, int lu, int tk)
{
    lugartoken *pl=*cabeca;
    while(pl!=NULL)
    {
        if(pl->lu == lu)
        {
            if(DEBUG > 2)
                printf("Tko:%d + ",pl->tk);
            pl->tk+=tk;
            if(DEBUG > 2)
                printf("Tk:%d = Tkf:%d\n",tk,pl->tk);
            break;
        }
        pl=pl->prox;
    }
    return;
}

void inserirlutk(lugartoken **cabeca,int lu,int tk)
{
    lugartoken *pl = *cabeca;
    lugartoken *plant = NULL;
    while(pl != NULL)
    {
        plant = pl;
        pl = pl->prox;
    }
    pl = malloc(sizeof(lugartoken));
    pl->lu = lu;
    pl->tk = tk;
    if(DEBUG > 2)
        printf("Pl->lu:%d\nPl->tk:%d\n",pl->lu,pl->tk);
    pl->prox = NULL;
    if(plant != NULL)
        plant->prox = pl;
    else
        *cabeca = pl;

    return;
}

void inserirlutra(lugartransicao **cabeca,int lu,int tk,int trans,int key)
{
    lugartransicao *pl = *cabeca;
    lugartransicao *plant = NULL;
    while(pl != NULL)
    {
        plant = pl;
        pl = pl->prox;
    }
    pl = malloc(sizeof(lugartransicao));
    pl->li = lu ;
    pl->tkp = tk;
    pl->tf = trans;
    pl->key = key;
    if(DEBUG > 2)
        printf("Pl->li:%d\nPl->tkp:%d\nPl->tf:%d\nPl->key:%d\n",pl->li,pl->tkp,pl->tf,pl->key);
    pl->prox = NULL;
    if(plant != NULL)
        plant->prox = pl;
    else
        *cabeca = pl;

    return;
}

void inserirtralu(transicaolugar **cabeca,int trans,int tk,int lu)
{
    transicaolugar *pl = *cabeca;
    transicaolugar *plant = NULL;
    while(pl != NULL)
    {
        plant = pl;
        pl = pl->prox;
    }
    pl = malloc(sizeof(transicaolugar));
    pl->ti = trans;
    pl->tkg = tk;
    pl->lf = lu;
    if(DEBUG > 2)
        printf("Pl->ti:%d\nPl->tkg:%d\nPl->lf:%d\n",pl->ti,pl->tkg,pl->lf);
    pl->prox = NULL;
    if(plant != NULL)
        plant->prox = pl;
    else
        *cabeca = pl;

    return;
}
