#include <stdio.h>
#include <vector>
#include <map>
#include <list>
#include <string>

// Estruturas do modulo ListaCompras
std::vector<std::string> vetorClientes;           // vetor com codigos dos clientes
std::map<std::string, int> mapaClientes;          // codigo do cliente -> indice interno

std::vector<std::string> vetorProdutos;           // vetor com nomes dos produtos
std::map<std::string, int> mapaProdutos;          // codigo do produto -> indice interno

std::vector<std::list<int>> listaCompras;         // para cada cliente, lista de indices de produtos

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

    // Primeira passagem: preenche vetores e mapas de clientes e produtos
    while (fscanf(arquivo, "%15[^,],%31[^,],%31[^,],%255[^\n]\n",
                  data, codCliente, codProduto, nomeProduto) == 4) {

        std::string sCodCliente(codCliente);
        std::string sCodProduto(codProduto);
        std::string sNomeProduto(nomeProduto);

        // Adiciona cliente se ainda nao existe
        if (mapaClientes.find(sCodCliente) == mapaClientes.end()) {
            mapaClientes[sCodCliente] = (int)vetorClientes.size();
            vetorClientes.push_back(sCodCliente);
        }

        // Adiciona produto se ainda nao existe
        if (mapaProdutos.find(sCodProduto) == mapaProdutos.end()) {
            mapaProdutos[sCodProduto] = (int)vetorProdutos.size();
            vetorProdutos.push_back(sNomeProduto);
        }
    }

    // Inicializa a lista de compras com uma lista vazia para cada cliente
    listaCompras.resize(vetorClientes.size());

    // Volta ao inicio do arquivo para segunda passagem
    rewind(arquivo);
    fscanf(arquivo, "%*[^\n]\n"); // pula cabecalho novamente

    // Segunda passagem: preenche a lista de compras
    while (fscanf(arquivo, "%15[^,],%31[^,],%31[^,],%255[^\n]\n",
                  data, codCliente, codProduto, nomeProduto) == 4) {

        std::string sCodCliente(codCliente);
        std::string sCodProduto(codProduto);

        int indiceCliente = mapaClientes[sCodCliente];
        int indiceProduto = mapaProdutos[sCodProduto];

        // Adiciona o produto na lista do cliente (sem duplicatas)
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

// Funcao testadora: exibe os produtos comprados por um cliente dado seu codigo original
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
        // Busca o codigo do produto pelo indice
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

int main() {
    carregarCompras("compras.csv");

    printf("=== MODULO LISTACOMPRAS ===\n");
    printf("Total de clientes: %d\n", (int)vetorClientes.size());
    printf("Total de produtos: %d\n", (int)vetorProdutos.size());

    // Testa com 3 clientes conforme exigido pelo PDF
    exibirProdutosDoCliente("05090301");
    exibirProdutosDoCliente("05190001");
    exibirProdutosDoCliente("23210301");

    return 0;
}
