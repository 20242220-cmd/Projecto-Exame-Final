#ifndef MST_H
#define MST_H

#include "../tipos.h"
#include "../nucleo/grafo.h"

/* ------------------------------------------------------------
   algoritmos/mst.h - Arvore Geradora Minima (Kruskal e Prim)
   Inclui: ../tipos.h + ../nucleo/grafo.h
   ------------------------------------------------------------ */

ResultadoMST kruskal_mst(Rede *r);
ResultadoMST prim_mst(Rede *r, int raiz);
void         mst_imprimir(Rede *r, ResultadoMST *m, const char *alg);

#endif /* MST_H */
