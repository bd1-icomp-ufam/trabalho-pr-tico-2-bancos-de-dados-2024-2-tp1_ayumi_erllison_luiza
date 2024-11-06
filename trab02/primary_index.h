#ifndef _DATABASE_H_
#include "database.h"
#define _DATABASE_H_
#endif

typedef struct
{
    int chaves[2 * M_PRIM + 1];
    long enderecos[2 * M_PRIM + 2];
    bool ehFolha = true;
    short numChaves = 0;
    int enderecoNo = 0;
    char preenchimento[8];
} no_index_primario_t;

// Inicia o arquivo de index primario
void inicializarIndicePrimario(FILE *arqIndex);

// Cria nó do arquivo de index primario na memória
void criarNoPrimario(bool folha, no_index_primario_t *novo);

// Escreve nó do arquivo de index primario da memória para o disco
void escreveNoDiscoPrimario(no_index_primario_t *no, long endereco, FILE *arqIndex);

// Lê um nó  do arquivo de index primario do disco para a memória
int lerNoDiscoPrimario(no_index_primario_t *outputNode, unsigned long endereco, FILE *arqIndex);

// Inserção master de uma chave no arquivo de index primario
void inserirIndexPrimario(int chave, unsigned long enderecoDados, FILE *arqIndex);

// Caminha pela árvore até encontrar o nó onde deve ser inserido a nova chave
void inserirRecursivoPrimario(no_index_primario_t *no, no_index_primario_t *pai, int chave, int dataOffset, FILE *arqIndex);

// Faz a divisão dos nós conforme as propriedades da árvore
void dividirNoPrimario(no_index_primario_t *no, no_index_primario_t *pai, int dataOffset, FILE *arqIndex);

// Faz o deslocamento das chaves de um nó para a inserção de uma nova chave
void deslocarChavesPrimario(no_index_primario_t *no, int pos);

// Busca o índice da primeira maior chave depois da chave passada como parâmetro
int buscarChaveMaiorPrimario(no_index_primario_t *no, int chave);

// Busca binaria padrão, retornamos o indice
int buscaBinariaPrimario(int chave, int *vetor, int tam);

// Busca master de uma chave no arquivo de index primario
int buscaRegistroIndicePrimario(int chave, registro_t *registro, FILE *arqIndex, FILE *arqDados);

// Caminha pela árvore até encontrar o nó onde pode estar a chave buscada, em seguida chama a função buscar o registro nesse nó
int buscaIndicePrimarioRecursivo(no_index_primario_t *raiz, int chave, registro_t *registro, FILE *arqIndex, FILE *arqDados);

void escreveNoDiscoPrimario(no_index_primario_t *no, long endereco, FILE *arqIndex)
{

    // Pula para o endereco do bloco no arquivo de index primario e escreve o nó
    fseek(arqIndex, endereco, SEEK_SET);
    fwrite(no, sizeof(no_index_primario_t), 1, arqIndex);
}

int lerNoDiscoPrimario(no_index_primario_t *outputNode, unsigned long endereco, FILE *arqIndex)
{

    // Pula para o endereco do bloco no arquivo de index primario e retorna o nó
    fseek(arqIndex, endereco, SEEK_SET);
    return fread(outputNode, sizeof(no_index_primario_t), 1, arqIndex);
}

void inicializarIndicePrimario(FILE *arqIndex)
{

    // Cria um nó, define seu endereço como zero e escreve no index primario
    // Na nossa implementação, toda raiz fica no endereço 0
    no_index_primario_t raiz;
    criarNoPrimario(true, &raiz);
    raiz.enderecoNo = 0;
    escreveNoDiscoPrimario(&raiz, 0, arqIndex);
}

void deslocarChavesPrimario(no_index_primario_t *no, int pos)
{

    // Inicializa a variavel j com a quantidade de chaves no nó
    int j = no->numChaves;
    while (j > pos)
    {
        // Desloca as ultimas chaves do vetor ate a pos passada por parametro
        no->chaves[j] = no->chaves[j - 1];
        no->enderecos[j + 1] = no->enderecos[j];
        j--;
    }
    no->enderecos[j + 1] = no->enderecos[j];
}

void criarNoPrimario(bool folha, no_index_primario_t *novo)
{

    // Inicializa os endereços e chaves do novo nó
    for (int i = 0; i <= 2 * M_PRIM + 1; i++)
    {
        novo->chaves[i] = -1;
        novo->enderecos[i] = -1;
    }

    // Inicializa a posicao extra nas nos enderecos, a qtd de chaves e a flag "ehFolha"
    novo->enderecos[2 * M_PRIM + 2] = -1;
    novo->numChaves = 0;
    novo->ehFolha = folha;
}

