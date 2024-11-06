#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

#ifndef _DATABASE_H_
#include "database.h"
#define _DATABASE_H_
#endif

int posBlocks[NUM_BUCKETS] = {0};
int numRowBucket[NUM_BUCKETS] = {0};
int tamListBlocks[NUM_BUCKETS] = {0};

using namespace std;

bool lerRegistroDoBloco(const char *block, int &posBloco, TableRow &entrada)
{
    if (posBloco >= 4096)
        return false;

    std::memcpy(&entrada.id, block + posBloco, sizeof(entrada.id));
    posBloco += sizeof(entrada.id);

    if (entrada.id == -1)
        return false;

    std::memcpy(&entrada.titulo, block + posBloco, sizeof(entrada.titulo));
    posBloco += sizeof(entrada.titulo);
    std::memcpy(&entrada.ano, block + posBloco, sizeof(entrada.ano));
    posBloco += sizeof(entrada.ano);
    std::memcpy(&entrada.autores, block + posBloco, sizeof(entrada.autores));
    posBloco += sizeof(entrada.autores);
    std::memcpy(&entrada.citacoes, block + posBloco, sizeof(entrada.citacoes));
    posBloco += sizeof(entrada.citacoes);
    std::memcpy(&entrada.atualizacao, block + posBloco, sizeof(entrada.atualizacao));
    posBloco += sizeof(entrada.atualizacao);

    int tamSnippet;
    std::memcpy(&tamSnippet, block + posBloco, sizeof(tamSnippet));
    posBloco += sizeof(tamSnippet);
    entrada.snippet = std::string(block + posBloco, tamSnippet);
    posBloco += sizeof(char) * tamSnippet;

    return true;
}

void findrec(const std::string &nomeArquivo, int ID)
{
    std::ifstream arquivo(nomeArquivo, std::ios::binary);
    if (!arquivo.is_open())
    {
        std::cerr << "Erro ao abrir o arquivo " << nomeArquivo << "." << std::endl;
        return;
    }

    int bucket = calcularHash(ID);
    std::streampos posicaoInicial = bucket * NUM_BLOCKS * MAX_BLOCK_SIZE;
    arquivo.seekg(posicaoInicial);

    char block[4096];
    int numLinhas = 0;
    arquivo.read(reinterpret_cast<char *>(block), 4096);
    std::memcpy(&numLinhas, block, sizeof(int));
    arquivo.seekg(posicaoInicial);

    int linhasLidas = 0;
    int blocosLidos = 0;

    while (arquivo.read(reinterpret_cast<char *>(block), 4096) && linhasLidas < numLinhas)
    {
        blocosLidos++;
        int posBloco = (blocosLidos == 1) ? sizeof(int) : 0;

        TableRow entrada;
        while (lerRegistroDoBloco(block, posBloco, entrada) && linhasLidas <= numLinhas)
        {
            linhasLidas++;
            printf("ID: %d\n", entrada.id);
            if (ID == entrada.id)
            {
                printTableRow(entrada);
                std::cout << "------------------------------------" << std::endl;
                std::cout << "Total de Blocos: " << MAX_BLOCK_SIZE * NUM_BLOCKS << std::endl;
                std::cout << "Blocos Lidos: " << blocosLidos << std::endl;

                return;
            }
        }
    }
    std::cout << "Registro não encontrado" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " <ID>" << std::endl;
        return 1;
    }

    int ID = stoi(argv[1]);
    findrec("hash_table.dat", ID);
    return 0;
}