#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <ctype.h>

#define NUM_TERRITORIOS 5
#define TAM_NOME 30
#define TAM_COR 10
#define NUM_MISSOES 3

// Struct que representa um território
typedef struct {
    char nome[TAM_NOME];
    char cor[TAM_COR];   // cor do exército dominante (string curta)
    int tropas;          // número de tropas no território
} Territorio;

// ----------------- Protótipos -----------------
Territorio* alocarMapa();
void inicializarTerritorios(Territorio* mapa);
void liberarMemoria(Territorio* mapa);

void exibirMapa(const Territorio* mapa);
void exibirMenuPrincipal();
void exibirMissao(int idMissao);

int sortearMissao();
int verificarVitoria(const Territorio* mapa, const char* corJogador, int idMissao);

void limparBufferEntrada();
void toUpperStr(char* s);

void faseDeAtaque(Territorio* mapa, const char* corJogador);
void simularAtaque(Territorio* mapa, int origem, int destino);

int compararDesc(const void* a, const void* b);

// ----------------- Função main -----------------
int main() {
    setlocale(LC_ALL, "Portuguese");
    srand((unsigned)time(NULL));

    Territorio* mapa = alocarMapa();
    if (mapa == NULL) {
        printf("Erro ao alocar memória para o mapa.\n");
        return 1;
    }

    inicializarTerritorios(mapa);

    char corJogador[TAM_COR];
    printf("Digite a cor do seu exército (ex: VERMELHO ou AZUL): ");
    if (fgets(corJogador, sizeof(corJogador), stdin) != NULL) {
        // remover newline
        corJogador[strcspn(corJogador, "\n")] = '\0';
    } else {
        strncpy(corJogador, "VERMELHO", TAM_COR);
        corJogador[TAM_COR-1] = '\0';
    }
    toUpperStr(corJogador);

    int idMissao = sortearMissao();
    int opcao = -1;

    do {
        exibirMapa(mapa);
        exibirMissao(idMissao);
        exibirMenuPrincipal();

        printf("Escolha uma opção: ");
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida. Tente novamente.\n");
            limparBufferEntrada();
            opcao = -1;
            continue;
        }
        limparBufferEntrada();

        switch (opcao) {
            case 1:
                faseDeAtaque(mapa, corJogador);
                break;
            case 2:
                if (verificarVitoria(mapa, corJogador, idMissao)) {
                    printf("\nParabéns! Você cumpriu sua missão!\n");
                    opcao = 0; // encerra
                } else {
                    printf("\nVocê ainda não cumpriu sua missão.\n");
                }
                break;
            case 0:
                printf("Saindo do jogo...\n");
                break;
            default:
                printf("Opção inválida.\n");
        }

    } while (opcao != 0);

    liberarMemoria(mapa);

    // Pausa final para que o usuário veja a saída antes de fechar
    printf("\nPressione ENTER para sair...");
    getchar();

    return 0;
}

// ----------------- Implementação das funções -----------------

// Aloca dinamicamente o vetor de territórios
Territorio* alocarMapa() {
    return (Territorio*) calloc(NUM_TERRITORIOS, sizeof(Territorio));
}

// Inicializa nomes, cores e tropas dos territórios
void inicializarTerritorios(Territorio* mapa) {
    const char* nomes[NUM_TERRITORIOS] = {"Acre", "Bahia", "Ceara", "Amazonas", "Parana"};
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        strncpy(mapa[i].nome, nomes[i], TAM_NOME);
        mapa[i].nome[TAM_NOME-1] = '\0';
        // Distribuição inicial alternada de cores para exemplo
        if (i % 2 == 0) {
            strncpy(mapa[i].cor, "VERMELHO", TAM_COR);
        } else {
            strncpy(mapa[i].cor, "AZUL", TAM_COR);
        }
        mapa[i].cor[TAM_COR-1] = '\0';
        mapa[i].tropas = 3 + rand() % 8; // entre 3 e 10 tropas
    }
}

// Libera memória alocada
void liberarMemoria(Territorio* mapa) {
    free(mapa);
}

