#include <stdio.h>
#include "bfs_dfs.h"

/* ============================================================
   algoritmos/bfs_dfs.c - implementacao BFS e DFS
   ============================================================ */

/* ------------------------------------------------------------
   bfs_mais_proximo
   Devolve o indice do aeroporto mais proximo de 'origem'
   (em numero de saltos) que NAO esteja na lista 'excluir'.
   'excluir' deve ser um vector de tamanho MAX_AEROPORTOS com
   valores 1 (excluido) ou 0 (incluido).
   Devolve -1 se nao houver nenhum candidato acessivel.
   ------------------------------------------------------------ */
int bfs_mais_proximo(Rede *r, int origem, int excluir[])
{
    int visitados[MAX_AEROPORTOS];
    int fila[MAX_AEROPORTOS];
    int inicio;
    int fim;
    int atual;
    int i;
    Aresta *a;

    if (r == NULL || origem < 0 || origem >= r->num_aeroportos) {
        return -1;
    }

    for (i = 0; i < MAX_AEROPORTOS; i++) {
        visitados[i] = 0;
    }

    inicio = 0;
    fim = 0;
    fila[fim] = origem;
    fim = fim + 1;
    visitados[origem] = 1;

    while (inicio < fim) {
        atual = fila[inicio];
        inicio = inicio + 1;

        if (atual != origem &&
            (excluir == NULL || excluir[atual] == 0)) {
            return atual;
        }

        a = r->lista_adj[atual];
        while (a != NULL) {
            if (visitados[a->destino] == 0) {
                visitados[a->destino] = 1;
                fila[fim] = a->destino;
                fim = fim + 1;
            }
            a = a->prox;
        }
    }

    return -1;
}

/* ------------------------------------------------------------
   bfs_distancia
   Devolve o numero minimo de saltos (arestas) entre origem
   e destino, ou -1 se nao for possivel chegar.
   ------------------------------------------------------------ */
int bfs_distancia(Rede *r, int origem, int destino)
{
    int visitados[MAX_AEROPORTOS];
    int dist[MAX_AEROPORTOS];
    int fila[MAX_AEROPORTOS];
    int inicio;
    int fim;
    int atual;
    int i;
    Aresta *a;

    if (r == NULL ||
        origem < 0 || origem >= r->num_aeroportos ||
        destino < 0 || destino >= r->num_aeroportos) {
        return -1;
    }

    for (i = 0; i < MAX_AEROPORTOS; i++) {
        visitados[i] = 0;
        dist[i] = -1;
    }

    inicio = 0;
    fim = 0;
    fila[fim] = origem;
    fim = fim + 1;
    visitados[origem] = 1;
    dist[origem] = 0;

    while (inicio < fim) {
        atual = fila[inicio];
        inicio = inicio + 1;

        if (atual == destino) {
            return dist[atual];
        }

        a = r->lista_adj[atual];
        while (a != NULL) {
            if (visitados[a->destino] == 0) {
                visitados[a->destino] = 1;
                dist[a->destino] = dist[atual] + 1;
                fila[fim] = a->destino;
                fim = fim + 1;
            }
            a = a->prox;
        }
    }

    return -1;
}

/* ------------------------------------------------------------
   bfs_caminho
   Calcula o caminho (em numero de saltos, sem considerar
   distancias) entre orig e dest e guarda os indices dos
   aeroportos visitados em 'cam' (tamanho maximo MAX_ROTA).
   Devolve o numero de aeroportos no caminho, ou 0 se nao
   houver caminho possivel.
   ------------------------------------------------------------ */
