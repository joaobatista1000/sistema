#include <stdio.h>
#include <vector>
#include <algorithm>
#include "listaCompras.h"

// Estrutura que representa o ranqueamento de um produto
struct ProdutoRanking {
    int indiceProduto;
    double ranking;
};

// Funcao de comparacao para ordenar por ranking (menor = mais recomendado)
bool comparar(const ProdutoRanking& a, const ProdutoRanking& b) {
    return a.ranking < b.ranking;
}

// Calcula a matriz de similaridade (igual ao modulo Similaridade)
std::vector<std::vector<double>> calcularSimilaridade() {
    int n = (int)vetorClientes.size();
    int m = (int)vetorProdutos.size();

    // Monta matriz de compras A
    std::vector<std::vector<int>> A(n, std::vector<int>(m, 0));
    for (int i = 0; i < n; i++) {
        for (int idxProduto : listaCompras[i]) {
            A[i][idxProduto] = 1;
        }
    }

    // Calcula matriz de intersecao I = A x A^T
    std::vector<std::vector<int>> I(n, std::vector<int>(n, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < m; k++) {
                I[i][j] += A[i][k] * A[j][k];
            }
        }
    }

    // Calcula matriz de similaridade S com Jaccard
    std::vector<std::vector<double>> S(n, std::vector<double>(n, 1.0));
    for (int i = 0; i < n; i++) {
        int totalI = (int)listaCompras[i].size();
        for (int j = 0; j < n; j++) {
            if (totalI > 0) {
                S[i][j] = 1.0 - (double)I[i][j] / totalI;
            }
        }
    }

    return S;
}

// Recomenda os k produtos mais relevantes para um cliente
void recomendar(const char* codCliente, int k,
                const std::vector<std::vector<double>>& S) {

    std::string sCodCliente(codCliente);

    if (mapaClientes.find(sCodCliente) == mapaClientes.end()) {
        printf("Cliente %s nao encontrado.\n", codCliente);
        return;
    }

    int c = mapaClientes[sCodCliente]; // indice interno do cliente
    int n = (int)vetorClientes.size();
    int m = (int)vetorProdutos.size();

    // Passo 1: identifica vizinhos (similaridade < 1 e nao eh ele mesmo)
    std::vector<int> vizinhos;
    for (int j = 0; j < n; j++) {
        if (j != c && S[c][j] < 1.0) {
            vizinhos.push_back(j);
        }
    }

    // Passo 2: inicializa vetor de ranking com 1 para todos os produtos
    std::vector<ProdutoRanking> R(m);
    for (int p = 0; p < m; p++) {
        R[p].indiceProduto = p;
        R[p].ranking = 1.0;
    }

    // Monta conjunto de produtos ja comprados pelo cliente c
    std::vector<bool> compradoPorC(m, false);
    for (int idxProduto : listaCompras[c]) {
        compradoPorC[idxProduto] = true;
    }

    // Passo 3: calcula o ranking
    for (int s : vizinhos) {
        for (int idxProduto : listaCompras[s]) {
            if (!compradoPorC[idxProduto]) {
                R[idxProduto].ranking *= S[c][s];
            }
        }
    }

    // Passo 4: ordena de forma nao decrescente e exibe os k primeiros
    std::sort(R.begin(), R.end(), comparar);

    printf("\nRecomendacoes para o cliente %s:\n", codCliente);
    int exibidos = 0;
    for (int i = 0; i < m && exibidos < k; i++) {
        int idx = R[i].indiceProduto;
        // Exibe apenas produtos nao comprados pelo cliente
        if (!compradoPorC[idx]) {
            printf("  %d. %s (ranking: %.4f)\n",
                   exibidos + 1,
                   vetorProdutos[idx].c_str(),
                   R[i].ranking);
            exibidos++;
        }
    }

    if (exibidos == 0) {
        printf("  Nenhuma recomendacao disponivel.\n");
    }
}

int main() {
    // Carrega os dados
    carregarCompras("compras.csv");

    // Calcula a matriz de similaridade
    std::vector<std::vector<double>> S = calcularSimilaridade();

    printf("=== MODULO RECOMENDACAO ===\n");

    int k = 3; // numero de produtos a recomendar

    // Testa com 3 clientes pelos codigos originais
    recomendar("05090301", k, S);
    recomendar("05190001", k, S);
    recomendar("23210301", k, S);

    return 0;
}
