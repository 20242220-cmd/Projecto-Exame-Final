#ifndef GRAFO_H
#define GRAFO_H

#include "../tipos.h"

/* ------------------------------------------------------------
   nucleo/grafo.h - TAD Rede (grafo de aeroportos e rotas)
   Inclui apenas: tipos.h
   ------------------------------------------------------------ */

Rede *criar_rede(void);
void  destruir_rede(Rede *r);

int   adicionar_aeroporto(Rede *r, Aeroporto a);
void  adicionar_rota(Rede *r, int orig, int dest, int km, int min);
void  remover_rota(Rede *r, int orig, int dest);

int   indice_por_codigo(Rede *r, const char *codigo);

Rede *carregar_ficheiros(const char *f_aero, const char *f_rotas);
void  guardar_rede(Rede *r, const char *ficheiro);

void  imprimir_rede(Rede *r);
void  imprimir_aeroporto(Rede *r, int idx);

#endif /* GRAFO_H */
