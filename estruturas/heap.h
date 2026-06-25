#ifndef HEAP_H
#define HEAP_H

#include "../tipos.h"

/* ------------------------------------------------------------
   estruturas/heap.h - Min-Heap generico (ElementoHeap)
   Inclui apenas: ../tipos.h
   ------------------------------------------------------------ */

typedef struct {
    ElementoHeap dados[HEAP_MAX];
    int          tamanho;
} Heap;

void          heap_iniciar(Heap *h);
int           heap_inserir(Heap *h, ElementoHeap e);
ElementoHeap  heap_extrair_min(Heap *h);
ElementoHeap *heap_topo(Heap *h);
int           heap_vazia(Heap *h);
int           heap_cheia(Heap *h);
void          heap_imprimir(Heap *h);

ElementoHeap  heap_elem_voo(int id, int prio, int tick, const char *desc);
ElementoHeap  heap_elem_dijkstra(int id_no, int custo);

#endif /* HEAP_H */
