/* **************************************************************************** * 
 *    Ex12.c, v2.0                                                              * 
 *    Simulador de rede de petri utilizando paralelismo com threads e           *
 *    allegro para gerar imagem da rede.                                        *
 *                                                                              *
 *                                                                              *
 *    Copyright (C) 2015 by Arthur Carvalho de Albuquerque Cardoso              *
 *                          Mateus Lenier Rezende                               *
 *                                                                              *
 *    This program is free software; you can redistribute it and/or modify      *
 *    it under the terms of the GNU General Public License as published by      *
 *    the Free Software Foundation; either version 2 of the License, or         *
 *    (at your option) any later version.                                       *
 *                                                                              *
 *    This program is distributed in the hope that it will be useful,           *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *    GNU General Public License for more details.                              *
 *                                                                              *
 *    You should have received a copy of the GNU General Public License         *
 *    along with this program; if not, write to the                             *
 *    Free Software Foundation, Inc.,                                           *
 *    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                 *
 *                                                                              *
 *    To contact the author, please write to:                                   *
 *                                                                              *
 *    Arthur Carvalho de Albuquerque Cardoso  <arthurcardoso2005@hotmail.com>   *
 *    Phone: +55 (81) 99974-9388                                                *
 *                                                                              *
 *    Mateus Lenier Rezende <mateuslenier@gmail.com>                            *
 *    Phone +55 (81) 99891-2596                                                 *
 * **************************************************************************** *
 *
 */

/**
* \file ex12.c
* \brief simulador de rede de petri
* \author Arthur Carvalho de Albuquerque Cardoso <<arthurcardoso2005@hotmail.com>>
* \author Mateus Lenier Rezende <<mateuslenier@gmail.com>>
* \version 2.0
* \date 2015-12-20
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <allegro.h>
#include <time.h>
#include <math.h>

#define FNAME "entrada-petri-1.txt" /**< Entrada de dados. */
#define VAZIO 0 /**< Define para nao ter numero magico, caso lugar com 0 token. */
#define X 800 /**< Tamanho X da tela. */
#define Y 600 /**< Tamanho Y da tela. */
#define XCentro X/2.0 /**< Posicao X do centro da circunferencia. */
#define YCentro Y/2.0 /**< Posicao Y do centro da circunferencia. */
#define IMAGENAME "ex12.bmp" /**< Imagem gerada pelo allegro. */
#define CORBRANCO (makecol(255,255,255)) /**< Macro para cor branca. */
#define CORPRETO 1 /**< Macro para cor preta. */
#define CORCINZA (makecol(160,160,160)) /**< Macro para cor cinza. */
#define CORAZUL (makecol(0,0,255)) /**< Macro para cor azul. */
#define CORVERDE (makecol(0,255,0)) /**< Macro para cor verde. */
#define CORAMARELO (makecol(255,255,100)) /**< Macro para cor amarelo. */
#define CORVERMELHO (makecol(255,0,0)) /**< Macro para cor vermelho. */

#ifndef NMAX
#define NMAX 1000 /**< Numero de interacoes petri. */
#endif

#ifndef PAT
#define PAT 50 /**< Porcentagem de ativacao da transicao. */
#endif

#ifndef DEBUG
#define DEBUG 0 /**< Ativa DEBUG. */
#endif

typedef struct st_thread /** Struct utilizada na criação de uma lista pthread. */
{
    pthread_t thr;
    struct st_thread *prox;
}thread;

typedef struct st_arco /** Struct utilizada na criação de duas listas: na lista de arco lugar-transição e na lista de transição-lugar. */
{
    int inicio,tkgp,final;
    struct st_arco *prox;
}arco;

typedef struct st_transicao /** Struct utilizada na criação de uma lista de transições e que tambem possui duas listas, de arcos que entram e que saem da transição. */
{
    int trans;
    arco *entram;
    arco *saem;
    struct st_transicao *prox;

}transicao;

typedef struct st_lugartoken /** Struct utilizada no armazenamento da quantidade de tokens em função do lugar. */
{
    int lu,tk;
    struct st_lugartoken *prox;
}lugartoken;

