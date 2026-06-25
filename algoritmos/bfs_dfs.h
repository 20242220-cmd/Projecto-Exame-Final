#ifndef BFS_DFS_H
#define BFS_DFS_H

#include "../tipos.h"
#include "../nucleo/grafo.h"

/* ------------------------------------------------------------
   algoritmos/bfs_dfs.h - Percursos em largura e profundidade
   Inclui: ../tipos.h + ../nucleo/grafo.h
   ------------------------------------------------------------ */

int  bfs_mais_proximo(Rede *r, int origem, int excluir[]);
int  bfs_distancia(Rede *r, int origem, int destino);
int  bfs_caminho(Rede *r, int orig, int dest, int cam[]);

void dfs_explorar(Rede *r, int origem, int visitados[]);
int  dfs_conexo(Rede *r);
int  dfs_isolados(Rede *r, int isolados[]);

#endif /* BFS_DFS_H */
