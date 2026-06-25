#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tipos.h"
#include "nucleo/grafo.h"
#include "algoritmos/bfs_dfs.h"
#include "algoritmos/dijkstra.h"
#include "algoritmos/mst.h"

/* ============================================================
   RouteSim - main.c
   SEMANA 2: menu Rotas funcional (Dijkstra, MST, BFS, DFS)
   ============================================================ */

void limpar_ecra(void)
{
    system("cls");
}

void aguardar_tecla(void)
{
    printf("\nPrima ENTER para continuar...");
    getchar();
}

void cabecalho(const char *titulo)
{
    printf("============================================================\n");
    printf("  %s\n", titulo);
    printf("============================================================\n\n");
}

int ler_opcao(void)
{
    char linha[16];
    int opcao;

    if (fgets(linha, sizeof(linha), stdin) == NULL) {
        return -1;
    }

    if (sscanf(linha, "%d", &opcao) != 1) {
        return -1;
    }

    return opcao;
}

int ler_codigo_iata(Rede *rede, const char *prompt, char cod[MAX_CODIGO])
{
    printf("%s", prompt);
    if (scanf("%3s", cod) != 1) {
        while (getchar() != '\n') {
            /* limpar buffer */
        }
        printf("[ERRO] Leitura invalida.\n");
        return -1;
    }
    while (getchar() != '\n') {
        /* limpar buffer */
    }

    return indice_por_codigo(rede, cod);
}

void menu_aeroportos(Rede *rede)
{
    int opcao;
    int idx;
    char cod[MAX_CODIGO];
    int continuar;
    int i;

    continuar = 1;
    while (continuar == 1) {
        limpar_ecra();
        cabecalho("MENU AEROPORTOS");
        printf("1. Listar todos os aeroportos\n");
        printf("2. Ver detalhes de um aeroporto\n");
        printf("3. Ver estado das pistas\n");
        printf("4. Ver aeroportos por continente\n");
        printf("5. Pesquisar aeroporto (autocompletar)\n");
        printf("0. Voltar\n\n");
        printf("Opcao: ");

        opcao = ler_opcao();

        switch (opcao) {
            case 1:
                limpar_ecra();
                cabecalho("LISTA DE AEROPORTOS");
                imprimir_rede(rede);
                aguardar_tecla();
                break;

            case 2:
                limpar_ecra();
                cabecalho("DETALHES DO AEROPORTO");
                idx = ler_codigo_iata(rede, "Codigo IATA (ex: LUA): ", cod);
                if (idx == -1) {
                    printf("[ERRO] Aeroporto nao encontrado.\n");
                } else {
                    imprimir_aeroporto(rede, idx);
                    printf("[OK] Detalhes mostrados com sucesso.\n");
                }
                aguardar_tecla();
                break;

            case 3:
                limpar_ecra();
                cabecalho("ESTADO DAS PISTAS");
                printf("%-4s %-36s %6s %6s\n",
                       "COD", "NOME", "TOTAL", "LIVRES");
                for (i = 0; i < rede->num_aeroportos; i++) {
                    printf("%-4s %-36s %6d %6d\n",
                           rede->aeroportos[i].codigo,
                           rede->aeroportos[i].nome,
                           rede->aeroportos[i].num_pistas,
                           rede->aeroportos[i].pistas_livres);
                }
                printf("\n[OK] Estado das pistas mostrado.\n");
                aguardar_tecla();
                break;

            case 4:
                limpar_ecra();
                cabecalho("AEROPORTOS POR CONTINENTE");
                printf("Continente (ex: Africa, Europa, Asia, Americas, Oceania): ");
                if (scanf("%19s", cod) != 1) {
                    printf("[ERRO] Leitura invalida.\n");
                    aguardar_tecla();
                    break;
                }
                while (getchar() != '\n') {
                    /* limpar buffer */
                }
                printf("\n");
                for (i = 0; i < rede->num_aeroportos; i++) {
                    if (strcmp(rede->aeroportos[i].continente, cod) == 0) {
                        printf("%-4s %s (%s)\n",
                               rede->aeroportos[i].codigo,
                               rede->aeroportos[i].nome,
                               rede->aeroportos[i].pais);
                    }
                }
                printf("\n[OK] Pesquisa por continente concluida.\n");
                aguardar_tecla();
                break;

            case 5:
                limpar_ecra();
                cabecalho("PESQUISAR AEROPORTO");
                printf("[ERRO] Funcionalidade ainda nao implementada (Semana 3 - Trie).\n");
                aguardar_tecla();
                break;

            case 0:
                continuar = 0;
                break;

            default:
                printf("[ERRO] Opcao invalida. Tente novamente.\n");
                aguardar_tecla();
                break;
        }
    }
}

