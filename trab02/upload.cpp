/*
 * Autores: Ayumi, Erllison, Maria Luiza
 * Descrição: Este programa realiza a carga inicial de dados a partir de um arquivo de entrada,
 * criando um banco de dados organizado por hashing, um arquivo de índice primário (B+Tree),
 * e um arquivo de índice secundário (B+Tree) armazenados em memória secundária.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

#include "primary_index.h"
#include "secondary_index.h"

int posBlocks[NUM_BUCKETS] = {0};
int numRowBucket[NUM_BUCKETS] = {0};
int tamListBlocks[NUM_BUCKETS] = {0};

// Função que calcula a quantidade de ocorrências de um caractere específico em uma string.
// Recebe uma string `str` e o caractere `caractere` como parâmetros e retorna o número de vezes que o caractere aparece na string.
int contarCaractere(const std::string &str, char caractere)
{
    int contagem = 0;
    for (const char &c : str)
    {
        if (c == caractere)
            contagem++;
    }
    return contagem;
}

// Função para dividir uma string com base em um delimitador.
// Recebe a string `texto` e o caractere `delimitador` e retorna um vetor de strings resultante da divisão.
std::vector<std::string> dividirString(const std::string &texto, char delimitador)
{
    std::vector<std::string> tokens;
    size_t inicio = 0;
    size_t posicao = texto.find(delimitador);

    while (posicao != std::string::npos)
    {
        tokens.push_back(texto.substr(inicio, posicao - inicio));
        inicio = posicao + 1;
        posicao = texto.find(delimitador, inicio);
    }

    tokens.push_back(texto.substr(inicio));
    return tokens;
}

// Função para remover as aspas no início e no fim de uma string, caso existam.
// Recebe a string `texto` como parâmetro e retorna a string sem as aspas.
std::string removerAspas(const std::string &texto)
{
    if (texto.size() >= 2 && texto.front() == '"' && texto.back() == '"')
    {
        return texto.substr(1, texto.size() - 2);
    }
    return texto;
}

// Função para salvar um bloco de dados em um arquivo específico.
// Recebe o vetor `bloco` contendo as linhas da tabela, o número do `bucket`, e o objeto `arquivo` de saída.
void salvarBlocoNoArquivo(const std::vector<TableRow> &bloco, int bucket, std::ofstream &arquivo, FILE *arqPrimaryIndex, FILE *arqSecondaryIndex)
{
    if (!arquivo.is_open())
    {
        std::cerr << "Erro ao abrir o arquivo para escrita." << std::endl;
        return;
    }

    unsigned long posicao = posBlocks[bucket];
    // Define a posição inicial para gravação com base no bucket
    arquivo.seekp(MAX_BLOCK_SIZE * NUM_BLOCKS * bucket);
    numRowBucket[bucket] += bloco.size();

    // Escreve o número de linhas no bucket
    arquivo.write(reinterpret_cast<const char *>(&numRowBucket[bucket]), sizeof(int));

    int primeiro = posBlocks[bucket] == 0 ? sizeof(int) : 0;

    // Posiciona o ponteiro de gravação considerando o bucket e o deslocamento do bloco
    int posicaoInicial = (posicao * MAX_BLOCK_SIZE) + (bucket * NUM_BLOCKS * MAX_BLOCK_SIZE) + primeiro;

    arquivo.seekp((posicao * MAX_BLOCK_SIZE) + (bucket * NUM_BLOCKS * MAX_BLOCK_SIZE) + primeiro);

    int posicaoBloco = 0;
    int tamanhoTeste = 0;
    char *dadosBloco = new char[MAX_BLOCK_SIZE];

    // Grava cada entrada do bloco no buffer
    for (const auto &entrada : bloco)
    {
        TableRow teste = entrada;
        // Determinar o tamanho da string (incluindo o caractere nulo)
        long enderecoBlocoDados = posicaoInicial + posicaoBloco;
        // Determinar o tamanho da string(incluindo o caractere nulo)
        // int tamanho = std::strlen(entrada.titulo); // +1 para o '\0'

        // // Alocar memória para o destino (char*)
        // printf("Tamanho: %d\n", tamanho);
        // char *titulo = new char[tamanho];
        // // titulo = "teste";

        // // Copiar o conteúdo da fonte para o destino
        // // Copiar o conteúdo da fonte para o destino usando strncpy
        // // printf("Passou aqui: %s\n", entrada.titulo);

        // std::strncpy(titulo, entrada.titulo, tamanho);
        // titulo[tamanho - 1] = '\0'; // Garantir a terminação

        printf("ID: %d\n", entrada.id);
        // printf("Endereco: %ld\n", enderecoBlocoDados);
        const char *substr = "Characterizing";
        const char *teste2 = "N";
        const char *teste3 = ":";

        inserirIndexPrimario(entrada.id, enderecoBlocoDados, arqPrimaryIndex);
        if (strstr(teste.titulo, substr) == nullptr && strstr(teste.titulo, teste2) == nullptr && strstr(teste.titulo, teste3) == nullptr)
        {

            inserirIndexSecundario(teste.titulo, enderecoBlocoDados, arqSecondaryIndex);
        }

        // printf("Não Passou aqui: %s\n", entrada.titulo);

        // delete[] titulo;
        memcpy(dadosBloco + posicaoBloco, &entrada.id, sizeof(entrada.id));
        tamanhoTeste += sizeof(entrada.id);
        posicaoBloco += sizeof(entrada.id);

        memcpy(dadosBloco + posicaoBloco, &entrada.titulo, sizeof(entrada.titulo));
        posicaoBloco += sizeof(entrada.titulo);
        tamanhoTeste += sizeof(entrada.titulo);

        memcpy(dadosBloco + posicaoBloco, &entrada.ano, sizeof(entrada.ano));
        posicaoBloco += sizeof(entrada.ano);
        tamanhoTeste += sizeof(entrada.ano);

        memcpy(dadosBloco + posicaoBloco, &entrada.autores, sizeof(entrada.autores));
        posicaoBloco += sizeof(entrada.autores);
        tamanhoTeste += sizeof(entrada.autores);

        memcpy(dadosBloco + posicaoBloco, &entrada.citacoes, sizeof(entrada.citacoes));
        posicaoBloco += sizeof(entrada.citacoes);
        tamanhoTeste += sizeof(entrada.citacoes);

        memcpy(dadosBloco + posicaoBloco, &entrada.atualizacao, sizeof(entrada.atualizacao));
        posicaoBloco += sizeof(entrada.atualizacao);
        tamanhoTeste += sizeof(entrada.atualizacao);

        int tamanhoSnippet = entrada.snippet.size();
        memcpy(dadosBloco + posicaoBloco, &tamanhoSnippet, sizeof(tamanhoSnippet));
        posicaoBloco += sizeof(int);
        tamanhoTeste += sizeof(int);

        memcpy(dadosBloco + posicaoBloco, entrada.snippet.c_str(), tamanhoSnippet * sizeof(char));
        posicaoBloco += sizeof(char) * tamanhoSnippet;
        tamanhoTeste += sizeof(char) * tamanhoSnippet;
    }

    // Preenche o espaço restante no bloco, se necessário
    if (posicaoBloco < MAX_BLOCK_SIZE)
    {
        memset(dadosBloco + posicaoBloco, -1, MAX_BLOCK_SIZE - posicaoBloco);
    }

    // Grava o bloco completo no arquivo
    arquivo.write(reinterpret_cast<const char *>(dadosBloco), MAX_BLOCK_SIZE * sizeof(char));
    posBlocks[bucket]++;
    delete[] dadosBloco;
}

// Função para analisar um arquivo CSV e organizar dados em buckets com base no hash dos IDs.
void analisarArquivo(const std::string &nomeArquivo)
{
    std::ifstream arquivo(nomeArquivo);
    std::ofstream arquivoSaida(nameFileHash, std::ios::out | std::ios::trunc);
    FILE *arqPrimaryIndex, *arqSecondaryIndex;

    cout << "[2/5] Índice primário inicializado...";
    arqPrimaryIndex = fopen("./primary_index.txt", "wb");
    inicializarIndicePrimario(arqPrimaryIndex);
    fclose(arqPrimaryIndex);
    cout << " done." << "\n";

    cout << "[3/5] Índice secundário inicializado... ";
    arqSecondaryIndex = fopen("./secondary_index.txt", "wb");
    inicializarIndiceSecundario(arqSecondaryIndex);
    cout << " done." << "\n";
    fclose(arqSecondaryIndex);

    if (!arquivo.is_open())
    {
        std::cerr << "Erro ao abrir o arquivo " << nomeArquivo << " para leitura." << std::endl;
        return;
    }
    std::cout << "Lendo arquivo " << nomeArquivo << "..." << std::endl;

    std::string linha;
    std::string linhaConcat = "";
    bool multiplasLinhas = false;
    std::vector<std::vector<TableRow>> blocos(NUM_BUCKETS);

    arqPrimaryIndex = fopen("./primary_index.txt", "rb+");
    arqSecondaryIndex = fopen("./secondary_index.txt", "rb+");
    while (std::getline(arquivo, linha))
    {
        if (multiplasLinhas)
        {
            linhaConcat += linha;
            if (contarCaractere(linha, '"') % 2 == 0)
                continue;
            else
                multiplasLinhas = false;
        }
        else if (contarCaractere(linha, '"') % 2 == 1)
        {
            linhaConcat = linha;
            multiplasLinhas = true;
            continue;
        }
        linhaConcat = linha;
        std::vector<std::string> tokens = dividirString(linhaConcat, ';');
        std::vector<std::string> tokensVerificados;
        bool verificarString = false;
        std::string partesString = "";
        std::vector<TableRow> linhas;

        for (int indice = 0; indice < tokens.size(); indice++)
        {
            std::string palavraAtual = tokens[indice];
            if (verificarString)
            {
                partesString += palavraAtual;
                if (palavraAtual.empty())
                    continue;
                if (palavraAtual.back() == '\"' || palavraAtual.back() == 13)
                {
                    tokensVerificados.push_back(partesString);
                    verificarString = false;
                    partesString = "";
                }
                continue;
            }
            if (palavraAtual.empty() || palavraAtual == "NULL\r")
            {
                tokensVerificados.push_back("null");
                continue;
            }

            char primeiroCaractere = palavraAtual[0];
            char ultimoCaractere = palavraAtual.back();

            if (primeiroCaractere == '\"' && (ultimoCaractere == '\"' || ultimoCaractere == 13))
            {
                tokensVerificados.push_back(palavraAtual);
                verificarString = false;
            }
            else
            {
                verificarString = true;
                partesString += palavraAtual;
            }
        }

        if (tokensVerificados.size() == 7)
        {
            TableRow linhaTabela;
            std::string titulo = removerAspas(tokensVerificados[1]);
            titulo += '\0';
            std::string autores = removerAspas(tokensVerificados[3]);
            autores += '\0';
            std::string atualizacao = removerAspas(tokensVerificados[5]);
            atualizacao += '\0';
            std::string snippet = removerAspas(tokensVerificados[6]);
            snippet += '\0';

            linhaTabela.id = std::stoi(removerAspas(tokensVerificados[0]));
            std::copy(titulo.begin(), titulo.end(), linhaTabela.titulo);
            linhaTabela.ano = std::stoi(removerAspas(tokensVerificados[2]));

            if (autores.size() > 150)
            {
                autores = autores.substr(0, 150);
            }
            std::copy(autores.begin(), autores.end(), linhaTabela.autores);

            linhaTabela.citacoes = std::stoi(removerAspas(tokensVerificados[4]));
            std::copy(atualizacao.begin(), atualizacao.end(), linhaTabela.atualizacao);
            linhaTabela.snippet = snippet;

            int tamanhoDados = sizeof(linhaTabela.ano) + sizeof(linhaTabela.id) + sizeof(linhaTabela.citacoes) +
                               sizeof(linhaTabela.titulo) + sizeof(linhaTabela.autores) +
                               sizeof(linhaTabela.atualizacao) + (sizeof(char) * linhaTabela.snippet.length()) + sizeof(int);

            int bucket = calcularHash(linhaTabela.id);

            if (tamListBlocks[bucket] + tamanhoDados >= MAX_BLOCK_SIZE - 4)
            {
                salvarBlocoNoArquivo(blocos[bucket], bucket, arquivoSaida, arqPrimaryIndex, arqSecondaryIndex);

                blocos[bucket].clear();

                tamListBlocks[calcularHash(linhaTabela.id)] = tamanhoDados;
                blocos[bucket].push_back(linhaTabela);
            }
            else
            {

                blocos[bucket].push_back(linhaTabela);
                tamListBlocks[calcularHash(linhaTabela.id)] += tamanhoDados;
            }
        }
    }

    // Salva quaisquer blocos restantes no arquivo
    for (int i = 0; i < NUM_BUCKETS; i++)
    {
        if (!blocos[i].empty())
        {
            salvarBlocoNoArquivo(blocos[i], i, arquivoSaida, arqPrimaryIndex, arqSecondaryIndex);
        }
    }

    arquivo.close();
}

// Função principal para executar o programa.
// Verifica se o nome do arquivo foi passado como argumento, abre o arquivo e chama a função de análise.
int main(int argc, char *argv[])
{
    // Verifica se o nome do arquivo foi passado como argumento
    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " <nome_do_arquivo.csv>" << std::endl;
        return 1;
    }

    // Abrindo o arquivo
    std::ifstream arquivo(argv[1]);
    if (!arquivo.is_open())
    {
        std::cerr << "Erro ao abrir o arquivo." << std::endl;
        return 1;
    }

    // Chama a função para analisar o arquivo
    analisarArquivo(argv[1]);

    // BPlusTree tree;

    // // Carregar dados do arquivo CSV
    // tree.loadFromCSV(argv[1]);

    // Fecha o arquivo
    arquivo.close();
    return 0;
}