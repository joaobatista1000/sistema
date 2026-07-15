#include <stdio.h>
#include "listaCompras.h"

// Definicao das estruturas globais
std::vector<std::string> vetorClientes;
std::map<std::string, int> mapaClientes;
std::vector<std::string> vetorProdutos;
std::map<std::string, int> mapaProdutos;
std::vector<std::list<int>> listaCompras;

// Funcao que processa o CSV e preenche todas as estruturas
void carregarCompras(const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        return;
    }

    char data[16], codCliente[32], codProduto[32], nomeProduto[256];

    // Pula o cabecalho
    fscanf(arquivo, "%*[^\n]\n");

    // Primeira passagem: preenche vetores e mapas
    while (fscanf(arquivo, "%15[^,],%31[^,],%31[^,],%255[^\n]\n",
                  data, codCliente, codProduto, nomeProduto) == 4) {

        std::string sCodCliente(codCliente);
        std::string sCodProduto(codProduto);
        std::string sNomeProduto(nomeProduto);

        if (mapaClientes.find(sCodCliente) == mapaClientes.end()) {
            mapaClientes[sCodCliente] = (int)vetorClientes.size();
            vetorClientes.push_back(sCodCliente);
        }

        if (mapaProdutos.find(sCodProduto) == mapaProdutos.end()) {
            mapaProdutos[sCodProduto] = (int)vetorProdutos.size();
            vetorProdutos.push_back(sNomeProduto);
        }
    }

    // Inicializa lista de compras com lista vazia para cada cliente
    listaCompras.resize(vetorClientes.size());

    // Volta ao inicio para segunda passagem
    rewind(arquivo);
    fscanf(arquivo, "%*[^\n]\n");

    // Segunda passagem: preenche a lista de compras
    while (fscanf(arquivo, "%15[^,],%31[^,],%31[^,],%255[^\n]\n",
                  data, codCliente, codProduto, nomeProduto) == 4) {

        std::string sCodCliente(codCliente);
        std::string sCodProduto(codProduto);

        int indiceCliente = mapaClientes[sCodCliente];
        int indiceProduto = mapaProdutos[sCodProduto];

        // Adiciona produto sem duplicatas
        bool jaExiste = false;
        for (int idx : listaCompras[indiceCliente]) {
            if (idx == indiceProduto) {
                jaExiste = true;
                break;
            }
        }
        if (!jaExiste) {
            listaCompras[indiceCliente].push_back(indiceProduto);
        }
    }

    fclose(arquivo);
}

// Exibe os produtos comprados por um cliente dado seu codigo original
void exibirProdutosDoCliente(const char* codCliente) {
    std::string sCodCliente(codCliente);

    if (mapaClientes.find(sCodCliente) == mapaClientes.end()) {
        printf("Cliente %s nao encontrado.\n", codCliente);
        return;
    }

    int indice = mapaClientes[sCodCliente];
    printf("\nCliente: %s (indice interno: %d)\n", codCliente, indice);
    printf("Produtos comprados:\n");

    for (int idxProduto : listaCompras[indice]) {
        std::string codProd = "";
        for (auto& par : mapaProdutos) {
            if (par.second == idxProduto) {
                codProd = par.first;
                break;
            }
        }
        printf("  - [%s] %s\n", codProd.c_str(), vetorProdutos[idxProduto].c_str());
    }
}
