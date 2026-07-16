#ifndef LISTACOMPRAS_H
#define LISTACOMPRAS_H

#include <vector>
#include <map>
#include <list>
#include <string>

// Estruturas do modulo ListaCompras
extern std::vector<std::string> vetorClientes;
extern std::map<std::string, int> mapaClientes;
extern std::vector<std::string> vetorProdutos;
extern std::map<std::string, int> mapaProdutos;
extern std::vector<std::list<int>> listaCompras;

// Funcoes do modulo ListaCompras
void carregarCompras(const char* nomeArquivo);
void exibirProdutosDoCliente(const char* codCliente);

#endif