void inserirIndexPrimario(int chave, unsigned long enderecoDados, FILE *arqIndex)
{

    // Cria uma variavel do tipo nó, lê no arquivo de indices e chama a função recursiva de inserção
    no_index_primario_t raiz;
    lerNoDiscoPrimario(&raiz, 0, arqIndex);
    inserirRecursivoPrimario(&raiz, NULL, chave, enderecoDados, arqIndex);
}

void inserirRecursivoPrimario(no_index_primario_t *no, no_index_primario_t *pai, int chave, int dataOffset, FILE *arqIndex)
{

    // Busca a posicao onde a nova chave deve ser inserida
    int pos = buscarChaveMaiorPrimario(no, chave);

    // Caso o nó seja folha, podemos fazer a inserção
    if (no->ehFolha)
    {

        // Deslocamos o vetor ate a posicao onde devemos inserir a nova chave
        deslocarChavesPrimario(no, pos);

        // Copiamos a chave para o indice, add+1 na qtd de chaves, add o endereco e reescreve o nó no index
        no->chaves[pos] = chave;
        no->numChaves++;
        no->enderecos[pos] = dataOffset;
        escreveNoDiscoPrimario(no, no->enderecoNo, arqIndex);
    }
    else
    {
        // Caso o nó não seja folha, devemos procurar a chave pelo próximo nó
        // Puxa o proximo nó no arquivo de index e chama a função novamente para ele
        no_index_primario_t nextNode;
        if (lerNoDiscoPrimario(&nextNode, no->enderecos[pos], arqIndex))
        {
            inserirRecursivoPrimario(&nextNode, no, chave, dataOffset, arqIndex);
        }
    }

    // Verifica se é necessário dividir o nó após a inserção
    if (no->numChaves > 2 * M_PRIM)
    {
        dividirNoPrimario(no, pai, dataOffset, arqIndex);
    }
}

void dividirNoPrimario(no_index_primario_t *no, no_index_primario_t *pai, int dataOffset, FILE *arqIndex)
{
    int i, j;
    int meio = no->numChaves / 2;
    no_index_primario_t filhoEsquerda;

    // Caso o pai seja null, ou seja, o nó é a própria raiz, há a criação de um novo pai
    if (pai == NULL)
    {

        // Cria um novo nó, copia as chaves e endereços do nó para o novo e zera as chaves e endereços do nó atual
        criarNoPrimario(no->ehFolha, &filhoEsquerda);

        for (int i = 0; i < 2 * M_PRIM + 1; i++)
        {
            filhoEsquerda.chaves[i] = no->chaves[i];
            filhoEsquerda.enderecos[i] = no->enderecos[i];
            no->chaves[i] = -1;
            no->enderecos[i] = -1;
        }

        // Copia o endereco extra do nó para o filho da esquerda e zera o endereço extra do no atual
        filhoEsquerda.enderecos[2 * M_PRIM + 1] = no->enderecos[2 * M_PRIM + 1];
        no->enderecos[2 * M_PRIM + 1] = -1;

        // Copiar a quantidade de chaves, define o endereço do novo nó e faz o endereço do nó no índice 0 receber o endereço do novo nó
        filhoEsquerda.numChaves = no->numChaves;
        filhoEsquerda.enderecoNo = pegarProximoNoOffset(arqIndex);
        no->enderecos[0] = filhoEsquerda.enderecoNo;

        // Define o no como novo pai
        pai = no;
        pai->ehFolha = false;
        pai->numChaves = 0;
        no = &filhoEsquerda;

        // Escrever os novos nós no arquivo de index primario
        escreveNoDiscoPrimario(pai, pai->enderecoNo, arqIndex);
        escreveNoDiscoPrimario(no, no->enderecoNo, arqIndex);
    }

    // Caso o nó tenha pai
    no_index_primario_t filhoDireita;
    criarNoPrimario(no->ehFolha, &filhoDireita);
    filhoDireita.enderecoNo = pegarProximoNoOffset(arqIndex);
    escreveNoDiscoPrimario(&filhoDireita, filhoDireita.enderecoNo, arqIndex);

    // Adiciona as chaves do nó atual no nó da direita
    for (i = meio + 1, j = 0; i < no->numChaves; i++, j++)
    {
        filhoDireita.chaves[j] = no->chaves[i];
        filhoDireita.enderecos[j] = no->enderecos[i];
    }
    filhoDireita.enderecos[j] = no->enderecos[i];

    // Nó da esquerda
    for (int i = meio + 1; i <= no->numChaves; i++)
    {

        // Aqui sera feita a alteração para transformar em Arvore B+
        if (i == 2 * M_PRIM && no->ehFolha)
        {
            no->enderecos[i] = filhoDireita.enderecoNo;
        }
        else
        {
            // Coloca dados
            no->enderecos[i] = -1;
        }
    }

    // Adicionar a chave promovida no nó pai
    int promovida = no->chaves[meio];
    int pos = buscarChaveMaiorPrimario(pai, promovida);
    deslocarChavesPrimario(pai, pos);
    pai->chaves[pos] = promovida;
    pai->enderecos[pos + 1] = filhoDireita.enderecoNo;

    // Atualizar a quantidade de chaves de cada nó
    filhoDireita.numChaves = no->numChaves - meio - 1;
    no->numChaves = meio + 1;
    pai->numChaves++;

    // Escrever os nós no arquivo de index primario
    escreveNoDiscoPrimario(no, no->enderecoNo, arqIndex);
    escreveNoDiscoPrimario(&filhoDireita, filhoDireita.enderecoNo, arqIndex);
    escreveNoDiscoPrimario(pai, pai->enderecoNo, arqIndex);
}