int bfs_caminho(Rede *r, int orig, int dest, int cam[])
{
    int visitados[MAX_AEROPORTOS];
    int anterior[MAX_AEROPORTOS];
    int fila[MAX_AEROPORTOS];
    int inicio;
    int fim;
    int atual;
    int i;
    int n;
    int tmp[MAX_ROTA];
    Aresta *a;

    if (r == NULL ||
        orig < 0 || orig >= r->num_aeroportos ||
        dest < 0 || dest >= r->num_aeroportos ||
        cam == NULL) {
        return 0;
    }

    for (i = 0; i < MAX_AEROPORTOS; i++) {
        visitados[i] = 0;
        anterior[i] = -1;
    }

    inicio = 0;
    fim = 0;
    fila[fim] = orig;
    fim = fim + 1;
    visitados[orig] = 1;

    while (inicio < fim) {
        atual = fila[inicio];
        inicio = inicio + 1;

        if (atual == dest) {
            break;
        }

        a = r->lista_adj[atual];
        while (a != NULL) {
            if (visitados[a->destino] == 0) {
                visitados[a->destino] = 1;
                anterior[a->destino] = atual;
                fila[fim] = a->destino;
                fim = fim + 1;
            }
            a = a->prox;
        }
    }

    if (visitados[dest] == 0) {
        return 0;
    }

    /* reconstruir caminho de tras para a frente */
    n = 0;
    atual = dest;
    while (atual != -1 && n < MAX_ROTA) {
        tmp[n] = atual;
        n = n + 1;
        atual = anterior[atual];
    }

    /* inverter para ordem orig -> dest */
    for (i = 0; i < n; i++) {
        cam[i] = tmp[n - 1 - i];
    }

    return n;
}

/* ------------------------------------------------------------
   dfs_explorar
   Marca recursivamente todos os aeroportos acessiveis a
   partir de 'origem' no vector 'visitados' (1 = visitado).
   'visitados' deve ter tamanho MAX_AEROPORTOS e estar
   inicializado a 0 antes da primeira chamada.
   ------------------------------------------------------------ */
void dfs_explorar(Rede *r, int origem, int visitados[])
{
    Aresta *a;

    if (r == NULL || visitados == NULL) {
        return;
    }

    if (origem < 0 || origem >= r->num_aeroportos) {
        return;
    }

    if (visitados[origem] == 1) {
        return;
    }

    visitados[origem] = 1;

    a = r->lista_adj[origem];
    while (a != NULL) {
        if (visitados[a->destino] == 0) {
            dfs_explorar(r, a->destino, visitados);
        }
        a = a->prox;
    }
}

/* ------------------------------------------------------------
   dfs_conexo
   Devolve 1 se a rede e totalmente conexa (todos os
   aeroportos acessiveis a partir do aeroporto 0), ou 0
   caso contrario.
   ------------------------------------------------------------ */
int dfs_conexo(Rede *r)
{
    int visitados[MAX_AEROPORTOS];
    int i;

    if (r == NULL || r->num_aeroportos == 0) {
        return 0;
    }

    for (i = 0; i < MAX_AEROPORTOS; i++) {
        visitados[i] = 0;
    }

    dfs_explorar(r, 0, visitados);

    for (i = 0; i < r->num_aeroportos; i++) {
        if (visitados[i] == 0) {
            return 0;
        }
    }

    return 1;
}

/* ------------------------------------------------------------
   dfs_isolados
   Preenche 'isolados' com os indices dos aeroportos que NAO
   sao acessiveis a partir do aeroporto 0. Devolve o numero
   de aeroportos isolados encontrados.
   ------------------------------------------------------------ */
int dfs_isolados(Rede *r, int isolados[])
{
    int visitados[MAX_AEROPORTOS];
    int i;
    int n;

    if (r == NULL || isolados == NULL || r->num_aeroportos == 0) {
        return 0;
    }

    for (i = 0; i < MAX_AEROPORTOS; i++) {
        visitados[i] = 0;
    }

    dfs_explorar(r, 0, visitados);

    n = 0;
    for (i = 0; i < r->num_aeroportos; i++) {
        if (visitados[i] == 0) {
            isolados[n] = i;
            n = n + 1;
        }
    }

    return n;
}
