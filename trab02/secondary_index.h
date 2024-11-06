#ifndef _DATABASE_H_
#include "database.h"
#define _DATABASE_H_
#endif

using namespace std;

typedef char chave_t[301];

typedef struct
{
    chave_t chaves[2 * M_SEC + 1];
    long enderecos[2 * M_SEC + 2];
    bool ehFolha = true;
    short numChaves = 0;
    int enderecoNo = 0;
    char preenchimento[56];
} no_index_secundario_t;

// Inicia o arquivo de index secundario
void inicializarIndiceSecundario(FILE *arqIndex);

// Cria nó do arquivo de index secundario na memória
void criarNoSecundario(bool folha, no_index_secundario_t *novo);

// Escreve nó do arquivo de index secundario da memória para o disco
void escreveNoDiscoSecundario(no_index_secundario_t *no, long endereco, FILE *arqIndex);

// Lê um nó  do arquivo de index secundario do disco para a memória
int lerNoDiscoSecundario(no_index_secundario_t *outputNode, unsigned long endereco, FILE *arqIndex);

// Inserção master de uma chave no arquivo de index secundario
void inserirIndexSecundario(chave_t chave, unsigned long enderecoDados, FILE *arqIndex);

// Caminha pela árvore até encontrar o nó onde deve ser inserido a nova chave
void inserirRecursivoSecundario(no_index_secundario_t *no, no_index_secundario_t *pai, chave_t chave, int dataOffset, FILE *arqIndex);

// Faz a divisão dos nós conforme as propriedades da árvore
void dividirNoSecundario(no_index_secundario_t *no, no_index_secundario_t *pai, int dataOffset, FILE *arqIndex);

// Faz o deslocamento das chaves de um nó para a inserção de uma nova chave
void deslocarChavesSecundario(no_index_secundario_t *no, int pos);

// Busca o índice da primeira maior chave depois da chave passada como parâmetro
int buscarChaveMaiorSecundario(no_index_secundario_t *no, chave_t chave);

// Busca binaria padrão, retornamos o indice
int buscaBinariaSecundario(chave_t chave, chave_t *vetor, int tam);

// Busca master de uma chave no arquivo de index secundario
int buscaRegistroIndiceSecundario(chave_t chave, registro_t *registro, FILE *arqIndex, FILE *arqDados);

// Caminha pela árvore até encontrar o nó onde pode estar a chave buscada, em seguida chama a função buscar o registro nesse nó
int buscaIndiceSecundarioRecursivo(no_index_secundario_t *raiz, chave_t chave, registro_t *registro, FILE *arqIndex, FILE *arqDados);

// Busca pelo registro no bloco do arquivo de dados
int buscarRegistroBlocoSecundario(chave_t titulo, bloco_t *bloco, registro_t *registro);

void escreveNoDiscoSecundario(no_index_secundario_t *no, long endereco, FILE *arqIndex)
{

    // Pula para o endereco do bloco no arquivo de index secundario e escreve o nó
    fseek(arqIndex, endereco, SEEK_SET);
    fwrite(no, sizeof(no_index_secundario_t), 1, arqIndex);
}

int lerNoDiscoSecundario(no_index_secundario_t *outputNode, unsigned long endereco, FILE *arqIndex)
{

    // Pula para o endereco do bloco no arquivo de index secundario e retorna o nó
    fseek(arqIndex, endereco, SEEK_SET);
    return fread(outputNode, sizeof(no_index_secundario_t), 1, arqIndex);
}

void inicializarIndiceSecundario(FILE *arqIndex)
{

    // Cria um nó, define seu endereço como zero e escreve no index secundario
    // Na nossa implementação, toda raiz fica no endereço 0
    no_index_secundario_t raiz;
    criarNoSecundario(true, &raiz);
    raiz.enderecoNo = 0;
    escreveNoDiscoSecundario(&raiz, 0, arqIndex);
}

void deslocarChavesSecundario(no_index_secundario_t *no, int pos)
{

    // Inicializa a variavel j com a quantidade de chaves no nó
    int j = no->numChaves;
    while (j > pos)
    {
        // Desloca as ultimas chaves do vetor ate a pos passada por parametro
        strcpy(no->chaves[j], no->chaves[j - 1]);
        no->enderecos[j + 1] = no->enderecos[j];
        j--;
    }
    no->enderecos[j + 1] = no->enderecos[j];
}

void criarNoSecundario(bool folha, no_index_secundario_t *novo)
{

    // Inicializa os endereços e chaves do novo nó
    for (int i = 0; i <= 2 * M_SEC + 1; i++)
    {
        strcpy(novo->chaves[i], "");
        novo->enderecos[i] = -1;
    }

    // Inicializa a posicao extra nas nos enderecos, a qtd de chaves e a flag "ehFolha"
    novo->enderecos[2 * M_SEC + 2] = -1;
    novo->numChaves = 0;
    novo->ehFolha = folha;
}