static struct st_lugartoken *lntk;
void *simupetri(void *trtemp);
void inserirlutk(lugartoken **cabeca,int lu,int tk); /**< Função para inserir novos lugarestokens na lista. */
void inserirtransicao(transicao **cabeca, int i); /** Função para inserir novas transições na lista. */
void inserirentram(arco **cabeca,int inicio,int tkgp,int final); /** Função para inserir novos arcos na lista, tanto arco lugar-transição quanto transição-lugar. */
void inserirpthread(thread **cabeca, pthread_t p); /** Função para inserir novas pthreads na lista. */
void ativacaotransicao(arco *head,lugartoken **cabeca); /** Função que simula a ativação das transiçôes na rede de petri. */
void retiratoken(lugartoken **cabeca, arco *head,arco *kopf);
void gerar_imagem(transicao *tr,int ql, int qt); /** Função utilizada para desenhar a rede de petri utilizando a biblioteca do allegro. */
void desenha_estados(BITMAP *buff, int k); /** Função para desenhar os lugares da rede de petri. */
void desenha_transicoes(BITMAP *buff, transicao *trans, int k, int c); /** Função para desenhar as transições da rede de petri. */
void desenha_arcos(int qo, int qf, BITMAP *buff, int k, int c, int flag); /** Funções utilizadas para desenhar os arcos da rede de petri. */
float alsin(float x1, float y1, float x2, float y2); /** Função que calcula um seno que será utilizado no desenho da seta. */
float alcos(float x1, float y1, float x2, float y2); /** Função que calcula um cosseno que será utilizado no desenho da seta. */
float arctan(float x1, float y1, float x2, float y2); /** Função que calcula o arcotangente para todos os quadrantes de forma individual. */