void menu_voos(Rede *rede)
{
    int opcao;
    int continuar;

    continuar = 1;
    while (continuar == 1) {
        limpar_ecra();
        cabecalho("MENU VOOS");
        printf("1. Listar voos activos\n");
        printf("2. Pesquisar voo por codigo\n");
        printf("3. Adicionar voo manualmente\n");
        printf("4. Cancelar voo\n");
        printf("5. Ver fila de aterragem (torre de controlo)\n");
        printf("0. Voltar\n\n");
        printf("Opcao: ");

        opcao = ler_opcao();

        switch (opcao) {
            case 0:
                continuar = 0;
                break;
            default:
                printf("[ERRO] Funcionalidade ainda nao implementada (Semana 3/4).\n");
                aguardar_tecla();
                break;
        }
    }

    (void) rede;
}

void menu_rotas(Rede *rede)
{
    int opcao;
    int continuar;
    char cod_orig[MAX_CODIGO];
    char cod_dest[MAX_CODIGO];
    char cod_evitar[MAX_CODIGO];
    int i_orig;
    int i_dest;
    int i_evitar;
    int excluir[MAX_AEROPORTOS];
    int i;
    int cam[MAX_ROTA];
    int tam_cam;
    int isolados[MAX_AEROPORTOS];
    int n_isolados;
    ResultadoRota rota;
    ResultadoMST mst;

    continuar = 1;
    while (continuar == 1) {
        limpar_ecra();
        cabecalho("MENU ROTAS");
        printf("1. Calcular rota optima entre dois aeroportos (Dijkstra)\n");
        printf("2. Calcular rota evitando um aeroporto (Dijkstra alternativo)\n");
        printf("3. Ver rede minima -- Kruskal\n");
        printf("4. Ver rede minima -- Prim\n");
        printf("5. Verificar conectividade da rede (DFS)\n");
        printf("6. Aeroportos acessiveis a partir de um ponto (BFS)\n");
        printf("0. Voltar\n\n");
        printf("Opcao: ");

        opcao = ler_opcao();

        switch (opcao) {
            case 1:
                limpar_ecra();
                cabecalho("ROTA OPTIMA (DIJKSTRA)");

                i_orig = ler_codigo_iata(rede, "Aeroporto de origem (ex: LUA): ", cod_orig);
                if (i_orig == -1) {
                    printf("[ERRO] Aeroporto de origem nao encontrado.\n");
                    aguardar_tecla();
                    break;
                }

                i_dest = ler_codigo_iata(rede, "Aeroporto de destino (ex: CDG): ", cod_dest);
                if (i_dest == -1) {
                    printf("[ERRO] Aeroporto de destino nao encontrado.\n");
                    aguardar_tecla();
                    break;
                }

                rota = dijkstra_rota(rede, i_orig, i_dest);
                printf("\n");
                dijkstra_imprimir(rede, &rota);
                aguardar_tecla();
                break;

            case 2:
                limpar_ecra();
                cabecalho("ROTA EVITANDO AEROPORTO (DIJKSTRA ALTERNATIVO)");

                i_orig = ler_codigo_iata(rede, "Aeroporto de origem (ex: LUA): ", cod_orig);
                if (i_orig == -1) {
                    printf("[ERRO] Aeroporto de origem nao encontrado.\n");
                    aguardar_tecla();
                    break;
                }

                i_dest = ler_codigo_iata(rede, "Aeroporto de destino (ex: CDG): ", cod_dest);
                if (i_dest == -1) {
                    printf("[ERRO] Aeroporto de destino nao encontrado.\n");
                    aguardar_tecla();
                    break;
                }

                i_evitar = ler_codigo_iata(rede, "Aeroporto a evitar (ex: FRA): ", cod_evitar);
                if (i_evitar == -1) {
                    printf("[ERRO] Aeroporto a evitar nao encontrado.\n");
                    aguardar_tecla();
                    break;
                }

                for (i = 0; i < MAX_AEROPORTOS; i++) {
                    excluir[i] = 0;
                }
                excluir[i_evitar] = 1;

                rota = dijkstra_evitando(rede, i_orig, i_dest, excluir);
                printf("\n[INFO] A evitar: %s (%s)\n\n",
                       rede->aeroportos[i_evitar].codigo,
                       rede->aeroportos[i_evitar].nome);
                dijkstra_imprimir(rede, &rota);
                aguardar_tecla();
                break;

            case 3:
                limpar_ecra();
                cabecalho("REDE MINIMA -- KRUSKAL");
                mst = kruskal_mst(rede);
                mst_imprimir(rede, &mst, "Kruskal");
                aguardar_tecla();
                break;

            case 4:
                limpar_ecra();
                cabecalho("REDE MINIMA -- PRIM");
                printf("[INFO] A calcular a partir do aeroporto 0 (%s).\n\n",
                       rede->aeroportos[0].codigo);
                mst = prim_mst(rede, 0);
                mst_imprimir(rede, &mst, "Prim");
                aguardar_tecla();
                break;

            case 5:
                limpar_ecra();
                cabecalho("CONECTIVIDADE DA REDE (DFS)");
                if (dfs_conexo(rede) == 1) {
                    printf("[OK] A rede e totalmente conexa.\n");
                    printf("Todos os %d aeroportos sao alcancaveis entre si.\n",
                           rede->num_aeroportos);
                } else {
                    printf("[AVISO] A rede NAO e totalmente conexa.\n");
                    printf("Existem aeroportos isolados a partir de %s:\n",
                           rede->aeroportos[0].codigo);

                    n_isolados = dfs_isolados(rede, isolados);
                    for (i = 0; i < n_isolados; i++) {
                        printf("  -> %s  %s\n",
                               rede->aeroportos[isolados[i]].codigo,
                               rede->aeroportos[isolados[i]].nome);
                    }
                }
                aguardar_tecla();
                break;

            case 6:
                limpar_ecra();
                cabecalho("AEROPORTOS ACESSIVEIS (BFS)");

                i_orig = ler_codigo_iata(rede, "Aeroporto de partida (ex: LUA): ", cod_orig);
                if (i_orig == -1) {
                    printf("[ERRO] Aeroporto nao encontrado.\n");
                    aguardar_tecla();
                    break;
                }

                printf("\n[OK] Aeroportos acessiveis a partir de %s (%s):\n\n",
                       rede->aeroportos[i_orig].codigo,
                       rede->aeroportos[i_orig].nome);
                printf("%-4s %-36s %10s\n", "COD", "NOME", "SALTOS");

                for (i = 0; i < rede->num_aeroportos; i++) {
                    if (i == i_orig) {
                        continue;
                    }
                    tam_cam = bfs_caminho(rede, i_orig, i, cam);
                    if (tam_cam > 0) {
                        printf("%-4s %-36s %10d\n",
                               rede->aeroportos[i].codigo,
                               rede->aeroportos[i].nome,
                               tam_cam - 1);
                    }
                }
                aguardar_tecla();
                break;

            case 0:
                continuar = 0;
                break;

            default:
                printf("[ERRO] Opcao invalida. Tente novamente.\n");
                aguardar_tecla();
                break;
        }
    }
}

