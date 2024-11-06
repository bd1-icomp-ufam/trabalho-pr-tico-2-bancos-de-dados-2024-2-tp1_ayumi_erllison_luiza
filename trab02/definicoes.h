// Tamanho de um bloco
#define BLOCK_SIZE 4096

// Tamanho de um registro da base de dados
#define REGISTER_SIZE 592

// Quantidade de blocos
#define QTD_BLOCKS 170241

// Quantidade de registros por bloco
#define BLOCK_FACTOR 6

// Quantidade de blocos em um bucket
#define BLOCKS_BY_BUCKETS 4

// Numero primo usada na função hash
#define PRIMO 120767

// Tamanho de um bucket
#define BUCKET_SIZE (BLOCKS_BY_BUCKETS * BLOCK_SIZE)

// Ordem do index primario
#define M_PRIM 169

// Ordem do index secundario
#define M_SEC 6