int main(void)
{
    int i=0,k=0,lu,trans,tk,ql, qt, qk, al, at;
    FILE *fl= fopen(FNAME,"r+");
    transicao *tr = NULL; /** Inicialização do ponteiro apontando para nulo. */
    transicao *trtemp= NULL; /** Inicialização do ponteiro apontando para nulo. */

    thread *lthr = NULL; /** Inicialização do ponteiro apontando para nulo. */
    pthread_t thrtemp;
    srand(time(NULL));
    fscanf(fl,"%d",&ql); /** Armazenamento da quantidade de lugares na variavel ql. */
    fscanf(fl,"%d",&qt); /** Armazenamento da quantidade de transições na variavel qt. */
    fscanf(fl,"%d",&qk); /** Armazenamento da quantidade de lugares com tokens na variavel qk. */
    fscanf(fl,"%d",&al); /** Armazenamento da quantidade de arcos lugar-transição na variavel al. */
    fscanf(fl,"%d",&at); /** Armazenamento da quantidade de arcos transição-lugar na variavel at. */
    printf("Quantidade de Lugares:%d\nQuantidade de Transicoes:%d\nQuantidade de Lugares com Tokens:%d\nQuantidade de Arcos Lugares:%d\nQuantidade de Arcos Transicoes:%d\n",ql,qt,qk,al,at);
    for(i = 0;i < qk;i++) /** Nesse laço é criado a lista lugar token especifcado anteriormente, ele é delimitado pela quantidade de lugares com tokens. */
    {
        fscanf(fl,"%d %d",&lu,&tk);
        for(trans = k; trans < ql ;trans++) /** Nesse laço sao criadas structs vazias até que ocorra uma igualdade entre o local de armazenamento e o local com token. */
        {
            if(trans == lu) /** Condição de igualdade para se criar um lugar com tokens diferentes de zero. */
            {
                if(DEBUG > 0)
                    printf("Lugar:%d/Tokens:%d\n",lu,tk);
                inserirlutk(&lntk,lu,tk); /** Chamada para a função inserirlutk. */
                break;
            }
            else
            {
                if(DEBUG > 0)
                    printf("Lugar:%d/Tokens:0\n",trans);
                inserirlutk(&lntk,trans,VAZIO); /** Chamada para a função inserirlutk. */
            }
        }
        k = trans+1;

    }
    if(k < ql) /** Caso o ultimo lugar token não seja o ultimo da lista de transições, ele entre nessa condição. */
        for(i = k;i < ql; i++) /** Preenchendo os ultimos espaços da lista com 0. */
        {
            if(DEBUG > 0)
                printf("Lugar:%d/Tokens:0\n",i);
            inserirlutk(&lntk,i,VAZIO);
        }
    for(i = 0;i < qt; i++) /** Criação da lista de transições delimitadas pela variavel qt. */
        inserirtransicao(&tr,i); /** Chamada para a função inserirtransição. */
    trtemp = tr; /** Variavel temporaria para não modificar a posição da cabeça. */
    for(i=0;i<al;i++) /** Criação da lista de arcos lugar transição. */
    {
        fscanf(fl,"%d %d %d",&lu,&tk,&trans);
        if(DEBUG > 0)
            printf("Lugar:%d---Tokens Perdidos:%d--->Transicao:%d\n",lu,tk,trans);        
        if(trtemp->trans == trans) /** Condição para cirar as listas de acordo com cada transição. */
            inserirentram(&trtemp->entram,lu,tk,trans); /** Chamada para a função inserirentram. */
        else
        {
            trtemp = trtemp->prox; /** Método utilizado para transitar pela lista transição. */
            inserirentram(&trtemp->entram,lu,tk,trans); /** Chamada para a função inserirentram. */
        }
    }
    trtemp = tr; /** Variavel temporaria para não modificar a posição da cabeça. */
    for(i=0;i < at;i++) /** Criação da lista de arcos transição lugar. */
    {
        fscanf(fl,"%d %d %d",&trans,&tk,&lu);
        if(DEBUG > 0)
            printf("Transicao:%d---Tokens Ganhos:%d--->Lugar:%d\n",trans,tk,lu);
        if(trtemp->trans == trans) /** Condição para cirar as listas de acordo com cada transição. */
            inserirentram(&trtemp->saem,trans,tk,lu); /** Chamada para a função inserirentram. */
        else
        {
            trtemp = trtemp->prox; /** Método utilizado para transitar pela lista transição. */
            inserirentram(&trtemp->saem,trans,tk,lu); /** Chamada para a função inserirentram. */
        }
    }
    gerar_imagem(tr, ql , qt); /** Chamada para a função gerar_imagem. */
    printf("\n|============INICIO SIMULACAO============|\n");
    while(tr != NULL) /** Condição que cria n pthreads de acordo com a quantidades transições do problema. */
    {
        if(pthread_create(&thrtemp,NULL, simupetri, (void *)tr)) /** Cria uma nova thread e verifica se não ocorreu nenhum problema. */
        {
            printf("\nFalha ao criar thread!");
            return -1;
        }
        tr = tr->prox; /** Método utilizado para transitar pela lista transição. */
        inserirpthread(&lthr,thrtemp); /** Chamada para a função inserirpthread. */
    }
    while(lthr != NULL) /** Condição que fecha as threads anteriormente abertas de acordo com o numero de threads na lista lthr. */
    {
        thrtemp = lthr->thr; /** Atualização da variavel thrtemp para fechar as diversas threads criadas. */
        if(pthread_join(thrtemp,NULL)) /** Fecha as threads e verifica se não ocorreu nenhum problema. */
        {
            printf("\nFalha ao fechar thread!\n");
            return -1;
        }
        lthr = lthr->prox; /** Método utilizado para transitar pela lista lthr. */
    }
    if(DEBUG > 0)
        while(lntk != NULL) /** Condição que permite transitar por toda a lista lugartoken. */
        {
            printf("Lugar:%d / Tokens Finais:%d\n",lntk->lu,lntk->tk);
            lntk=lntk->prox; /** Método utilizado para transitar pela lista Lugartoken. */
        }
    printf("|============FIM DA SIMULACAO============|\n\n");
    fclose(fl);
    return EXIT_SUCCESS;
}

