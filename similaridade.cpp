#include <stdio.h>
#include <vector>
#include "listaCompras.h"

// Constroi a matriz de compras densa A (n clientes x m produtos)
// A[i][j] = 1 se cliente i comprou produto j, 0 caso contrario
std::vector<std::vector<int>> construirMatrizCompras() {
    int n = (int)vetorClientes.size();
    int m = (int)vetorProdutos.size();

    // Inicializa matriz n x m com zeros
    std::vector<std::vector<int>> A(n, std::vector<int>(m, 0));

    for (int i = 0; i < n; i++) {
        for (int idxProduto : listaCompras[i]) {
            A[i][idxProduto] = 1;
        }
    }

    return A;
}

// Calcula a matriz de intersecao I = A x A^T
// I[i][j] = numero de produtos em comum entre cliente i e cliente j
std::vector<std::vector<int>> calcularMatrizIntersecao(
        const std::vector<std::vector<int>>& A) {

    int n = (int)A.size();
    int m = (int)A[0].size();

    std::vector<std::vector<int>> I(n, std::vector<int>(n, 0));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int soma = 0;
            for (int k = 0; k < m; k++) {
                soma += A[i][k] * A[j][k];
            }
            I[i][j] = soma;
        }
    }

    return I;
}

// Calcula a matriz de similaridade S usando distancia de Jaccard
// S[i][j] = 1 - I[i][j] / |Pi|
// onde |Pi| e o numero de produtos comprados pelo cliente i
std::vector<std::vector<double>> calcularMatrizSimilaridade(
        const std::vector<std::vector<int>>& I) {

    int n = (int)I.size();

    std::vector<std::vector<double>> S(n, std::vector<double>(n, 0.0));

    for (int i = 0; i < n; i++) {
        int totalProdutosI = (int)listaCompras[i].size();

        for (int j = 0; j < n; j++) {
            if (totalProdutosI == 0) {
                S[i][j] = 1.0; // sem produtos, maxima dissimilaridade
            } else {
                S[i][j] = 1.0 - (double)I[i][j] / totalProdutosI;
            }
        }
    }

    return S;
}

// Retorna o indice do cliente mais similar ao cliente dado
// (similaridade menor que 1 e que nao seja ele mesmo)
int clienteMaisSimilar(const std::vector<std::vector<double>>& S, int indiceCliente) {
    int n = (int)S.size();
    int melhor = -1;
    double menorSimilaridade = 2.0; // valor impossivel para inicializar

    for (int j = 0; j < n; j++) {
        if (j == indiceCliente) continue;           // ignora ele mesmo
        if (S[indiceCliente][j] >= 1.0) continue;  // ignora dissimilares

        if (S[indiceCliente][j] < menorSimilaridade) {
            menorSimilaridade = S[indiceCliente][j];
            melhor = j;
        }
    }

    return melhor;
}

int main() {
    // Carrega os dados do modulo ListaCompras
    carregarCompras("compras.csv");

    printf("=== MODULO SIMILARIDADE ===\n");
    printf("Clientes: %d | Produtos: %d\n\n",
           (int)vetorClientes.size(), (int)vetorProdutos.size());

    // Constroi as matrizes
    std::vector<std::vector<int>> A = construirMatrizCompras();
    std::vector<std::vector<int>> I = calcularMatrizIntersecao(A);
    std::vector<std::vector<double>> S = calcularMatrizSimilaridade(I);

    // Testador: para cada cliente, exibe o mais similar
    int n = (int)vetorClientes.size();
    for (int i = 0; i < n; i++) {
        int similar = clienteMaisSimilar(S, i);

        printf("Cliente [%d] %s -> ", i, vetorClientes[i].c_str());

        if (similar == -1) {
            printf("nenhum cliente similar encontrado\n");
        } else {
            printf("mais similar: [%d] %s (S = %.4f)\n",
                   similar,
                   vetorClientes[similar].c_str(),
                   S[i][similar]);
        }
    }

    return 0;
}
