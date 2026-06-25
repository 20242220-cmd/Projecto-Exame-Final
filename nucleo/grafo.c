#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

/* ============================================================
   nucleo/grafo.c - implementacao do TAD Rede
   ============================================================ */

Rede *criar_rede(void)
{
    Rede *r;
    int i;
    int j;

    r = (Rede *) malloc(sizeof(Rede));
    if (r == NULL) {
        printf("[ERRO] Falha ao alocar memoria para a rede.\n");
        return NULL;
    }

    r->num_aeroportos = 0;
    r->num_rotas = 0;

    for (i = 0; i < MAX_AEROPORTOS; i++) {
        r->lista_adj[i] = NULL;
        for (j = 0; j < MAX_AEROPORTOS; j++) {
            if (i == j) {
                r->matriz_dist[i][j] = 0;
            } else {
                r->matriz_dist[i][j] = INFINITO;
            }
        }
    }

    return r;
}

void destruir_rede(Rede *r)
{
    int i;
    Aresta *atual;
    Aresta *seguinte;

    if (r == NULL) {
        return;
    }

    for (i = 0; i < MAX_AEROPORTOS; i++) {
        atual = r->lista_adj[i];
        while (atual != NULL) {
            seguinte = atual->prox;
            free(atual);
            atual = seguinte;
        }
        r->lista_adj[i] = NULL;
    }

    free(r);
}

int adicionar_aeroporto(Rede *r, Aeroporto a)
{
    int idx;

    if (r == NULL) {
        printf("[ERRO] Rede invalida.\n");
        return -1;
    }

    if (r->num_aeroportos >= MAX_AEROPORTOS) {
        printf("[ERRO] Numero maximo de aeroportos atingido.\n");
        return -1;
    }

    idx = r->num_aeroportos;
    a.id = idx;
    r->aeroportos[idx] = a;
    r->num_aeroportos = r->num_aeroportos + 1;

    return idx;
}

void adicionar_rota(Rede *r, int orig, int dest, int km, int min)
{
    Aresta *nova;

    if (r == NULL) {
        printf("[ERRO] Rede invalida.\n");
        return;
    }

    if (orig < 0 || orig >= r->num_aeroportos ||
        dest < 0 || dest >= r->num_aeroportos) {
        printf("[ERRO] Indice de aeroporto invalido na rota.\n");
        return;
    }

    /* sentido orig -> dest */
    nova = (Aresta *) malloc(sizeof(Aresta));
    if (nova == NULL) {
        printf("[ERRO] Falha ao alocar memoria para aresta.\n");
        return;
    }
    nova->destino = dest;
    nova->distancia_km = km;
    nova->tempo_min = min;
    nova->peso_clima = 0;
    nova->prox = r->lista_adj[orig];
    r->lista_adj[orig] = nova;

    /* sentido dest -> orig (rota bidireccional) */
    nova = (Aresta *) malloc(sizeof(Aresta));
    if (nova == NULL) {
        printf("[ERRO] Falha ao alocar memoria para aresta.\n");
        return;
    }
    nova->destino = orig;
    nova->distancia_km = km;
    nova->tempo_min = min;
    nova->peso_clima = 0;
    nova->prox = r->lista_adj[dest];
    r->lista_adj[dest] = nova;

    r->matriz_dist[orig][dest] = km;
    r->matriz_dist[dest][orig] = km;

    r->num_rotas = r->num_rotas + 1;
}

