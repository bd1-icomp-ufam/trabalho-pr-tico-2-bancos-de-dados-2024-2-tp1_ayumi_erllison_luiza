#ifndef _DATABASE_H_
#include "database.h"
#define _DATABASE_H_
#endif
#include "secondary_index.h"

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        cout << "*** Informe um titulo para buscar.\n";
        return 1;
    }

    int qtdBlocosLidos, qtdBlocosTotal;
    chave_t titulo;
    strcpy(titulo, argv[1]);

    FILE *arqDadosTxt, *arqIndiceSecundario;
    registro_t registro;

    arqDadosTxt = fopen("hash_table.dat", "rb");
    arqIndiceSecundario = fopen("secondary_index.txt", "rb");
    if (arqDadosTxt == NULL || arqIndiceSecundario == NULL)
    {
        cout << "*** Arquivos de dados ou índices inexistentes. Execute o código de upload.\n";
        return 1;
    }

    qtdBlocosLidos = buscaRegistroIndiceSecundario(titulo, &registro, arqIndiceSecundario, arqDadosTxt);
    if (registro.id == -1)
    {
        cout << "*** Registro não encontrado.\n";
        return 1;
    }
    fseek(arqIndiceSecundario, 0, SEEK_END);
    qtdBlocosTotal = ftell(arqIndiceSecundario) / BLOCK_SIZE;

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
    fclose(arqIndiceSecundario);

    return 0;
}
