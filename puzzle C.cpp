// Valter Joaquim  fiz com colega Elton da Silva
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Movimento {
    bool esquerda, direita, cima, baixo;
};
typedef struct Movimento Movimento;

struct Arvore;

struct Lista {
    struct Arvore *valor;
    struct Lista *prox;
};
typedef struct Lista Lista;

struct Arvore {
    int tabuleiro[3][3];
    struct Arvore *pai;
    Lista *parentes;
    Lista *filhos;
};
typedef struct Arvore Arvore;

void adicionar_inicio(Lista **no, Lista *novo) {
    if (*no) {
        novo->prox = *no;
        *no = novo;
    } else {
        *no = novo;
        novo->prox = NULL;
    }
}

void empilhar(Lista **no, Lista *novo) {
    adicionar_inicio(no, novo);
}

Lista *remover_inicio(Lista **no) {
    if (*no) {
        Lista *elemento = (*no);
        (*no) = (*no)->prox;
        elemento->prox = NULL;
        return elemento;
    }
    return NULL;
}

Lista *desempilhar(Lista **no) {
    return remover_inicio(no);
}

Lista *criar_no_lista(Arvore *valor) {
    Lista *novo = malloc(sizeof(Lista));
    novo->valor = valor;
    novo->prox = NULL;
    return novo;
}

void mostrar_tabuleiro(const int tabuleiro[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%d ", tabuleiro[i][j]);
        }
        printf("\n");
    }
}

void copiar_tabuleiro(int dest[3][3], const int origem[3][3]) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            dest[i][j] = origem[i][j];
}

Arvore *criar_no_arvore(const int tabuleiro[3][3]) {
    Arvore *novo = malloc(sizeof(Arvore));
    novo->pai = NULL;
    novo->filhos = novo->parentes = NULL;
    copiar_tabuleiro(novo->tabuleiro, tabuleiro);
    return novo;
}

bool verificar_igualdade_tabuleiro(const int tabuleiro_x[3][3], const int tabuleiro_y[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (tabuleiro_x[i][j] != tabuleiro_y[i][j]) {
                return false;
            }
        }
    }
    return true;
}

bool verificar_existencia(Lista *lista, int tabuleiro[3][3]) {
    for (Lista *i = lista; i; i = i->prox)
        if (verificar_igualdade_tabuleiro(i->valor->tabuleiro, tabuleiro))
            return true;
    return false;
}

bool teste_de_objetivo(Arvore *no) {
    if (!no)
        return false;
    static const int solucao[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}
    };
    return verificar_igualdade_tabuleiro(no->tabuleiro, solucao);
}

void encontrar_espaco_vazio(const int tabuleiro[3][3], int *linha_vazia, int *coluna_vazia) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (tabuleiro[i][j] == 0) {
                *linha_vazia = i;
                *coluna_vazia = j;
            }
        }
    }
}

void mostrar_movimentos(Movimento movimento) {
    printf("Movimentos: ");
    if (movimento.esquerda)
        printf("esquerda ");
    if (movimento.direita)
        printf("direita ");
    if (movimento.cima)
        printf("cima ");
    if (movimento.baixo)
        printf("baixo ");
}

Movimento calcular_movimento(int i, int j) {
    Movimento mov = {
        .esquerda = j > 0,
        .direita = j < 2,
        .cima = i > 0,
        .baixo = i < 2
    };
    return mov;
}

Arvore *criar_no_movendo(const int tabuleiro[3][3], int linha_origem, int coluna_origem, int linha_destino, int coluna_destino) {
    int novo_tabuleiro[3][3];
    copiar_tabuleiro(novo_tabuleiro, tabuleiro);

    int aux = novo_tabuleiro[linha_origem][coluna_origem];
    novo_tabuleiro[linha_origem][coluna_origem] = novo_tabuleiro[linha_destino][coluna_destino];
    novo_tabuleiro[linha_destino][coluna_destino] = aux;

    return criar_no_arvore(novo_tabuleiro);
}

Lista *gerar_tabuleiros(const int tabuleiro[3][3], Movimento movimentos, int linha, int coluna) {
    Lista *filhos = NULL;
    if (movimentos.esquerda)
        adicionar_inicio(&filhos, criar_no_lista(criar_no_movendo(tabuleiro, linha, coluna, linha, coluna - 1)));
    if (movimentos.direita)
        adicionar_inicio(&filhos, criar_no_lista(criar_no_movendo(tabuleiro, linha, coluna, linha, coluna + 1)));
    if (movimentos.cima)
        adicionar_inicio(&filhos, criar_no_lista(criar_no_movendo(tabuleiro, linha, coluna, linha - 1, coluna)));
    if (movimentos.baixo)
        adicionar_inicio(&filhos, criar_no_lista(criar_no_movendo(tabuleiro, linha, coluna, linha + 1, coluna)));
    return filhos;
}

Lista *modelo_de_transicao(Arvore *no) {
    int linha, coluna;
    encontrar_espaco_vazio(no->tabuleiro, &linha, &coluna);
    Movimento movimentos = calcular_movimento(linha, coluna);
    Lista *lista = gerar_tabuleiros(no->tabuleiro, movimentos, linha, coluna), *lista_final = NULL;

    Lista *no_i;

    while ((no_i = remover_inicio(&lista))) {
        if (verificar_existencia(no->parentes, no_i->valor->tabuleiro)) {
            free(no_i);
        } else {
            adicionar_inicio(&lista_final, no_i);
        }
    }

    return lista_final;
}

void criar_caminho_tabuleiros(Arvore *no) {
    Arvore *original = no;
    while (no->pai) {
        no = no->pai;
        empilhar(&original->parentes, criar_no_lista(no));
    }
}

Arvore *imprimir(Arvore *no, bool *solucao, int contador, int limite);

void mostrar_caminho(Arvore *no) {
    int c = 0;
    for (Lista *i = no->parentes; i; i = i->prox) {
        c++;
        printf(".\n");
        mostrar_tabuleiro(i->valor->tabuleiro);
    }
    printf("\n%d\n", c);
}

int main() {
    int tabuleiro[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 0, 8}
    };
    bool solucao_encontrada = false;
    Arvore *no = criar_no_arvore(tabuleiro);
    Arvore *sol = imprimir(no, &solucao_encontrada, 0, 10); // Aumentei o limite para 10 para garantir a solução
    printf("\n");
    mostrar_caminho(sol);
    printf("\n");
    mostrar_tabuleiro(sol->tabuleiro);

    // Liberação de memória - percorra a árvore e libere os nós e listas alocadas
    return 0;
}

Arvore *imprimir(Arvore *no, bool *solucao, int contador, int limite) {
    if (teste_de_objetivo(no)) {
        criar_caminho_tabuleiros(no);
        printf("Solução encontrada!\n");
        *solucao = true;
        return no;
    } else if (!(*solucao) && contador < limite) {
        Arvore *sol;
        criar_caminho_tabuleiros(no);
        no->filhos = modelo_de_transicao(no);
        for (Lista *i = no->filhos; i; i = i->prox) {
            i->valor->pai = no;
            sol = imprimir(i->valor, solucao, contador + 1, limite);
            if (sol)
                return sol;
        }
    }
    return NULL;
}

