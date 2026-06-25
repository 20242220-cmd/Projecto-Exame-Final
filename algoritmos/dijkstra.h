#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "../tipos.h"
#include "../nucleo/grafo.h"
#include "../estruturas/heap.h"

/* ------------------------------------------------------------
   algoritmos/dijkstra.h - Caminho mais curto (Dijkstra)
   Inclui: ../tipos.h + ../nucleo/grafo.h + ../estruturas/heap.h
   ------------------------------------------------------------ */

ResultadoRota dijkstra_rota(Rede *r, int orig, int dest);
ResultadoRota dijkstra_evitando(Rede *r, int orig, int dest, int excluir[]);
void          dijkstra_distancias(Rede *r, int orig, int dist[]);
void          dijkstra_imprimir(Rede *r, ResultadoRota *rota);

#endif /* DIJKSTRA_H */