void menu_simulacao(Rede *rede)
{
    int opcao;
    int continuar;

    continuar = 1;
    while (continuar == 1) {
        limpar_ecra();
        cabecalho("MENU SIMULACAO");
        printf("1. Iniciar simulacao\n");
        printf("2. Avancar N ticks manualmente\n");
        printf("3. Ver mapa de voos em tempo real\n");
        printf("4. Gerar evento manual (trovoada, nevoeiro, etc.)\n");
        printf("5. Ver log de eventos\n");
        printf("0. Voltar\n\n");
        printf("Opcao: ");

        opcao = ler_opcao();

        switch (opcao) {
            case 0:
                continuar = 0;
                break;
            default:
                printf("[ERRO] Funcionalidade ainda nao implementada (Semana 4).\n");
                aguardar_tecla();
                break;
        }
    }

    (void) rede;
}

void menu_historico(Rede *rede)
{
    int opcao;
    int continuar;

    continuar = 1;
    while (continuar == 1) {
        limpar_ecra();
        cabecalho("MENU HISTORICO");
        printf("1. Listar todos os voos concluidos\n");
        printf("2. Buscar voo por ID\n");
        printf("3. Ver voos por intervalo de tempo\n");
        printf("4. Ranking de aeroportos mais movimentados\n");
        printf("5. Ranking de rotas mais longas\n");
        printf("0. Voltar\n\n");
        printf("Opcao: ");

        opcao = ler_opcao();

        switch (opcao) {
            case 0:
                continuar = 0;
                break;
            default:
                printf("[ERRO] Funcionalidade ainda nao implementada (Semana 3/4).\n");
                aguardar_tecla();
                break;
        }
    }

    (void) rede;
}