void gerar_imagem(transicao *tr, int ql, int qt) /** Função utilizada para desenhar a rede de petri utilizando a biblioteca do allegro. */
{ 
    BITMAP *buff;
    PALETTE pal;
    arco *pt = tr->entram; /** Variavel temporaria para não modificar a posição da cabeça. */
    arco *pl = tr->saem; /** Variavel temporaria para não modificar a posição da cabeça. */
    int k=1,flag;

    if(install_allegro(SYSTEM_NONE, &errno, atexit) !=0) /** Inicialização do allegro e com verificação de falhas. */
        exit(EXIT_FAILURE);
    set_color_depth(16); /** Setagem no numero de cores. */
    get_palette(pal);

    buff = create_bitmap(X,Y); /** Criação da imagem de tamanho X e Y, que foram previamentes definidos. */
    if(buff == NULL) /** Verificação de falhas na criação da imagem. */
    {
        printf("Nao foi possivel criar a imagem!\n");
        exit(EXIT_FAILURE);
    }
    desenha_estados(buff,ql); /** Chamada da função desenha_estados. */
    desenha_transicoes(buff, tr, ql, qt); /** Chamada da função desenha_transições. */


    while(1) /** Laço utilizado para transitar na lista de transições. */
    {
        flag=1; /** Flag que será usada como comparativo na função desenha_arcos. */
        while(pt !=NULL) /** Laço utilizado para transitar na lista de arco entram. */
        {
            desenha_arcos(pt->inicio*2,pt->final+k,buff,ql*2,pt->tkgp,flag); /** Chamada da função desenha arcos voltada para arcos lugar-transição. */
            pt = pt->prox; /** Método utilizado para transitar pela lista arco entram. */
        }
        flag=0; /** Flag que será usada como comparativo na função desenha_arcos. */
        while(pl !=NULL ) /** Laço utilizado para transitar na lista de arco saem. */
        {
            desenha_arcos(pl->inicio+k,pl->final*2,buff,ql*2,pl->tkgp,flag); /** Chamada da função desenha arcos voltada para arcos transição-lugar. */
            pl = pl->prox; /** Método utilizado para transitar pela lista arco saem. */
        }
        k++; /** Variavel utilizada para que osvalores das transições sempre entrem como impares nas funções. */
        tr = tr->prox; /** Método utilizado para transitar pela lista transição. */
        if(tr == NULL) /** Codição de verificação para ver se a lista transição acabou. */
            break;
        pt = tr->entram; /** Voltar o ponteiro para o inicio da lista arco entram da nova transição. */
        pl = tr->saem; /** Voltar o ponteiro para o inicio da lista arco saem da nova transição. */

    }
    save_bitmap(IMAGENAME, buff, pal); /** Salvar a imagem desenhada. */
    destroy_bitmap(buff); 
    allegro_exit(); /** Terminar o allegro. */

    printf("Imagem %s salva com sucesso!\n", IMAGENAME);
}


void *simupetri(void *trtemp) /** Função para simular a rede de petri. */
{
    int k;
    transicao *tr = (transicao*)trtemp; /** Conversão e armazenamento da variavel trtemp em tr. */
    for(k = 0;k < NMAX;k++) /** Simulção das interações da rede de petri. */
    {
        if(DEBUG > 4)
            printf("Pthread[%d]:\nInteracao[%d]:retirada de token\n\n",tr->trans,k);
        retiratoken(&lntk,tr->entram,tr->saem); /** Chamada da função retiratoken. */
    }
    pthread_exit(0); /** Saida da thread, retornando nenhum valor. */
}

void ativacaotransicao(arco *head,lugartoken **cabeca) /** Função que simula a ativação das transiçôes na rede de petri. */
{
    lugartoken *pl=*cabeca; /** Variavel temporaria para não modificar a posição da cabeça. */
    arco *pt=head; /** Variavel temporaria para não modificar a posição da cabeça. */
    while(pt != NULL) /** Laço utilizado para transitar na lista de arco saem. */
    {
        while(pl != NULL) /** Laço utilizado para transitar na lista de lugartoken. */
        {
            if(pt->final== pl->lu) /** Condição de igualdade entre lugar final do arco transição-lugar e lugar da lista lugartoken. */
            {
                    printf("pt->final:%d = pl->lu:%d\npl->tk:%d + pt->tkgp:%d\n",pt->final,pl->lu,pl->tk,pt->tkgp);
                    pl->tk+=pt->tkgp; /** Adição do numero de tokens de acordo com o numero de tokens ganhos com o arco transição-lugar. */
            }
            pl = pl->prox; /** Método utilizado para transitar pela lista lugartoken. */
        }
        pl=*cabeca; /** Voltar o ponteiro para o inicio da lista lugartoken. */
        pt=pt->prox; /** Método utilizado para transitar pela lista arco saem. */
    }
    return ;
}

