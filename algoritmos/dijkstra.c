#include <stdio.h>
#include "dijkstra.h"

/* ============================================================
   algoritmos/dijkstra.c - implementacao do algoritmo de
   Dijkstra (caminho mais curto), usando a Heap (min-heap)
   de estruturas/heap.h como fila de prioridade.
   ============================================================ */

/* ------------------------------------------------------------
   dijkstra_base
   Funcao auxiliar interna: calcula distancias minimas a
   partir de 'orig', ignorando aeroportos marcados em
   'excluir' (vector de tamanho MAX_AEROPORTOS, 1 = ignorar).
   Preenche 'dist' e 'anterior' (ambos tamanho MAX_AEROPORTOS).
   Se 'excluir' for NULL, nenhum aeroporto e ignorado.
   ------------------------------------------------------------ */
static void dijkstra_base(Rede *r, int orig, int excluir[],
                           int dist[], int anterior[])
{
    int visitado[MAX_AEROPORTOS];
    Heap h;
    ElementoHeap e;
    Aresta *a;
    int i;
    int u;
    int novo_custo;

    for (i = 0; i < MAX_AEROPORTOS; i++) {
        dist[i] = INFINITO;
        anterior[i] = -1;
        visitado[i] = 0;
    }

    if (r == NULL || orig < 0 || orig >= r->num_aeroportos) {
        return;
    }

    if (excluir != NULL && excluir[orig] == 1) {
        return;
    }

    dist[orig] = 0;

    heap_iniciar(&h);
    heap_inserir(&h, heap_elem_dijkstra(orig, 0));

    while (!heap_vazia(&h)) {
        e = heap_extrair_min(&h);
        u = e.id;

        if (u < 0 || u >= r->num_aeroportos) {
            continue;
        }

        if (visitado[u] == 1) {
            continue;
        }
        visitado[u] = 1;

        a = r->lista_adj[u];
        while (a != NULL) {
            if (excluir == NULL || excluir[a->destino] == 0) {
                novo_custo = dist[u] + a->distancia_km + a->peso_clima;
                if (novo_custo < dist[a->destino]) {
                    dist[a->destino] = novo_custo;
                    anterior[a->destino] = u;
                    heap_inserir(&h, heap_elem_dijkstra(a->destino, novo_custo));
                }
            }
            a = a->prox;
        }
    }
}

/* ------------------------------------------------------------
   dijkstra_reconstruir
   Constroi o ResultadoRota a partir dos vectores dist[] e
   anterior[] calculados por dijkstra_base, para o destino
   'dest'.
   ------------------------------------------------------------ */
static ResultadoRota dijkstra_reconstruir(Rede *r, int orig, int dest,
                                           int dist[], int anterior[])
{
    ResultadoRota res;
    int tmp[MAX_ROTA];
    int n;
    int atual;
    int i;
    int tempo;
    Aresta *a;

    res.tam_rota = 0;
    res.distancia_total = 0;
    res.tempo_total = 0;
    res.acessivel = 0;

    for (i = 0; i < MAX_ROTA; i++) {
        res.rota[i] = -1;
    }

    if (r == NULL ||
        orig < 0 || orig >= r->num_aeroportos ||
        dest < 0 || dest >= r->num_aeroportos) {
        return res;
    }

    if (dist[dest] >= INFINITO) {
        return res;
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
        res.rota[i] = tmp[n - 1 - i];
    }
    res.tam_rota = n;
    res.distancia_total = dist[dest];
    res.acessivel = 1;

    /* somar o tempo de viagem ao longo do caminho */
    tempo = 0;
    for (i = 0; i < n - 1; i++) {
        a = r->lista_adj[res.rota[i]];
        while (a != NULL) {
            if (a->destino == res.rota[i + 1]) {
                tempo = tempo + a->tempo_min;
                break;
            }
            a = a->prox;
        }
    }
    res.tempo_total = tempo;

    return res;
}

/* ------------------------------------------------------------
   dijkstra_rota
   Calcula a rota de custo minimo entre orig e dest.
   ------------------------------------------------------------ */
ResultadoRota dijkstra_rota(Rede *r, int orig, int dest)
{
    int dist[MAX_AEROPORTOS];
    int anterior[MAX_AEROPORTOS];

    dijkstra_base(r, orig, NULL, dist, anterior);

    return dijkstra_reconstruir(r, orig, dest, dist, anterior);
}

/* ------------------------------------------------------------
   dijkstra_evitando
   Calcula a rota de custo minimo entre orig e dest, evitando
   os aeroportos marcados em 'excluir' (vector de tamanho
   MAX_AEROPORTOS, 1 = evitar, 0 = permitido).
   ------------------------------------------------------------ */
ResultadoRota dijkstra_evitando(Rede *r, int orig, int dest, int excluir[])
{
    int dist[MAX_AEROPORTOS];
    int anterior[MAX_AEROPORTOS];

    dijkstra_base(r, orig, excluir, dist, anterior);

    return dijkstra_reconstruir(r, orig, dest, dist, anterior);
}

/* ------------------------------------------------------------
   dijkstra_distancias
   Preenche 'dist' (tamanho MAX_AEROPORTOS) com a distancia
   minima de 'orig' a cada aeroporto da rede.
   ------------------------------------------------------------ */
void dijkstra_distancias(Rede *r, int orig, int dist[])
{
    int anterior[MAX_AEROPORTOS];

    dijkstra_base(r, orig, NULL, dist, anterior);
}

/* ------------------------------------------------------------
   dijkstra_imprimir
   Imprime o resultado de uma rota calculada por Dijkstra,
   mostrando os codigos e nomes dos aeroportos no caminho.
   ------------------------------------------------------------ */
void dijkstra_imprimir(Rede *r, ResultadoRota *rota)
{
    int i;

    if (r == NULL || rota == NULL) {
        printf("[ERRO] Dados invalidos.\n");
        return;
    }

    if (rota->acessivel == 0) {
        printf("[ERRO] Nao existe rota possivel entre os aeroportos indicados.\n");
        return;
    }

    printf("----------------------------------------------------------\n");
    printf("[OK] Rota encontrada com %d escala(s).\n", rota->tam_rota - 1);
    printf("----------------------------------------------------------\n");

    for (i = 0; i < rota->tam_rota; i++) {
        printf("  %d. %-3s  %s\n",
               i + 1,
               r->aeroportos[rota->rota[i]].codigo,
               r->aeroportos[rota->rota[i]].nome);
    }

    printf("----------------------------------------------------------\n");
    printf("Distancia total : %d km\n", rota->distancia_total);
    printf("Tempo total     : %d min\n", rota->tempo_total);
    printf("----------------------------------------------------------\n");
}