void menu_relatorio(Rede *rede)
{
    int opcao;
    int continuar;

    continuar = 1;
    while (continuar == 1) {
        limpar_ecra();
        cabecalho("MENU RELATORIO");
        printf("1. Gerar relatorio completo (ficheiro relatorio.txt)\n");
        printf("2. Ver relatorio no ecra\n");
        printf("3. Exportar log de voos ordenado\n");
        printf("0. Voltar\n\n");
        printf("Opcao: ");

        opcao = ler_opcao();

        switch (opcao) {
            case 0:
                continuar = 0;
                break;
            default:
                printf("[ERRO] Funcionalidade ainda nao implementada (Semana 4).\n");
                aguardar_tecla();
                break;
        }
    }

    (void) rede;
}

int main(void)
{
    Rede *rede;
    int opcao;
    int continuar;

    rede = carregar_ficheiros("dados/aeroportos.dat", "dados/rotas.dat");
    if (rede == NULL) {
        printf("[ERRO] Nao foi possivel carregar a rede. A encerrar.\n");
        return 1;
    }

    printf("\n[OK] RouteSim iniciado com sucesso.\n");
    aguardar_tecla();

    continuar = 1;
    while (continuar == 1) {
        limpar_ecra();
        cabecalho("MENU PRINCIPAL - RouteSim");
        printf("1. Aeroportos\n");
        printf("2. Voos\n");
        printf("3. Rotas\n");
        printf("4. Simulacao\n");
        printf("5. Historico\n");
        printf("6. Relatorio\n");
        printf("0. Sair\n\n");
        printf("Opcao: ");

        opcao = ler_opcao();

        switch (opcao) {
            case 1:
                menu_aeroportos(rede);
                break;
            case 2:
                menu_voos(rede);
                break;
            case 3:
                menu_rotas(rede);
                break;
            case 4:
                menu_simulacao(rede);
                break;
            case 5:
                menu_historico(rede);
                break;
            case 6:
                menu_relatorio(rede);
                break;
            case 0:
                continuar = 0;
                break;
            default:
                printf("[ERRO] Opcao invalida. Tente novamente.\n");
                aguardar_tecla();
                break;
        }
    }

    destruir_rede(rede);
    printf("[OK] RouteSim encerrado com sucesso.\n");

    return 0;
}
