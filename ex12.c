/* Programa Rede de Petri com threads */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <allegro.h>
#include <time.h>
#include <math.h>

#define FNAME "entrada-petri-1.txt"
#define VAZIO 0
#define X 800
#define Y 600
#define XCentro X/2.0
#define YCentro Y/2.0
#define IMAGENAME "ex12.bmp"
#define CORBRANCO (makecol(255,255,255))
#define CORPRETO 1
#define CORCINZA (makecol(160,160,160))
#define CORAZUL (makecol(0,0,255))
#define CORVERDE (makecol(0,255,0))
#define CORAMARELO (makecol(255,255,100))
#define CORVERMELHO (makecol(255,0,0))

#ifndef NMAX
#define NMAX 3 
#endif

#ifndef PAT
#define PAT 50
#endif

#ifndef DEBUG
#define DEBUG 5
#endif

typedef struct st_thread
{
    pthread_t thr;
    struct st_thread *prox;
}thread;

typedef struct st_arco
{
    int inicio,tkgp,final;
    struct st_arco *prox;
}arco;

typedef struct st_transicao
{
    int trans;
    arco *entram;
    arco *saem;
    struct st_transicao *prox;

}transicao;

typedef struct st_lugartoken
{
    int lu,tk;
    struct st_lugartoken *prox;
}lugartoken;

static struct st_lugartoken *lntk;
void *simupetri(void *trtemp);
void inserirlutk(lugartoken **cabeca,int lu,int tk);
void inserirtransicao(transicao **cabeca, int i);
void inserirentram(arco **cabeca,int inicio,int tkgp,int final);
void inserirpthread(thread **cabeca, pthread_t p);
void ativacaotransicao(arco *head,lugartoken **cabeca);
int retiratoken(lugartoken **cabeca, arco *head);
//void gerar_imagem(petri *p);
void desenha_estados(BITMAP *buff, int k);
//void desenha_transicoes(BITMAP *buff, transicaolugar *trans, int k, int c);
void desenha_arcos(int qo, int qf, BITMAP *buff, int k, int c, int flag);
float arctan(float x1, float y1, float x2, float y2);

int main(void)
{
    int i=0,k=0,lu,trans,tk,ql, qt, qk, al, at;
    FILE *fl= fopen(FNAME,"r+");
    transicao *tr = NULL;
    transicao *trtemp= NULL;
    thread *lthr = NULL;
    pthread_t thrtemp;
    srand(time(NULL));
    fscanf(fl,"%d",&ql);
    fscanf(fl,"%d",&qt);
    fscanf(fl,"%d",&qk);
    fscanf(fl,"%d",&al);
    fscanf(fl,"%d",&at);
    printf("Quantidade de Lugares:%d\nQuantidade de Transicoes:%d\nQuantidade de Lugares com Tokens:%d\nQuantidade de Arcos Lugares:%d\nQuantidade de Arcos Transicoes:%d\n",ql,qt,qk,al,at);
    for(i = 0;i < qk;i++)
    {
        fscanf(fl,"%d %d",&lu,&tk);
        for(trans = k; trans < ql ;trans++)
        {
            if(trans == lu)
            {
                if(DEBUG > 0)
                    printf("Lugar:%d/Tokens:%d\n",lu,tk);
                inserirlutk(&lntk,lu,tk);
                break;
            }
            else
            {
                if(DEBUG > 0)
                    printf("Lugar:%d/Tokens:0\n",trans);
                inserirlutk(&lntk,trans,VAZIO);
            }
        }
        k = trans+1;

    }
    if(k < ql)
        for(i = k;i < ql; i++)
        {
            if(DEBUG > 0)
                printf("Lugar:%d/Tokens:0\n",i);
            inserirlutk(&lntk,i,VAZIO);
        }
    for(i = 0;i < qt; i++)
        inserirtransicao(&tr,i);
    trtemp = tr;
    for(i=0;i<al;i++)
    {
        fscanf(fl,"%d %d %d",&lu,&tk,&trans);
        if(DEBUG > 0)
            printf("Lugar:%d---Tokens Perdidos:%d--->Transicao:%d\n",lu,tk,trans);        
        if(trtemp->trans == trans)
            inserirentram(&trtemp->entram,lu,tk,trans);
        else
        {
            trtemp = trtemp->prox;
            inserirentram(&trtemp->entram,lu,tk,trans);
        }
    }
    trtemp = tr;
    for(i=0;i < at;i++)
    {
        fscanf(fl,"%d %d %d",&trans,&tk,&lu);
        if(DEBUG > 0)
            printf("Transicao:%d---Tokens Ganhos:%d--->Lugar:%d\n",trans,tk,lu);
        if(trtemp->trans == trans)
            inserirentram(&trtemp->saem,trans,tk,lu);
        else
        {
            trtemp = trtemp->prox;
            inserirentram(&trtemp->saem,trans,tk,lu);
        }
    }
    i=0;
    //gerar_imagem(*p);
    printf("\n|============INICIO SIMULACAO============|\n");
    while(tr != NULL)
    {
        if(pthread_create(&thrtemp NULL, simupetri, (void *)tr))
        {
            printf("\nFalha ao criar thread!");
            return -1;
        }
        printf("Pthread[%d]: Criado com Sucesso\n",i);
        tr = tr->prox;
        inserirpthread(&lthr,thrtemp);
    }
    for(i = 0;i < qt;i++)
    {
        pthread_join(pthread[i],NULL);
        printf("Pthread[%d]: Fechado com Sucesso\n",i);
    }
    if(DEBUG > 0)
        while(lntk != NULL)
        {
            printf("Lugar:%d / Tokens Finais:%d\n",lntk->lu,lntk->tk);
            lntk=lntk->prox;
        }
    printf("|============FIM DA SIMULACAO============|\n\n");
    fclose(fl);
    return EXIT_SUCCESS;
}