void retiratoken(lugartoken **cabeca, arco *head, arco *kopf)
{
    lugartoken *pl=*cabeca; /** Variavel temporaria para não modificar a posição da cabeça. */
    arco *pt=head; /** Variavel temporaria para não modificar a posição da cabeça. */
    while(pt != NULL) /** Laço utilizado para transitar na lista de arco saem. */
    {
        while(pl != NULL) /** Laço utilizado para transitar na lista de arco saem. */
        {
            if(pt->inicio == pl->lu && pl->tk >= pt->tkgp) /** Condição de igualdade entre lugar incial do arco lugar-transição e lugar da lista de tokens e verificação para saber se o numero de tokens no lugar é maior ou igual ao numero de tokens pedidos pelo arco lugar-transição. */
            {
                if(DEBUG > 4 )
                    printf("Pthread[%d]:\nHouve retirada de token\n\n",pt->final);
                printf("pt->inicio:%d = pl->lu:%d\npl->tk:%d - pt->tkgp:%d\n",pt->inicio,pl->lu,pl->tk,pt->tkgp);
                pl->tk-=pt->tkgp; /** Subtração do numero de tokens de acordo com o numero de tokens pedidos pelo arco lugar-transição. */
                if(rand()%100+1 < PAT) /** Condição de ativação da transição, 50% de chance de ativação. */
                {
                    if(DEBUG > 4)
                        printf("Pthread[%d]:\nTransicao Ativada com Sucesso\n\n",pt->final);
                    ativacaotransicao(kopf,&lntk); /** Chamada da função ativacaotransicao. */
                }
            }
            if(DEBUG > 4 && pt->inicio == pl->lu && pl->tk > pt->tkgp)
                printf("Pthread[%d]:\nNao houve retirada de token\n\n",pt->final);
            pl = pl->prox; /** Método utilizado para transitar pela lista lugartoken. */
        }
        pl=*cabeca; /** Voltar o ponteiro para o inicio da lista lugartoken. */
        pt=pt->prox; /** Método utilizado para transitar pela lista arco entram. */
    }
    return ;
}


void inserirlutk(lugartoken **cabeca,int lu,int tk) /** Função para inserir novos lugarestokens na lista. */
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


void inserirtransicao(transicao **cabeca,int i) /** Função para inserir novas transições na lista. */
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

void inserirentram(arco **cabeca,int inicio,int tkgp,int final) /** Função para inserir novos arcos na lista, tanto arco lugar-transição quanto transição-lugar. */
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

void desenha_estados(BITMAP *buff, int k) /** Função para desenhar os lugares da rede de petri. */
{
    int i;
    float raio,xi,yi,rc;
    raio= (Y/8) * (M_PI/(M_PI+k));
    rc = YCentro - raio*4;
    if(DEBUG)
        printf("Iniciando o desenho dos estados\n");
    for(i=0;i<k;i++) /** Laço utilizado para desenhar os estados de acordo com a quantidade de lugares. */
    {
        yi=YCentro+rc*cos((2*M_PI/k)*i);
        xi=XCentro+rc*sin((2*M_PI/k)*i);
        circle(buff, xi, yi, raio, CORAZUL);
        textprintf_ex(buff, font, (xi-18), (yi-5), CORVERDE, CORPRETO, "Est %d",i);
    }
    return;
}

void desenha_transicoes(BITMAP *buff, transicao *trans, int k , int c) /** Função para desenhar as transições da rede de petri. */
{
    int i, l=0, j=1;
    float xi,yi,rc,raio;
    transicao *pl=trans;
    raio = (Y/8)*(M_PI/(M_PI+k));
    rc = YCentro - raio*4;
    raio = (Y/12)*(M_PI/(M_PI+k));

    while(1) /** Laço utilizado para transitar pela lista de transiçoes. */
    {
        for(i=j;i<c*2;i++) /** Laço utilizado para desenhar as transições. */
        {
            yi=YCentro+rc*cos((2*M_PI/(k*2))*i);
            xi=XCentro+rc*sin((2*M_PI/(k*2))*i);
            line(buff, (xi), (yi)+raio, (xi), (yi)-raio, CORVERMELHO);
            if(M_PI/2<=(2*M_PI/(k*2))*i && (3*M_PI)/2>(2*M_PI/(k*2))*i) /** Condição de posicionamento dos textos de identificação de acordo com o angulo de posicionamento das transições. */
                textprintf_ex(buff, font, xi-10, yi-raio-12, CORVERDE, CORPRETO, "Tr%d",l++);
            else
            {
                textprintf_ex(buff, font, xi-10, yi+raio+5, CORVERDE, CORPRETO, "Tr%d",l++);
                break;
            }
            i++;
        }
        j = i+2;
        if(pl->prox!=NULL) /** Condição de continuidade caso o ponteiro prox não aponte para nulo. */
        {
            pl=pl->prox;
        }
        else
            break;
    }
    return;
}