// Exibe tabela com os territórios (função de leitura => const)
void exibirMapa(const Territorio* mapa) {
    printf("\n===== MAPA DE TERRITÓRIOS =====\n");
    printf("%-3s %-12s %-10s %s\n", "ID", "NOME", "COR", "TROPAS");
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        printf("%-3d %-12s %-10s %d\n", i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

// Menu principal
void exibirMenuPrincipal() {
    printf("\n===== MENU =====\n");
    printf("1. Fase de ataque\n");
    printf("2. Verificar missão\n");
    printf("0. Sair do jogo\n");
}

// Exibe missão sorteada
void exibirMissao(int idMissao) {
    printf("\n>>> Sua missão: ");
    switch (idMissao) {
        case 0:
            printf("Conquistar 3 territórios.\n");
            break;
        case 1:
            printf("Eliminar todos os exércitos AZUIS.\n");
            break;
        case 2:
            printf("Controlar todos os territórios de índice par (1-based).\n");
            break;
        default:
            printf("Missão desconhecida.\n");
    }
}

// Sorteia ID de missão
int sortearMissao() {
    return rand() % NUM_MISSOES;
}

// Verifica se a missão foi cumprida (retorna 1 = true, 0 = false)
int verificarVitoria(const Territorio* mapa, const char* corJogador, int idMissao) {
    int cont = 0;
    switch (idMissao) {
        case 0: // conquistar 3 territórios
            for (int i = 0; i < NUM_TERRITORIOS; i++) {
                if (strcmp(mapa[i].cor, corJogador) == 0) cont++;
            }
            return cont >= 3;
        case 1: // eliminar todos os AZUL
            for (int i = 0; i < NUM_TERRITORIOS; i++) {
                if (strcmp(mapa[i].cor, "AZUL") == 0) return 0;
            }
            return 1;
        case 2: // controlar todos os territórios pares (IDs 2,4,... em 1-based)
            for (int i = 1; i < NUM_TERRITORIOS; i += 2) {
                if (strcmp(mapa[i].cor, corJogador) != 0) return 0;
            }
            return 1;
        default:
            return 0;
    }
}

// Limpa buffer de entrada (stdin)
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// Converte string para maiúsculas (in-place)
void toUpperStr(char* s) {
    for (; *s; ++s) *s = (char) toupper((unsigned char)*s);
}

// ---------- Fase de ataque: interface e lógica ----------

// Gerencia a fase de ataque: pede territórios de origem/destino e chama simularAtaque
void faseDeAtaque(Territorio* mapa, const char* corJogador) {
    exibirMapa(mapa);

    int orig, dest;
    printf("\n--- Fase de Ataque ---\n");
    printf("Digite o ID do território de origem (quem você controla e com >1 tropa): ");
    if (scanf("%d", &orig) != 1) {
        printf("Entrada inválida.\n");
        limparBufferEntrada();
        return;
    }
    limparBufferEntrada();

    printf("Digite o ID do território de destino (o que pretende atacar): ");
    if (scanf("%d", &dest) != 1) {
        printf("Entrada inválida.\n");
        limparBufferEntrada();
        return;
    }
    limparBufferEntrada();

    // validações de índice
    if (orig < 1 || orig > NUM_TERRITORIOS || dest < 1 || dest > NUM_TERRITORIOS) {
        printf("IDs informados fora do intervalo.\n");
        return;
    }
    if (orig == dest) {
        printf("Origem e destino não podem ser o mesmo território.\n");
        return;
    }

    int iOrig = orig - 1;
    int iDest = dest - 1;

    // validação de propriedade e tropas
    if (strcmp(mapa[iOrig].cor, corJogador) != 0) {
        printf("Você não controla o território de origem.\n");
        return;
    }
    if (mapa[iOrig].tropas <= 1) {
        printf("O território de origem precisa ter pelo menos 2 tropas para atacar (1 deve ficar como ocupação).\n");
        return;
    }

    // executar ataque (uma sequência de confrontos até que uma condição ocorra)
    simularAtaque(mapa, iOrig, iDest);
}

// Simula um confronto entre dois territórios (origem e destino são índices 0-based)
void simularAtaque(Territorio* mapa, int origem, int destino) {
    // Simulação simples: repetimos confrontos até que atacante fique com 1 tropa ou defensor com 0
    printf("\nIniciando combate entre '%s' (origem) e '%s' (destino).\n",
           mapa[origem].nome, mapa[destino].nome);

    while (mapa[origem].tropas > 1 && mapa[destino].tropas > 0) {
        int atkTropas = mapa[origem].tropas;
        int defTropas = mapa[destino].tropas;

        int atkDice = atkTropas - 1; // máximo que pode usar
        if (atkDice > 3) atkDice = 3;
        if (atkDice < 1) atkDice = 1;

        int defDice = defTropas;
        if (defDice > 2) defDice = 2;
        if (defDice < 1) defDice = 1;

        int atkRolls[3] = {0,0,0};
        int defRolls[2] = {0,0};

        for (int i = 0; i < atkDice; i++) atkRolls[i] = 1 + rand() % 6;
        for (int i = 0; i < defDice; i++) defRolls[i] = 1 + rand() % 6;

        // ordenar desc
        qsort(atkRolls, atkDice, sizeof(int), compararDesc);
        qsort(defRolls, defDice, sizeof(int), compararDesc);

        printf("\nAtacante rolagens: ");
        for (int i = 0; i < atkDice; i++) printf("%d ", atkRolls[i]);
        printf("\nDefensor rolagens:  ");
        for (int i = 0; i < defDice; i++) printf("%d ", defRolls[i]);
        printf("\n");

        int comparacoes = (atkDice < defDice) ? atkDice : defDice;
        for (int i = 0; i < comparacoes; i++) {
            if (atkRolls[i] > defRolls[i]) {
                // defensor perde uma tropa
                mapa[destino].tropas--;
                printf("Comparação %d: atacante %d > defensor %d -> defensor perde 1 tropa\n",
                       i+1, atkRolls[i], defRolls[i]);
            } else {
                // atacante perde uma tropa (empate favorece defensor)
                mapa[origem].tropas--;
                printf("Comparação %d: atacante %d <= defensor %d -> atacante perde 1 tropa\n",
                       i+1, atkRolls[i], defRolls[i]);
            }
        }

        printf("Estado atual: %s tropas=%d | %s tropas=%d\n",
               mapa[origem].nome, mapa[origem].tropas,
               mapa[destino].nome, mapa[destino].tropas);

        // permitir que o jogador decida continuar atacando ou parar
        if (mapa[origem].tropas > 1 && mapa[destino].tropas > 0) {
            char escolha[4];
            printf("Deseja continuar atacando este território? (s/n): ");
            if (fgets(escolha, sizeof(escolha), stdin) == NULL) {
                break;
            }
            // considera apenas primeira letra
            if (tolower((unsigned char)escolha[0]) != 's') {
                printf("Ataque interrompido pelo jogador.\n");
                break;
            }
        }
    }

    // se defensor fora de tropas => conquista
    if (mapa[destino].tropas <= 0) {
        printf("\nTerritório %s conquistado!\n", mapa[destino].nome);
        // atualiza dono para o dono do território de origem
        strncpy(mapa[destino].cor, mapa[origem].cor, TAM_COR);
        mapa[destino].cor[TAM_COR-1] = '\0';
        // mover pelo menos 1 tropa do origin para ocupar
        mapa[origem].tropas--;
        mapa[destino].tropas = 1;
        printf("Uma tropa foi movida de %s para %s para ocupar o território.\n",
               mapa[origem].nome, mapa[destino].nome);
    } else if (mapa[origem].tropas <= 1) {
        printf("\nAtaque finalizado: tropas atacantes insuficientes para continuar.\n");
    } else {
        printf("\nAtaque finalizado.\n");
    }
}

// Comparador para qsort (ordena em ordem decrescente)
int compararDesc(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ib - ia);
}
