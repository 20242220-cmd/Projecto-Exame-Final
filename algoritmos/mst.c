#include <stdio.h>
#include "mst.h"

/* ============================================================
   algoritmos/mst.c - implementacao de Kruskal e Prim
   (Arvore Geradora Minima)
   ============================================================ */

/* ------------------------------------------------------------
   Union-Find (Disjoint Set) - usado pelo Kruskal
   ------------------------------------------------------------ */
static int uf_pai[MAX_AEROPORTOS];

static void uf_iniciar(int n)
{
    int i;

    for (i = 0; i < n; i++) {
        uf_pai[i] = i;
    }
}

static int uf_find(int x)
{
    while (uf_pai[x] != x) {
        x = uf_pai[x];
    }

    return x;
}

static void uf_unir(int a, int b)
{
    int ra;
    int rb;

    ra = uf_find(a);
    rb = uf_find(b);

    if (ra != rb) {
        uf_pai[ra] = rb;
    }
}

/* ------------------------------------------------------------
   kruskal_mst
   Calcula a Arvore Geradora Minima usando o algoritmo de
   Kruskal: ordena todas as arestas por distancia e adiciona-as
   uma a uma se nao formarem ciclo (verificado com Union-Find).
   ------------------------------------------------------------ */
ResultadoMST kruskal_mst(Rede *r)
{
    ResultadoMST res;
    ArestaMST todas[MAX_AEROPORTOS * MAX_AEROPORTOS];
    int num_todas;
    int i;
    int j;
    Aresta *a;
    ArestaMST tmp;

    res.num_arestas = 0;
    res.distancia_total = 0;

    if (r == NULL || r->num_aeroportos == 0) {
        printf("[ERRO] Rede invalida.\n");
        return res;
    }

    /* recolher todas as arestas (cada uma uma vez, orig < dest) */
    num_todas = 0;
    for (i = 0; i < r->num_aeroportos; i++) {
        a = r->lista_adj[i];
        while (a != NULL) {
            if (a->destino > i) {
                todas[num_todas].origem = i;
                todas[num_todas].destino = a->destino;
                todas[num_todas].distancia_km = a->distancia_km;
                num_todas = num_todas + 1;
            }
            a = a->prox;
        }
    }

    /* ordenacao por bolha (bubble sort) por distancia crescente */
    for (i = 0; i < num_todas - 1; i++) {
        for (j = 0; j < num_todas - 1 - i; j++) {
            if (todas[j].distancia_km > todas[j + 1].distancia_km) {
                tmp = todas[j];
                todas[j] = todas[j + 1];
                todas[j + 1] = tmp;
            }
        }
    }

    /* aplicar Union-Find e seleccionar arestas sem ciclo */
    uf_iniciar(r->num_aeroportos);

    for (i = 0; i < num_todas; i++) {
        if (uf_find(todas[i].origem) != uf_find(todas[i].destino)) {
            uf_unir(todas[i].origem, todas[i].destino);
            res.arestas[res.num_arestas] = todas[i];
            res.num_arestas = res.num_arestas + 1;
            res.distancia_total = res.distancia_total + todas[i].distancia_km;
        }
    }

    return res;
}

/* ------------------------------------------------------------
   prim_mst
   Calcula a Arvore Geradora Minima usando o algoritmo de Prim,
   a partir do aeroporto 'raiz'.
   ------------------------------------------------------------ */
ResultadoMST prim_mst(Rede *r, int raiz)
{
    ResultadoMST res;
    int na_arvore[MAX_AEROPORTOS];
    int custo_min[MAX_AEROPORTOS];
    int origem_min[MAX_AEROPORTOS];
    int i;
    int u;
    int v;
    int melhor;
    int melhor_custo;
    Aresta *a;

    res.num_arestas = 0;
    res.distancia_total = 0;

    if (r == NULL || r->num_aeroportos == 0 ||
        raiz < 0 || raiz >= r->num_aeroportos) {
        printf("[ERRO] Rede ou aeroporto raiz invalido.\n");
        return res;
    }

    for (i = 0; i < r->num_aeroportos; i++) {
        na_arvore[i] = 0;
        custo_min[i] = INFINITO;
        origem_min[i] = -1;
    }

    custo_min[raiz] = 0;

    for (i = 0; i < r->num_aeroportos; i++) {
        /* escolher o vertice fora da arvore com menor custo_min */
        melhor = -1;
        melhor_custo = INFINITO;
        for (v = 0; v < r->num_aeroportos; v++) {
            if (na_arvore[v] == 0 && custo_min[v] < melhor_custo) {
                melhor_custo = custo_min[v];
                melhor = v;
            }
        }

        if (melhor == -1) {
            /* nao ha mais vertices acessiveis */
            break;
        }

        na_arvore[melhor] = 1;
        u = melhor;

        if (origem_min[u] != -1) {
            res.arestas[res.num_arestas].origem = origem_min[u];
            res.arestas[res.num_arestas].destino = u;
            res.arestas[res.num_arestas].distancia_km = custo_min[u];
            res.num_arestas = res.num_arestas + 1;
            res.distancia_total = res.distancia_total + custo_min[u];
        }

        /* actualizar custos dos vizinhos de u */
        a = r->lista_adj[u];
        while (a != NULL) {
            v = a->destino;
            if (na_arvore[v] == 0 && a->distancia_km < custo_min[v]) {
                custo_min[v] = a->distancia_km;
                origem_min[v] = u;
            }
            a = a->prox;
        }
    }

    return res;
}

/* ------------------------------------------------------------
   mst_imprimir
   Imprime as arestas da MST e a distancia total, indicando
   tambem o algoritmo usado.
   ------------------------------------------------------------ */
void mst_imprimir(Rede *r, ResultadoMST *m, const char *alg)
{
    int i;

    if (r == NULL || m == NULL) {
        printf("[ERRO] Dados invalidos.\n");
        return;
    }

    printf("----------------------------------------------------------\n");
    printf("[OK] Rede minima (algoritmo: %s)\n", alg == NULL ? "?" : alg);
    printf("----------------------------------------------------------\n");
    printf("%-4s -> %-4s   DISTANCIA\n", "ORIG", "DEST");

    for (i = 0; i < m->num_arestas; i++) {
        printf("%-4s -> %-4s   %6d km\n",
               r->aeroportos[m->arestas[i].origem].codigo,
               r->aeroportos[m->arestas[i].destino].codigo,
               m->arestas[i].distancia_km);
    }

    printf("----------------------------------------------------------\n");
    printf("Total de ligacoes : %d\n", m->num_arestas);
    printf("Distancia total   : %d km\n", m->distancia_total);
    printf("----------------------------------------------------------\n");
}
