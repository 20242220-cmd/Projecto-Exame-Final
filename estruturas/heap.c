#include <stdio.h>
#include <string.h>
#include "heap.h"

/* ============================================================
   estruturas/heap.c - implementacao do Min-Heap (ElementoHeap)
   Heap baseado no campo 'prioridade' (menor = mais urgente)
   ============================================================ */

void heap_iniciar(Heap *h)
{
    if (h == NULL) {
        return;
    }

    h->tamanho = 0;
}

int heap_vazia(Heap *h)
{
    if (h == NULL) {
        return 1;
    }

    return (h->tamanho == 0);
}

int heap_cheia(Heap *h)
{
    if (h == NULL) {
        return 1;
    }

    return (h->tamanho >= HEAP_MAX);
}

static void heap_troca(ElementoHeap *a, ElementoHeap *b)
{
    ElementoHeap tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

static void heap_subir(Heap *h, int i)
{
    int pai;

    while (i > 0) {
        pai = (i - 1) / 2;
        if (h->dados[i].prioridade < h->dados[pai].prioridade) {
            heap_troca(&h->dados[i], &h->dados[pai]);
            i = pai;
        } else {
            break;
        }
    }
}

static void heap_descer(Heap *h, int i)
{
    int esq;
    int dir;
    int menor;

    for (;;) {
        esq = 2 * i + 1;
        dir = 2 * i + 2;
        menor = i;

        if (esq < h->tamanho &&
            h->dados[esq].prioridade < h->dados[menor].prioridade) {
            menor = esq;
        }
        if (dir < h->tamanho &&
            h->dados[dir].prioridade < h->dados[menor].prioridade) {
            menor = dir;
        }

        if (menor == i) {
            break;
        }

        heap_troca(&h->dados[i], &h->dados[menor]);
        i = menor;
    }
}

int heap_inserir(Heap *h, ElementoHeap e)
{
    if (h == NULL) {
        printf("[ERRO] Heap invalida.\n");
        return -1;
    }

    if (heap_cheia(h)) {
        printf("[ERRO] Heap cheia, nao foi possivel inserir.\n");
        return -1;
    }

    h->dados[h->tamanho] = e;
    h->tamanho = h->tamanho + 1;
    heap_subir(h, h->tamanho - 1);

    return 0;
}

ElementoHeap heap_extrair_min(Heap *h)
{
    ElementoHeap topo;
    ElementoHeap vazio;

    memset(&vazio, 0, sizeof(ElementoHeap));
    vazio.prioridade = INFINITO;
    vazio.id = -1;

    if (h == NULL || heap_vazia(h)) {
        printf("[ERRO] Heap vazia, nao foi possivel extrair.\n");
        return vazio;
    }

    topo = h->dados[0];

    h->tamanho = h->tamanho - 1;
    h->dados[0] = h->dados[h->tamanho];
    heap_descer(h, 0);

    return topo;
}

ElementoHeap *heap_topo(Heap *h)
{
    if (h == NULL || heap_vazia(h)) {
        return NULL;
    }

    return &h->dados[0];
}

void heap_imprimir(Heap *h)
{
    int i;

    if (h == NULL) {
        printf("[ERRO] Heap invalida.\n");
        return;
    }

    if (heap_vazia(h)) {
        printf("[INFO] Heap vazia.\n");
        return;
    }

    printf("----------------------------------------------------------\n");
    printf("POS  PRIORIDADE  ID    EXTRA  DESCRICAO\n");
    printf("----------------------------------------------------------\n");
    for (i = 0; i < h->tamanho; i++) {
        printf("%3d  %10d  %4d  %5d  %s\n",
               i,
               h->dados[i].prioridade,
               h->dados[i].id,
               h->dados[i].dado_extra,
               h->dados[i].descricao);
    }
    printf("----------------------------------------------------------\n");
}

ElementoHeap heap_elem_voo(int id, int prio, int tick, const char *desc)
{
    ElementoHeap e;

    e.prioridade = prio;
    e.id = id;
    e.dado_extra = tick;

    if (desc != NULL) {
        strncpy(e.descricao, desc, sizeof(e.descricao) - 1);
        e.descricao[sizeof(e.descricao) - 1] = '\0';
    } else {
        e.descricao[0] = '\0';
    }

    return e;
}

ElementoHeap heap_elem_dijkstra(int id_no, int custo)
{
    ElementoHeap e;

    e.prioridade = custo;
    e.id = id_no;
    e.dado_extra = custo;
    strcpy(e.descricao, "dijkstra");

    return e;
}