void remover_rota(Rede *r, int orig, int dest)
{
    Aresta *atual;
    Aresta *anterior;

    if (r == NULL) {
        printf("[ERRO] Rede invalida.\n");
        return;
    }

    if (orig < 0 || orig >= r->num_aeroportos ||
        dest < 0 || dest >= r->num_aeroportos) {
        printf("[ERRO] Indice de aeroporto invalido.\n");
        return;
    }

    /* remover orig -> dest */
    atual = r->lista_adj[orig];
    anterior = NULL;
    while (atual != NULL) {
        if (atual->destino == dest) {
            if (anterior == NULL) {
                r->lista_adj[orig] = atual->prox;
            } else {
                anterior->prox = atual->prox;
            }
            free(atual);
            break;
        }
        anterior = atual;
        atual = atual->prox;
    }

    /* remover dest -> orig */
    atual = r->lista_adj[dest];
    anterior = NULL;
    while (atual != NULL) {
        if (atual->destino == orig) {
            if (anterior == NULL) {
                r->lista_adj[dest] = atual->prox;
            } else {
                anterior->prox = atual->prox;
            }
            free(atual);
            break;
        }
        anterior = atual;
        atual = atual->prox;
    }

    r->matriz_dist[orig][dest] = INFINITO;
    r->matriz_dist[dest][orig] = INFINITO;

    if (r->num_rotas > 0) {
        r->num_rotas = r->num_rotas - 1;
    }

    printf("[OK] Rota removida com sucesso.\n");
}

int indice_por_codigo(Rede *r, const char *codigo)
{
    int i;

    if (r == NULL || codigo == NULL) {
        return -1;
    }

    for (i = 0; i < r->num_aeroportos; i++) {
        if (strcmp(r->aeroportos[i].codigo, codigo) == 0) {
            return i;
        }
    }

    return -1;
}

Rede *carregar_ficheiros(const char *f_aero, const char *f_rotas)
{
    Rede *r;
    FILE *fp;
    char linha[256];
    char codigo[MAX_CODIGO];
    char nome[MAX_NOME];
    char pais[MAX_PAIS];
    char continente[MAX_CONTINENTE];
    int  num_pistas;
    int  pos_x;
    int  pos_y;
    Aeroporto a;
    char cod_orig[MAX_CODIGO];
    char cod_dest[MAX_CODIGO];
    int  km;
    int  min;
    int  i_orig;
    int  i_dest;

    r = criar_rede();
    if (r == NULL) {
        return NULL;
    }

    /* --- carregar aeroportos --- */
    fp = fopen(f_aero, "r");
    if (fp == NULL) {
        printf("[ERRO] Nao foi possivel abrir o ficheiro %s.\n", f_aero);
        destruir_rede(r);
        return NULL;
    }

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        if (linha[0] == '#' || linha[0] == '\n' || linha[0] == '\r') {
            continue;
        }

        if (sscanf(linha, "%3[^|]|%79[^|]|%59[^|]|%19[^|]|%d|%d|%d",
                   codigo, nome, pais, continente,
                   &num_pistas, &pos_x, &pos_y) == 7) {

            strcpy(a.codigo, codigo);
            strcpy(a.nome, nome);
            strcpy(a.pais, pais);
            strcpy(a.continente, continente);
            a.num_pistas = num_pistas;
            a.pistas_livres = num_pistas;
            a.estado = AERO_ABERTO;
            a.nivel_movimento = 0;
            a.pos_x = pos_x;
            a.pos_y = pos_y;

            adicionar_aeroporto(r, a);
        }
    }
    fclose(fp);

    printf("[OK] %d aeroportos carregados de %s.\n",
           r->num_aeroportos, f_aero);

    /* --- carregar rotas --- */
    fp = fopen(f_rotas, "r");
    if (fp == NULL) {
        printf("[ERRO] Nao foi possivel abrir o ficheiro %s.\n", f_rotas);
        return r;
    }

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        if (linha[0] == '#' || linha[0] == '\n' || linha[0] == '\r') {
            continue;
        }

        if (sscanf(linha, "%3[^|]|%3[^|]|%d|%d",
                   cod_orig, cod_dest, &km, &min) == 4) {

            i_orig = indice_por_codigo(r, cod_orig);
            i_dest = indice_por_codigo(r, cod_dest);

            if (i_orig == -1 || i_dest == -1) {
                printf("[ERRO] Rota ignorada: %s-%s (aeroporto inexistente).\n",
                       cod_orig, cod_dest);
                continue;
            }

            adicionar_rota(r, i_orig, i_dest, km, min);
        }
    }
    fclose(fp);

    printf("[OK] %d rotas carregadas de %s.\n", r->num_rotas, f_rotas);

    return r;
}