void desenha_arcos(int qo, int qf, BITMAP *buff, int k, int c, int flag) /** Funções utilizadas para desenhar os arcos da rede de petri. */
{
    float si,co,alfa, beta, phi, x1, y1, x2, y2, x3, y3, xo, yo, xf, yf, raio, xt1, yt1, xt2, yt2, rc;
    raio=(Y/8)*(M_PI/(M_PI+(k/2)));
    rc = YCentro - raio*4;

    y1 = YCentro + rc*cos((2*M_PI/k)*qo);
    x1 = XCentro + rc*sin((2*M_PI/k)*qo);
    y3 = YCentro + rc*cos((2*M_PI/k)*qf);
    x3 = XCentro + rc*sin((2*M_PI/k)*qf);

    alfa=arctan(x1,y1,x3,y3); /** Chamada da função arctan com valor retornado armazenado na variavel alpha. */
    y2=(y3+y1)/2 + raio * cos(alfa);
    x2=(x3+x1)/2 - raio * sin(alfa);

    if(((alfa >= 0) && (alfa <= M_PI/2)) || ((alfa >= M_PI) && (alfa <= 3*M_PI/2))) /** Condição para determinar ponto de chegada dos arcos. */
    {
        beta=arctan(x3,y3,x2,y2); /** Chamada da função arctan com valor retornado armazenado na variavel beta. */
        phi=arctan(x1,y1,x2,y2); /** Chamada da função arctan com valor retornado armazenado na variavel phi. */
        if(flag) /** Condição estabelecidada para dividir o desenho de arocs lugar-transição do desenho de arcos transição-lugar. */
        {
            xo = x1 + raio * cos(phi);
            yo = y1 + raio * sin(phi);
            xf = x3;
            yf = y3;
        }
        else
        {
            xo = x1;
            yo = y1;
            xf = x3 + raio * cos(beta);
            yf = y3 + raio * sin(beta);
        }

    }
    else
    {
        alfa=arctan(x3,y3,x1,y1); /** Chamada da função arctan com valor retornado armazenado na variavel alfa. */
        y2=(y3+y1)/2 + raio * cos(alfa);
        x2=(x3+x1)/2 - raio * sin(alfa);
        beta=arctan(x1,y1,x2,y2);
        phi=arctan(x3,y3,x2,y2);
        if(flag) /** Condição estabelecidada para dividir o desenho de arocs lugar-transição do desenho de arcos transição-lugar. */
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
    si=alsin(x2,y2,xf,yf); /** Chamada da função alsin com valor retornado armazenado na variavel si. */
    co=alcos(x2,y2,xf,yf); /** Chamada da função alcos com valor retornado armazenado na variavel co. */
    xt1 = xf - (raio / 4) * (si + co);
    yt1 = yf + (raio / 4) * (co - si);
    xt2 = xf + (raio / 4) * (si - co);
    yt2 = yf - (raio / 4) * (si + co);
    triangle(buff, xf, yf, xt1, yt1, xt2, yt2, CORBRANCO);
    textprintf_ex(buff, font, x2, y2, CORVERDE, CORPRETO, "%d", c);

    return;
}

float alcos(float x1, float y1, float x2, float y2) /** Função que calcula um cosseno que será utilizado no desenho da seta. */
{
    if(x1==0 && x2 == 0 && y1 == 0 && y2 == 0)
        return ~0;
    return ((x2-x1)/sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)));
}

float alsin(float x1, float y1, float x2, float y2) /** Função que calcula um seno que será utilizado no desenho da seta. */
{
    if(x1==0 && x2 == 0 && y1 == 0 && y2 == 0)
        return ~0;
    return ((y2-y1)/sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)));
}


float arctan(float x1, float y1, float x2, float y2) /** Função que calcula o arcotangente para todos os quadrantes de forma individual. */
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
    if((x2 < x1) && (y2 > y1)) /** QUAD = 2 */
        return a + M_PI/2.0;
    if((x2 < x1) && (y2 < y1)) /** QUAD = 3 */
        return a + M_PI;
    if((x2 > x1) && (y2 < y1)) /** QUAD = 4 */
        return a + 3.0*M_PI/2.0;
    return a; /** QUAD = 1 */
}

void inserirpthread(thread **cabeca, pthread_t p) /** Função para inserir novas pthreads na lista. */
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

