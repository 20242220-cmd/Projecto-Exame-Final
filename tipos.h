#ifndef TIPOS_H
#define TIPOS_H

/* ============================================================
   RouteSim - tipos.h
   BASE DE TUDO. Nao inclui nenhum outro ficheiro do projecto.
   Todos os tipos, structs e constantes partilhados vivem aqui.
   Este ficheiro nao e modificado depois de criado.
   ============================================================ */

#include <stdio.h>

/* ------------------------------------------------------------
   Constantes globais
   ------------------------------------------------------------ */
#define MAX_AEROPORTOS    20
#define MAX_NOME          80
#define MAX_CODIGO         4    /* codigo IATA: 3 letras + terminador */
#define MAX_PAIS          60
#define MAX_CONTINENTE    20
#define MAX_VOOS         100
#define MAX_EVENTOS      200
#define MAX_PISTAS         6
#define MAX_ROTA          30    /* maximo de escalas numa rota */
#define INFINITO       99999
#define TICK_MS          200    /* ms entre ticks de simulacao */

/* Estruturas de dados */
#define HASH_TAM         127
#define HASH_COD_VOO      10
#define HEAP_MAX         200
#define TRIE_ALFA         27    /* a-z + traco */
#define TRIE_MAX_SUG      10
#define TRIE_MAX_COMP     80
#define AVL_MAX_RES      500
#define AB_ORDEM           5

/* ------------------------------------------------------------
   Enumeracoes
   ------------------------------------------------------------ */
typedef enum {
    VOO_NORMAL,
    VOO_VIP,
    VOO_EMERGENCIA,
    VOO_CARGA
} TipoVoo;

typedef enum {
    AGENDADO,
    A_DECOLAR,
    EM_VOO,
    EM_ESPERA,       /* aguarda pista livre */
    A_ATERRAR,
    ATERRADO,
    CANCELADO,
    DESVIADO         /* rota recalculada por evento */
} EstadoVoo;

typedef enum {
    EVT_TROVOADA,
    EVT_NEVOEIRO,
    EVT_VENTO,
    EVT_FALHA,
    EVT_VIP,
    EVT_EMERGENCIA,
    EVT_LOTACAO,
    EVT_VIA_LIVRE
} TipoEvento;

typedef enum {
    AERO_ABERTO,
    AERO_FECHADO,    /* trovoada ou outro evento grave */
    AERO_REDUZIDO,   /* nevoeiro - so 1 pista activa */
    AERO_LOTADO      /* sem pistas livres temporariamente */
} EstadoAeroporto;

/* ------------------------------------------------------------
   Grafo - Aeroporto e Aresta
   ------------------------------------------------------------ */
typedef struct {
    int  id;
    char codigo[MAX_CODIGO];          /* ex: LUA */
    char nome[MAX_NOME];              /* nome completo */
    char pais[MAX_PAIS];              /* ex: Angola */
    char continente[MAX_CONTINENTE];  /* ex: Africa */
    int  num_pistas;
    int  pistas_livres;
    EstadoAeroporto estado;
    int  nivel_movimento;             /* 0-100 */
    int  pos_x;
    int  pos_y;
} Aeroporto;

typedef struct Aresta {
    int            destino;
    int            distancia_km;
    int            tempo_min;
    int            peso_clima;        /* penalizacao por eventos */
    struct Aresta *prox;
} Aresta;

typedef struct {
    Aeroporto aeroportos[MAX_AEROPORTOS];
    Aresta   *lista_adj[MAX_AEROPORTOS];
    int       matriz_dist[MAX_AEROPORTOS][MAX_AEROPORTOS];
    int       num_aeroportos;
    int       num_rotas;
} Rede;

/* ------------------------------------------------------------
   Voo em simulacao
   ------------------------------------------------------------ */
typedef struct {
    char      codigo[HASH_COD_VOO];   /* ex: TAAG02 */
    TipoVoo   tipo;
    EstadoVoo estado;
    int       origem;
    int       destino;
    int       rota[MAX_ROTA];
    int       tam_rota;
    int       pos_rota;
    int       tick_partida;
    int       tick_chegada_previsto;
    int       num_desvios;
    int       activo;
} Voo;

/* ------------------------------------------------------------
   Evento de simulacao
   ------------------------------------------------------------ */
typedef struct {
    TipoEvento tipo;
    int        id_aeroporto;
    int        duracao_ticks;
    int        tick_inicio;
    char       descricao[80];
} EventoSim;

/* ------------------------------------------------------------
   Registo historico de voo concluido (usado pela AVL)
   ------------------------------------------------------------ */
typedef struct {
    int  id;
    char codigo[HASH_COD_VOO];
    int  origem;
    int  destino;
    int  tick_partida;
    int  tick_chegada;
    int  distancia_total;
    int  num_desvios;
    char descricao[80];
} RegistoVoo;

/* ------------------------------------------------------------
   Resultados de algoritmos
   ------------------------------------------------------------ */
typedef struct {
    int rota[MAX_ROTA];
    int tam_rota;
    int distancia_total;
    int tempo_total;
    int acessivel;
} ResultadoRota;

typedef struct {
    int origem;
    int destino;
    int distancia_km;
} ArestaMST;

typedef struct {
    ArestaMST arestas[MAX_AEROPORTOS];
    int       num_arestas;
    int       distancia_total;
} ResultadoMST;

/* ------------------------------------------------------------
   Heap - elemento de prioridade
   ------------------------------------------------------------ */
typedef struct {
    int  prioridade;     /* menor = mais urgente (min-heap) */
    int  id;             /* id do voo ou indice de no */
    int  dado_extra;     /* tick, custo, etc. */
    char descricao[60];
} ElementoHeap;

/* ------------------------------------------------------------
   Hash - entrada (encadeamento externo)
   ------------------------------------------------------------ */
typedef struct EntradaHash {
    char                codigo[HASH_COD_VOO];
    int                 id_voo;
    int                 activo;
    struct EntradaHash *prox;
} EntradaHash;

#endif /* TIPOS_H */
