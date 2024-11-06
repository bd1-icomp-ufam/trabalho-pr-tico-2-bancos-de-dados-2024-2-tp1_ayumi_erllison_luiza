#include "definicoes.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <iomanip>

using namespace std;

// Definição da estrutura TableRow para armazenar informações de uma linha da tabela
struct TableRow
{
    int id;
    char titulo[300];
    int ano;
    char autores[150];
    int citacoes;
    char atualizacao[20];
    std::string snippet;
};

// Constantes usadas para o tamanho máximo de bloco, número de buckets e número de blocos
constexpr int MAX_BLOCK_SIZE = 4096;
constexpr int NUM_BUCKETS = 12101;
constexpr int NUM_BLOCKS = 23;

// Nome do arquivo onde a tabela hash será armazenada
const std::string nameFileHash = "hash_table.dat";

// Função para imprimir os detalhes de um TableRow
void printTableRow(const TableRow &row)
{
    std::cout << "ID: " << row.id << std::endl;
    std::cout << "Título: " << row.titulo << std::endl;
    std::cout << "Ano: " << row.ano << std::endl;
    std::cout << "Autores: " << row.autores << std::endl;
    std::cout << "Citações: " << row.citacoes << std::endl;
    std::cout << "Atualização: " << row.atualizacao << std::endl;
    std::cout << "Snippet: " << row.snippet << std::endl;
}

// Função para calcular o hash de um número, retornando um índice para a tabela hash
int calcularHash(int numero)
{
    return (numero % NUM_BUCKETS);
}

typedef struct
{
    int id = -1;
    char titulo[301];
    int ano;
    char autores[151];
    int citacoes;
    char atualizacoes[20];
    char snipet[101];
} registro_t;

typedef struct
{
    registro_t registros[BLOCK_FACTOR];
    char preenchimento[544];
} bloco_t;

// Faz a inicialização do arquivo de dados
void inicializarArquivo(FILE *arquivoDados);

// Insere um novo registro no bloco de dados
int inserirRegistroEmBloco(registro_t *registro, bloco_t *bloco);

// Pega o próximo nó do arquivo de index primario/secundario
long pegarProximoNoOffset(FILE *arqIndex);

// Faz uma busca pela chave (id) no bloco
int buscarRegistroBloco(int id, bloco_t *bloco, registro_t *registro);

void inicializarArquivo(FILE *arquivoDados)
{
    bloco_t blocoVazio;

    // Escreve todo o arquivo com blocos vazios
    for (int i = 0; i < QTD_BLOCKS; i++)
    {
        fwrite(&blocoVazio, sizeof(bloco_t), 1, arquivoDados);
    }

    // Move o cursor para o inicio do arquivo
    fseek(arquivoDados, 0, SEEK_SET);
}

int inserirRegistroEmBloco(registro_t *registro, bloco_t *bloco)
{

    // Busca um indice livre no bloco para inserir o registro passado por parametro
    for (int i = 0; i < BLOCK_FACTOR; i++)
    {
        // Caso tenha uma posicao livre ele copia esse registro para o bloco e retorna 1
        if (bloco->registros[i].id == -1 || bloco->registros[i].id == 0)
        {
            memcpy(&bloco->registros[i], registro, sizeof(registro_t));
            return 1;
        }
    }

    // Caso contrario (tudo preenchido) retorna 0
    return 0;
}

long pegarProximoNoOffset(FILE *arqIndex)
{
    fseek(arqIndex, 0, SEEK_END);
    return ftell(arqIndex);
}

int buscarRegistroBloco(int id, bloco_t *bloco, registro_t *registro)
{

    for (int i = 0; i < BLOCK_FACTOR; i++)
    {
        // Retorna 0 caso ache um registro que tenha id = -1 ou 0
        // Caso encontremos um registro desse, significa que todos os registros posteriores também não foram inicializados
        if (bloco->registros[i].id == -1 || bloco->registros[i].id == 0)
        {
            return 0;
        }

        // Retorna 1 caso o id do registro do indice em questão for igual a chave (id) passada por parametro
        if (bloco->registros[i].id == id)
        {
            memcpy(registro, &bloco->registros[i], sizeof(registro_t));
            return 1;
        }
    }

    // Retorna 0 caso passe por todos os registro e nenhum deles tenha id igual a chave (id)
    return 0;
}