/*void gerar_imagem(petri *p)
  { 
  BITMAP *buff;
  PALETTE pal;
  int k=1,flag;

  if(install_allegro(SYSTEM_NONE, &errno, atexit) !=0)
  exit(EXIT_FAILURE);
  set_color_depth(16);
  get_palette(pal);

  buff = create_bitmap(X,Y);
  if(buff == NULL)
  {
  printf("Nao foi possivel criar a imagem!\n");
  exit(EXIT_FAILURE);
  }
  desenha_estados(buff,p->ql);
  desenha_transicoes(buff, p->tralu,p->ql,p->qt);

  flag=1;
  while(p->lutra !=NULL)
  {
  desenha_arcos(p->lutra->li*2,p->lutra->tf+k,buff,p->ql*2,p->lutra->tkp,flag);
  p->lutra=p->lutra->prox;
  k++;

  }
  k=1;
  flag=0;
  while(p->tralu !=NULL )
  {
  desenha_arcos(p->tralu->ti+k,p->tralu->lf*2,buff,p->ql*2,p->tralu->tkg,flag);
  p->tralu=p->tralu->prox;
  k++;
  }

  save_bitmap(IMAGENAME, buff, pal);
  destroy_bitmap(buff);
  allegro_exit();

  printf("Imagem %s salva com sucesso!\n", IMAGENAME);
  }*/


void *simupetri(void *trtemp)
{
    int k,flag;
    transicao *tri = (transicao*)trtemp;
    transicao *tr=tri;
    printf("simupetri:%d\n",tr->trans);
    for(k = 0;k < NMAX;k++)
    {
        if(DEBUG > 4)
            printf("Pthread[%d]:\nInteracao[%d]:retirada de token\n\n",tr->trans,k);
        flag=retiratoken(&lntk,tr->entram);
        if(DEBUG > 4 && !flag)
            printf("Pthread[%d]:\nNao houve retirada de token\n\n",tr->trans);            
        if(rand()%100+1 < PAT && flag)
        {
            if(DEBUG > 4)
                printf("Pthread[%d]:\nInteracao[%d]:Transicao Ativada com Sucesso\n\n",tr->trans,k);
            ativacaotransicao(tr->saem,&lntk);
        }
    }
    pthread_exit(0);
}