int buscaBinariaPrimario(int chave, int *vetor, int tam)
{
    int in = 0, fim = tam - 1, meio;

    while (in <= fim)
    {
        meio = (in + fim) / 2;
        if (chave < vetor[meio])
        {
            fim = meio - 1;
        }
        else if (chave > vetor[meio])
        {
            in = meio + 1;
        }
        else
        {
            return meio;
        }
    }
    return meio;
}

int buscarChaveMaiorPrimario(no_index_primario_t *no, int chave)
{
    int pos = 0;
    for (; pos < no->numChaves && no->chaves[pos] < chave; pos++)
        ;
    return pos;
}

int buscaRegistroIndicePrimario(int chave, registro_t *registro, FILE *arqIndex, FILE *arqDados)
{

    // Lê o nó (raiz) do arq de index e chama a função recursiva de busca
    no_index_primario_t raiz;
    lerNoDiscoPrimario(&raiz, 0, arqIndex);
    return 1 + buscaIndicePrimarioRecursivo(&raiz, chave, registro, arqIndex, arqDados);
}

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

int buscaIndicePrimarioRecursivo(no_index_primario_t *raiz, int chave, registro_t *registro, FILE *arqIndex, FILE *arqDados)
{
    int indiceChaveMaioOuIgual = buscaBinariaPrimario(chave, raiz->chaves, raiz->numChaves);
    if (raiz->ehFolha)
    {

        // Chave não encontrada
        if (raiz->chaves[indiceChaveMaioOuIgual] != chave)
        {
            return 0;
        }
        char block[4096];
        int posBloco = 0;
        TableRow entrada;

        // Carrega o bloco para a memória e busca o registro no bloco
        // bloco_t bloco;
        fseek(arqDados, raiz->enderecos[indiceChaveMaioOuIgual], SEEK_SET);

        // fseek(arqDados, sizeof(bloco_t) * raiz->enderecos[indiceChaveMaioOuIgual], SEEK_SET);
        printf("Endereco: %ld\n", raiz->enderecos[indiceChaveMaioOuIgual]);
        printf("Endereco: %ld\n", sizeof(TableRow));
        fread(&block, 4096, 1, arqDados);
        lerRegistroDoBloco(block, posBloco, entrada);
        printTableRow(entrada);
        registro->id = entrada.id;
        // buscarRegistroBloco(chave, &bloco, registro);
        return 0;
    }
    else
    {
        // Caso contrário (o nó atual não é folha)
        // Chama a função recursiva pra subarvore onde a chave pode estar
        if (chave <= raiz->chaves[indiceChaveMaioOuIgual])
        {
            lerNoDiscoPrimario(raiz, raiz->enderecos[indiceChaveMaioOuIgual], arqIndex);
        }
        else
        {
            lerNoDiscoPrimario(raiz, raiz->enderecos[indiceChaveMaioOuIgual + 1], arqIndex);
        }
        return 1 + buscaIndicePrimarioRecursivo(raiz, chave, registro, arqIndex, arqDados);
    }
}