void inserirIndexSecundario(chave_t chave, unsigned long enderecoDados, FILE *arqIndex)
{

    // Cria uma variavel do tipo nó, lê no arquivo de indices e chama a função recursiva de inserção
    no_index_secundario_t raiz;
    lerNoDiscoSecundario(&raiz, 0, arqIndex);
    printf("Inserindo chave %s\n", chave);
    inserirRecursivoSecundario(&raiz, NULL, chave, enderecoDados, arqIndex);
}

void inserirRecursivoSecundario(no_index_secundario_t *no, no_index_secundario_t *pai, chave_t chave, int dataOffset, FILE *arqIndex)
{

    // Busca a posicao onde a nova chave deve ser inserida
    int pos = buscarChaveMaiorSecundario(no, chave);
    // Caso o nó seja folha, podemos fazer a inserção
    if (no->ehFolha)
    {

        // Deslocamos o vetor ate a posicao onde devemos inserir a nova chave
        deslocarChavesSecundario(no, pos);

        // Copiamos a chave para o indice, add+1 na qtd de chaves, add o endereco e reescreve o nó no index
        strcpy(no->chaves[pos], chave);
        no->numChaves++;
        no->enderecos[pos] = dataOffset;
        escreveNoDiscoSecundario(no, no->enderecoNo, arqIndex);
    }
    else
    {
        // Caso o nó não seja folha, devemos procurar a chave pelo próximo nó
        // Puxa o proximo nó no arquivo de index e chama a função novamente para ele
        no_index_secundario_t nextNode;
        if (lerNoDiscoSecundario(&nextNode, no->enderecos[pos], arqIndex))
        {
            inserirRecursivoSecundario(&nextNode, no, chave, dataOffset, arqIndex);
        }
    }

    // Verifica se é necessário dividir o nó após a inserção
    if (no->numChaves > 2 * M_SEC)
    {
        dividirNoSecundario(no, pai, dataOffset, arqIndex);
    }
}