void guardar_rede(Rede *r, const char *ficheiro)
{
    FILE *fp;
    int i;
    Aresta *a;

    if (r == NULL) {
        printf("[ERRO] Rede invalida.\n");
        return;
    }

    fp = fopen(ficheiro, "w");
    if (fp == NULL) {
        printf("[ERRO] Nao foi possivel criar o ficheiro %s.\n", ficheiro);
        return;
    }

    fprintf(fp, "# rede exportada pelo RouteSim\n");
    for (i = 0; i < r->num_aeroportos; i++) {
        fprintf(fp, "%s|%s|%s|%s|%d|%d|%d\n",
                r->aeroportos[i].codigo,
                r->aeroportos[i].nome,
                r->aeroportos[i].pais,
                r->aeroportos[i].continente,
                r->aeroportos[i].num_pistas,
                r->aeroportos[i].pos_x,
                r->aeroportos[i].pos_y);
    }

    fprintf(fp, "# rotas\n");
    for (i = 0; i < r->num_aeroportos; i++) {
        a = r->lista_adj[i];
        while (a != NULL) {
            if (a->destino > i) {
                fprintf(fp, "%s|%s|%d|%d\n",
                        r->aeroportos[i].codigo,
                        r->aeroportos[a->destino].codigo,
                        a->distancia_km,
                        a->tempo_min);
            }
            a = a->prox;
        }
    }

    fclose(fp);
    printf("[OK] Rede guardada em %s.\n", ficheiro);
}

void imprimir_rede(Rede *r)
{
    int i;

    if (r == NULL) {
        printf("[ERRO] Rede invalida.\n");
        return;
    }

    printf("[OK] Rede com %d aeroportos e %d rotas.\n",
           r->num_aeroportos, r->num_rotas);
    printf("----------------------------------------------------------\n");
    printf("ID  COD  NOME                                  PAIS\n");
    printf("----------------------------------------------------------\n");

    for (i = 0; i < r->num_aeroportos; i++) {
        printf("%2d  %-3s  %-36s  %s\n",
               r->aeroportos[i].id,
               r->aeroportos[i].codigo,
               r->aeroportos[i].nome,
               r->aeroportos[i].pais);
    }
    printf("----------------------------------------------------------\n");
}

void imprimir_aeroporto(Rede *r, int idx)
{
    Aresta *a;

    if (r == NULL) {
        printf("[ERRO] Rede invalida.\n");
        return;
    }

    if (idx < 0 || idx >= r->num_aeroportos) {
        printf("[ERRO] Indice de aeroporto invalido.\n");
        return;
    }

    printf("----------------------------------------------------------\n");
    printf("Codigo      : %s\n", r->aeroportos[idx].codigo);
    printf("Nome        : %s\n", r->aeroportos[idx].nome);
    printf("Pais        : %s\n", r->aeroportos[idx].pais);
    printf("Continente  : %s\n", r->aeroportos[idx].continente);
    printf("Pistas      : %d (livres: %d)\n",
           r->aeroportos[idx].num_pistas,
           r->aeroportos[idx].pistas_livres);
    printf("Movimento   : %d\n", r->aeroportos[idx].nivel_movimento);

    switch (r->aeroportos[idx].estado) {
        case AERO_ABERTO:
            printf("Estado      : ABERTO\n");
            break;
        case AERO_FECHADO:
            printf("Estado      : FECHADO\n");
            break;
        case AERO_REDUZIDO:
            printf("Estado      : REDUZIDO\n");
            break;
        case AERO_LOTADO:
            printf("Estado      : LOTADO\n");
            break;
        default:
            printf("Estado      : DESCONHECIDO\n");
            break;
    }

    printf("Rotas directas:\n");
    a = r->lista_adj[idx];
    if (a == NULL) {
        printf("  (nenhuma)\n");
    }
    while (a != NULL) {
        printf("  -> %s  (%d km, %d min)\n",
               r->aeroportos[a->destino].codigo,
               a->distancia_km,
               a->tempo_min);
        a = a->prox;
    }
    printf("----------------------------------------------------------\n");
}