void ativacaotransicao(arco *head,lugartoken **cabeca)
{
    lugartoken *pl=*cabeca;
    arco *pt=head;
    while(pt != NULL)
    {
        while(pl != NULL)
        {
            if(pt->final== pl->lu)
            {
                printf("pt->final:%d = pl->lu:%d\npl->tk:%d + pt->tkgp:%d\n",pt->final,pl->lu,pl->tk,pt->tkgp);
                pl->tk+=pt->tkgp;
            }
            pl = pl->prox;
        }
        pl=*cabeca;
        pt=pt->prox;
    }
    return ;
}
int retiratoken(lugartoken **cabeca, arco *head)
{
    lugartoken *pl=*cabeca;
    arco *pt=head;
    while(pl != NULL)
    {
        if(pt->inicio == pl->lu && pl->tk >= pt->tkgp)
        {
            printf("pt->inicio:%d = pl->lu:%d\npl->tk:%d - pt->tkgp:%d\n",pt->inicio,pl->lu,pl->tk,pt->tkgp);
            pl->tk-=pt->tkgp;
            return 1;
        }
        pl = pl->prox;
    }
    return 0;
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


void inserirtransicao(transicao **cabeca,int i)
{
    transicao *pl = *cabeca;
    transicao *plant = NULL;
    while(pl != NULL)
    {
        plant = pl;
        pl = pl->prox;
    }
    pl = malloc(sizeof(transicao));
    pl->trans = i;
    pl->prox = NULL;
    if(plant != NULL)
        plant->prox = pl;
    else
        *cabeca = pl;

    return;
}
void inserirentram(arco **cabeca,int inicio,int tkgp,int final)
{
    arco *pl = *cabeca;
    arco *plant = NULL;
    while(pl != NULL)
    {
        plant = pl;
        pl = pl->prox;
    }
    pl = malloc(sizeof(arco));
    pl->inicio = inicio;
    pl->tkgp = tkgp;
    pl->final = final;
    if(DEBUG > 2)
        printf("Pl->inicio:%d\nPl->tkgp:%d\nPl->final:%d\n",pl->inicio,pl->tkgp,pl->final);
    pl->prox = NULL;
    if(plant != NULL)
        plant->prox = pl;
    else
        *cabeca = pl;

    return;
}

void desenha_estados(BITMAP *buff, int k)
{
    int i;
    float raio,xi,yi,rc;
    raio= (Y/8) * (M_PI/(M_PI+k));
    rc = YCentro - raio*4;
    if(DEBUG)
        printf("Iniciando o desenho dos estados\n");
    for(i=0;i<k;i++)
    {
        yi=YCentro+rc*cos((2*M_PI/k)*i);
        xi=XCentro+rc*sin((2*M_PI/k)*i);
        circle(buff, xi, yi, raio, CORAZUL);
        textprintf_ex(buff, font, (xi-18), (yi-5), CORVERDE, CORPRETO, "Est %d",i);
    }
    return;
}

/*void desenha_transicoes(BITMAP *buff, transicaolugar *trans, int k , int c)
  {
  int i, l=0, j=1;
  float xi,yi,rc,raio;
  transicaolugar *pl=trans;
  raio = (Y/8)*(M_PI/(M_PI+k));
  rc = YCentro - raio*4;
  raio = (Y/12)*(M_PI/(M_PI+k));

  while(1)
  {
  for(i=j;i<c*2;i++)
  {
  yi=YCentro+rc*cos((2*M_PI/(k*2))*i);
  xi=XCentro+rc*sin((2*M_PI/(k*2))*i);
  line(buff, (xi), (yi)+raio, (xi), (yi)-raio, CORVERMELHO);
  if(M_PI/2<=(2*M_PI/(k*2))*i && (3*M_PI)/2>(2*M_PI/(k*2))*i)
  textprintf_ex(buff, font, xi-10, yi-raio-12, CORVERDE, CORPRETO, "Tr%d",l++);
  else
  {
  textprintf_ex(buff, font, xi-10, yi+raio+5, CORVERDE, CORPRETO, "Tr%d",l++);
  break;
  }
  i++;
  }
  k = i+2;
  if(pl->prox!=NULL)
  {
  pl=pl->prox;
  }
  else
  break;
  }
  }*/

void desenha_arcos(int qo, int qf, BITMAP *buff, int k, int c, int flag)
{
    float delta, alfa, beta, phi, x1, y1, x2, y2, x3, y3, xo, yo, xf, yf, raio, xt1, yt1, xt2, yt2, rc;
    raio=(Y/8)*(M_PI/(M_PI+(k/2)));
    rc = YCentro - raio*4;

    y1 = YCentro + rc*cos((2*M_PI/k)*qo);
    x1 = XCentro + rc*sin((2*M_PI/k)*qo);
    y3 = YCentro + rc*cos((2*M_PI/k)*qf);
    x3 = XCentro + rc*sin((2*M_PI/k)*qf);

    alfa=arctan(x1,y1,x3,y3);
    y2=(y3+y1)/2 + raio * cos(alfa);
    x2=(x3+x1)/2 - raio * sin(alfa);

    if(((alfa >= 0) && (alfa <= M_PI/2)) || ((alfa >= M_PI) && (alfa <= 3*M_PI/2)))
    {
        beta=arctan(x3,y3,x2,y2);
        phi=arctan(x1,y1,x2,y2);
        xo = x1 + raio * cos(phi);
        yo = y1 + raio * sin(phi);
        xf = x3 + raio * cos(beta);
        yf = y3 + raio * sin(beta);
    }
    else
    {
        alfa=arctan(x3,y3,x1,y1);
        y2=(y3+y1)/2 + raio * cos(alfa);
        x2=(x3+x1)/2 - raio * sin(alfa);
        beta=arctan(x1,y1,x2,y2);
        phi=arctan(x3,y3,x2,y2);
        if(flag)
        {
            xo = x1 - raio * cos(phi);
            yo = y1 - raio * sin(phi);
            xf = x3;
            yf = y3;
        }
        else
        {
            xo = x1;
            yo = y1;
            xf = x3 - raio * cos(beta);
            yf = y3 - raio * sin(beta);
        }
    }
    int coo[8];
    coo[0] = (int)xo;
    coo[1] = (int)yo;
    coo[2] = (int)x2;
    coo[3] = (int)y2;
    coo[4] = (int)x2;
    coo[5] = (int)y2;
    coo[6] = (int)xf;
    coo[7] = (int)yf;
    spline(buff,coo,CORBRANCO);

    delta=arctan(x2,y2,x3,y3);
    xt2 = xf - (raio / 4) * (sin(delta) + cos(delta));
    yt2 = yf + (raio / 4) * (sin(delta) - cos(delta));
    xt1 = xf + (raio / 4) * (sin(delta) - cos(delta));
    yt1 = yf - (raio / 4) * (sin(delta) + cos(delta));

    triangle(buff, xt1, yt1, xt2, yt2, xf, yf, CORBRANCO);
    textprintf_ex(buff, font, x2, y2, CORVERDE, CORPRETO, "%d", c);

}

float arctan(float x1, float y1, float x2, float y2)
{
    if(x2 == x1)
    {
        if(y2 == y1)
            return 9.0;
        else
            if(y2>y1)
                return M_PI/2.0;
        return 3.0*M_PI/2.0;
    }
    if((y2 == y1) && (x2 < x1))
        return M_PI;
    float a = atan(fabs(y2-y1)/fabs(x2-x1));
    if((x2 < x1) && (y2 > y1)) // QUAD = 2
        return a + M_PI/2.0;
    if((x2 < x1) && (y2 < y1)) // QUAD = 3
        return a + M_PI;
    if((x2 > x1) && (y2 < y1)) // QUAD = 4
        return a + 3.0*M_PI/2.0;
    return a; // QUAD = 1
}

void inserirpthread(thread **cabeca, pthread_t p)
{
    thread *pl = *cabeca;
    thread *plant = NULL;

    while(pl != NULL)
    {
        plant = pl;
        pl = pl -> prox;
    }

    pl = malloc(sizeof(thread));
    pl -> thr = p;
    pl -> prox = NULL;

    if(plant != NULL)
        plant -> prox = pl;
    else
        *cabeca = pl;
}