void dividirNoSecundario(no_index_secundario_t *no, no_index_secundario_t *pai, int dataOffset, FILE *arqIndex)
{
    int i, j;
    int meio = no->numChaves / 2;
    no_index_secundario_t filhoEsquerda;

    // Caso o pai seja null, ou seja, o nó é a própria raiz, há a criação de um novo pai
    if (pai == NULL)
    {

        // Cria um novo nó, copia as chaves e endereços do nó para o novo e zera as chaves e endereços do nó atual
        criarNoSecundario(no->ehFolha, &filhoEsquerda);
        for (int i = 0; i < 2 * M_SEC + 1; i++)
        {
            strcpy(filhoEsquerda.chaves[i], no->chaves[i]);
            filhoEsquerda.enderecos[i] = no->enderecos[i];
            strcpy(no->chaves[i], "");
            no->enderecos[i] = -1;
        }

        // Copia o endereco extra do nó para o filho da esquerda e zera o endereço extra do no atual
        filhoEsquerda.enderecos[2 * M_SEC + 1] = no->enderecos[2 * M_SEC + 1];
        no->enderecos[2 * M_SEC + 1] = -1;

        // Copiar a quantidade de chaves, define o endereço do novo nó e faz o endereço do nó no índice 0 receber o endereço do novo nó
        filhoEsquerda.numChaves = no->numChaves;
        filhoEsquerda.enderecoNo = pegarProximoNoOffset(arqIndex);
        no->enderecos[0] = filhoEsquerda.enderecoNo;

        // Definir o no como novo pai
        pai = no;
        pai->ehFolha = false;
        pai->numChaves = 0;
        no = &filhoEsquerda;

        // Escrever os novos nós no arquivo de index secundario
        escreveNoDiscoSecundario(pai, pai->enderecoNo, arqIndex);
        escreveNoDiscoSecundario(no, no->enderecoNo, arqIndex);
    }

    // Caso o nó tenha pai
    no_index_secundario_t filhoDireita;
    criarNoSecundario(no->ehFolha, &filhoDireita);
    filhoDireita.enderecoNo = pegarProximoNoOffset(arqIndex);
    escreveNoDiscoSecundario(&filhoDireita, filhoDireita.enderecoNo, arqIndex);

    // Adiciona as chaves do nó atual no nó da direita
    for (i = meio + 1, j = 0; i < no->numChaves; i++, j++)
    {
        strcpy(filhoDireita.chaves[j], no->chaves[i]);
        filhoDireita.enderecos[j] = no->enderecos[i];
    }
    filhoDireita.enderecos[j] = no->enderecos[i];

    // Nó da esquerda
    for (int i = meio + 1; i <= no->numChaves; i++)
    {

        // Aqui sera feita a alteração para transformar em Arvore B+
        if (i == 2 * M_SEC && no->ehFolha)
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
    chave_t promovida;
    strcpy(promovida, no->chaves[meio]);
    int pos = buscarChaveMaiorSecundario(pai, promovida);
    deslocarChavesSecundario(pai, pos);
    strcpy(pai->chaves[pos], promovida);
    pai->enderecos[pos + 1] = filhoDireita.enderecoNo;

    // Atualizar a quantidade de chaves de cada nó
    filhoDireita.numChaves = no->numChaves - meio - 1;
    no->numChaves = meio + 1;
    pai->numChaves++;

    // Escrever os nós no arquivo de index secundario
    escreveNoDiscoSecundario(no, no->enderecoNo, arqIndex);
    escreveNoDiscoSecundario(&filhoDireita, filhoDireita.enderecoNo, arqIndex);
    escreveNoDiscoSecundario(pai, pai->enderecoNo, arqIndex);
}

int buscaBinariaSecundario(chave_t chave, chave_t *vetor, int tam)
{
    int in = 0, fim = tam - 1, meio;

    while (in <= fim)
    {
        meio = (in + fim) / 2;
        if (strcmp(chave, vetor[meio]) < 0)
        {
            fim = meio - 1;
        }
        else if (strcmp(chave, vetor[meio]) > 0)
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

int buscarChaveMaiorSecundario(no_index_secundario_t *no, chave_t chave)
{
    int pos = 0;
    for (; pos < no->numChaves && strcmp(chave, no->chaves[pos]) > 0; pos++)
        ;
    return pos;
}

int buscaRegistroIndiceSecundario(chave_t chave, registro_t *registro, FILE *arqIndex, FILE *arqDados)
{

    // Lê o nó (raiz) do arq de index e chama a função recursiva de busca
    no_index_secundario_t raiz;
    lerNoDiscoSecundario(&raiz, 0, arqIndex);
    return 1 + buscaIndiceSecundarioRecursivo(&raiz, chave, registro, arqIndex, arqDados);
}
bool lerRegistroDoBlocoSecundario(const char *block, int &posBloco, TableRow &entrada)
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

int buscaIndiceSecundarioRecursivo(no_index_secundario_t *raiz, chave_t chave, registro_t *registro, FILE *arqIndex, FILE *arqDados)
{
    int indiceChaveMaioOuIgual = buscaBinariaSecundario(chave, raiz->chaves, raiz->numChaves);

    if (raiz->ehFolha)
    {

        // Chave não encontrada
        if (strcmp(raiz->chaves[indiceChaveMaioOuIgual], chave) != 0)
        {
            return 0;
        }

        // Carrega o bloco para a memória e busca o registro no bloco
        char block[4096];
        int posBloco = 0;
        TableRow entrada;

        // Carrega o bloco para a memória e busca o registro no bloco
        fseek(arqDados, raiz->enderecos[indiceChaveMaioOuIgual], SEEK_SET);

        // fseek(arqDados, sizeof(bloco_t) * raiz->enderecos[indiceChaveMaioOuIgual], SEEK_SET);
        printf("Endereco: %ld\n", raiz->enderecos[indiceChaveMaioOuIgual]);
        printf("Endereco: %ld\n", sizeof(TableRow));
        fread(&block, 4096, 1, arqDados);
        lerRegistroDoBlocoSecundario(block, posBloco, entrada);
        printTableRow(entrada);
        registro->id = entrada.id;
        // bloco_t bloco;
        // fseek(arqDados, sizeof(bloco_t) * raiz->enderecos[indiceChaveMaioOuIgual], SEEK_SET);
        // fread(&bloco, sizeof(bloco_t), 1, arqDados);
        // buscarRegistroBlocoSecundario(chave, &bloco, registro);
        return 0;
    }
    else
    {
        // Caso contrário (o nó atual não é folha)
        // Chama a função recursiva pra subarvore onde a chave pode estar
        if (strcmp(chave, raiz->chaves[indiceChaveMaioOuIgual]) <= 0)
        {
            lerNoDiscoSecundario(raiz, raiz->enderecos[indiceChaveMaioOuIgual], arqIndex);
        }
        else
        {
            lerNoDiscoSecundario(raiz, raiz->enderecos[indiceChaveMaioOuIgual + 1], arqIndex);
        }
        return 1 + buscaIndiceSecundarioRecursivo(raiz, chave, registro, arqIndex, arqDados);
    }
}

int buscarRegistroBlocoSecundario(chave_t titulo, bloco_t *bloco, registro_t *registro)
{

    for (int i = 0; i < BLOCK_FACTOR; i++)
    {
        // Retorna 0 caso ache um registro que tenha titulo = ""
        // Caso encontremos um registro desse, significa que todos os registros posteriores também não foram inicializados
        if (strcmp(bloco->registros[i].titulo, "") == 0)
        {
            return 0;
        }

        // Retorna 1 caso o id do registro do indice em questão for igual a chave (id) passada por parametro
        if (strcmp(bloco->registros[i].titulo, titulo) == 0)
        {
            memcpy(registro, &bloco->registros[i], sizeof(registro_t));
            return 1;
        }
    }

    // Retorna 0 caso passe por todos os registro e nenhum deles tenha id igual a chave (id)
    return 0;
}