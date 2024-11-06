/*
 * Autores: Ayumi, Erllison, Maria Luiza
 * Descrição: Este programa pesquisa um registro através do arquivo de índice primário (B+Tree)
 * com base no ID fornecido, retornando os campos do registro e informações sobre os blocos lidos.
 */

#ifndef _DATABASE_H_
#include "database.h"
#define _DATABASE_H_
#endif
#include "primary_index.h"

/**
 * Função que busca um registro pelo índice primário (B+Tree) baseado no ID informado.
 *
 * Implementação da busca usando o índice primário (B+Tree).
 *
 * @param ID Identificador do registro a ser buscado
 */

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        cout << "*** Informe um ID para buscar.\n";
        return 1;
    }

    int id = atoi(argv[1]), qtdBlocosLidos, qtdBlocosTotal;
    FILE *arqDadosTxt, *arqIndicePrimario;
    registro_t registro;

    arqDadosTxt = fopen("hash_table.dat", "rb");
    arqIndicePrimario = fopen("primary_index.txt", "rb");
    if (arqDadosTxt == NULL || arqIndicePrimario == NULL)
    {
        cout << "*** Arquivos de dados ou índices inexistentes. Execute o código de upload.\n";
        return 1;
    }

    qtdBlocosLidos = buscaRegistroIndicePrimario(id, &registro, arqIndicePrimario, arqDadosTxt);
    if (registro.id == -1)
    {
        cout << "*** Registro não encontrado\n";
        return 1;
    }
    fseek(arqIndicePrimario, 0, SEEK_END);
    qtdBlocosTotal = ftell(arqIndicePrimario) / BLOCK_SIZE;

    cout << "Quantidade Total de Blocos: " << qtdBlocosTotal << "\n";
    cout << "Quantidade de Blocos Lidos: " << qtdBlocosLidos << "\n";
    cout << "---------------------------------\n";

    // cout << "ID: " << registro.id << "\n";
    // cout << "Título: " << registro.titulo << "\n";
    // cout << "Ano: " << registro.ano << "\n";
    // cout << "Autores: " << registro.autores << "\n";
    // cout << "Citacoes: " << registro.citacoes << "\n";
    // cout << "Atualizacoes: " << registro.atualizacoes << "\n";
    // cout << "Snipet: " << registro.snipet << "\n";

    fclose(arqDadosTxt);
    fclose(arqIndicePrimario);

    return 0;
}
