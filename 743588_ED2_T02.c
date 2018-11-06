/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
 *
 * RA: 		743588
 * Aluno:	Pietro Zuntini Bonfim 
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 			11
#define TAM_NOME 					51
#define TAM_MARCA 					51
#define TAM_DATA 					11
#define TAM_ANO 					3
#define TAM_PRECO 					8
#define TAM_DESCONTO 				4
#define TAM_CATEGORIA 				51
#define TAM_STRING_INDICE 			(TAM_MARCA + TAM_NOME)

#define TAM_REGISTRO 				192
#define MAX_REGISTROS 				1000
#define MAX_ORDEM 					150
#define TAM_ARQUIVO 				(MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!"
#define INICIO_BUSCA 				"********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM 			"********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 			"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO 				"********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO 		"***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO 	"***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO 					"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 						"FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP 			"Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS 			"Busca por %s.\nNos percorridos:\n"

/* Registro do Produto */
typedef struct produto {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA]; 	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Produto;

/* Estrutura da chave de um nó do Índice Primário */
typedef struct Chaveip {
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/* Estrutura da chave de um  do Índice Secundário */
typedef struct Chaveis {
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip {
	int num_chaves;  		/* numero de chaves armazenadas */
	Chave_ip *chave; 		/* vetor das chaves e rrns [m-1] */
	int *desc;		 		/* ponteiros para os descendentes, *desc[m] */
	char folha;		 		/* flag folha da arvore */
} node_Btree_ip;

typedef struct nodeis {
	int num_chaves;  		/* numero de chaves armazenadas */
	Chave_is *chave; 		/* vetor das chaves e rrns [m-1] */
	int *desc;		 		/* ponteiros para os descendentes, *desc[m] */
	char folha;		 		/* flag folha da arvore */
} node_Btree_is;

typedef struct indice {
	int raiz;
} Indice;

/* Variáveis globais */
char ARQUIVO[MAX_REGISTROS * TAM_REGISTRO + 1];
char ARQUIVO_IP[2000 * sizeof(Chave_ip)];
char ARQUIVO_IS[2000 * sizeof(Chave_is)];

/* Ordem das arvores */
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; 	/* Número de nós presentes no ARQUIVO_IP */
int nregistrosis; 	/* Número de nós presentes no ARQUIVO_IS */

/* Quantidade de bytes que ocupa cada nó da árvore nos arquivos de índice: */
int tamanho_registro_ip;
int tamanho_registro_is;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

// Recuperar registro em determinado RRN e devolver em uma struct Produto
Produto recuperar_registro(int rrn);

// Gera a chave primária
void gerar_chave(Produto *P);

// Exibe o produto
int exibir_registro(int rrn);

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número de registros. */
int carregar_arquivo();

/* (Re)faz o índice primário */
void criar_iprimary(Indice *iprimary);

/* (Re)faz o índice de marcas  */
void criar_ibrand(Indice *ibrand);

// Atualiza os dois índices com o novo registro inserido
void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Produto P);



/********************FUNÇÕES DO MENU*********************/
void cadastrar(Indice *iprimary, Indice *ibrand);

int alterar(Indice iprimary);

void buscar(Indice iprimary, Indice ibrand);

void listar(Indice iprimary, Indice ibrand);

/*******************************************************/


/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ============================= DAS ÁRVORES B ============================== 
 * ========================================================================== */

// Busca por um nó com determinada chave primária
void *b_tree_search(void *raiz, char *pk);


// Escreve um nó da árvore no arquivo de índice. O terceiro parametro serve para informar qual indice se trata
void write_btree(void *salvar, int rrn, char ip);
void write_btree_ip(node_Btree_ip *salvar, int rrn);
void write_btree_is(node_Btree_is *salvar, int rrn);

// Lê um nó do arquivo de índice e retorna na estrutura
void *read_btree(int rrn, char ip);
node_Btree_ip *read_btree_ip(int rrn);
node_Btree_is *read_btree_is(int rrn);

// Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
// É conveniente que essa função também inicialize os campos necessários com valores nulos
void *criar_no(char ip);
node_Btree_ip *criar_no_ip();
node_Btree_is *criar_no_is();

// Libera todos os campos dinâmicos do nó, inclusive ele mesmo
void libera_no(void *node, char ip);



/*
*   Caro aluno,
*   	caso não queira trabalhar com void*, é permitido dividir as funções que utilizam
*   	em duas, sendo uma para cada índice...
*   Um exemplo, a write_btree e read_btree ficariam como:
*
*   void write_btree_ip(node_Btree_ip *salvar, int rrn),  node_Btree_ip *read_btree_ip(int rrn),
*   void write_btree_is(node_Btree_is *salvar, int rrn) e node_Btree_is *read_btree_is(int rrn).
*/



int main()
{
	char *p; /* # */
			 /* Arquivo */
	int carregarArquivo = 0;
	nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d\n", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();


	/* Recebe as ordens dos índices PRIMÁRIO e SECUNDÁRIO */
	scanf("%d %d%*c", &ordem_ip, &ordem_is);


	tamanho_registro_ip = ordem_ip * 3 + 4 + (-1 + ordem_ip) * 14;
	tamanho_registro_is = ordem_is * 3 + 4 + (-1 + ordem_is) * (TAM_STRING_INDICE + 9);


	/* Índice primário */
	Indice iprimary;
	criar_iprimary(&iprimary);


	/* Índice secundário de nomes dos Produtos */
	Indice ibrand;
	criar_ibrand(&ibrand);


	/* Execução do programa */
	int opcao = 0;
	while (1)
	{
		scanf("%d%*c", &opcao);
		switch (opcao)
		{
		case 1: /* Cadastrar um novo Produto */
			cadastrar(&iprimary, &ibrand);
			break;

		case 2: /* Alterar o desconto de um Produto */
			printf(INICIO_ALTERACAO);
			if (alterar(iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		
		case 3: /* Buscar um Produto */
			printf(INICIO_BUSCA);
			buscar(iprimary, ibrand);
			break;
		
		case 4: /* Listar todos os Produtos */
			printf(INICIO_LISTAGEM);
			listar(iprimary, ibrand);
			break;
		
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO != '\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		
		case 6: /* Imprime o Arquivo de Índice Primário */
			printf(INICIO_INDICE_PRIMARIO);
			if (!*ARQUIVO_IP)
				puts(ARQUIVO_VAZIO);
			else
				for (p = ARQUIVO_IP; *p != '\0'; p += tamanho_registro_ip)
				{
					fwrite(p, 1, tamanho_registro_ip, stdout);
					puts("");
				}
			break;
		
		case 7: /* Imprime o Arquivo de Índice Secundário */
			printf(INICIO_INDICE_SECUNDARIO);
			if (!*ARQUIVO_IS)
				puts(ARQUIVO_VAZIO);
			else
				for (p = ARQUIVO_IS; *p != '\0'; p += tamanho_registro_is)
				{
					fwrite(p, 1, tamanho_registro_is, stdout);
					puts("");
				}
			break;
		
		case 8: /* Libera toda memória alocada dinâmicamente (se ainda houver) e encerra */
			return 0;
		
		default: /* exibe mensagem de erro */
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}

/* Gera a chave primária */
void gerar_chave(Produto *P) {
    P->pk[0] = '\0';                // Garante que os dados serão concatenados corretamente na chave primária
    strncat(P->pk, P->nome, 2);     // N1N2 
    strncat(P->pk, P->marca, 2);    // M1M2
 
    char *dAux;                     // DDMM
    char dataAux[11];               // Cria uma string dataAux para não perder o valor da data original com o strtok
    strcpy(dataAux, P->data);
    dAux = strtok(dataAux, "/");    // DD
    strncat(P->pk, dAux, 2);
    dAux = strtok(NULL, "/");       // MM
    strncat(P->pk, dAux, 2);
    
    strncat(P->pk, P->ano, 2);      // AL
}

/* Recupera o registro no ARQUIVO de dados e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn)
{
    char temp[193], *p;
    strncpy(temp, ARQUIVO + ((rrn)*192), 192);
    temp[192] = '\0';
    Produto j;

	// Recebe os dados da string temp retirada do ARQUIVO de dados em determinado RRN
	p = strtok(temp, "@");
	strcpy(j.pk, p);
	p = strtok(NULL, "@");
	strcpy(j.nome, p);
	p = strtok(NULL, "@");
	strcpy(j.marca, p);
	p = strtok(NULL, "@");
	strcpy(j.data, p);
	p = strtok(NULL, "@");
	strcpy(j.ano, p);
	p = strtok(NULL, "@");
	strcpy(j.preco, p);
	p = strtok(NULL, "@");
	strcpy(j.desconto, p);
	p = strtok(NULL, "@");
	strcpy(j.categoria, p);

    return j;
}


/* Exibe o Produto */
int exibir_registro(int rrn)
{
	if (rrn < 0)
		return 0;

	float preco;
	int desconto;
	Produto j = recuperar_registro(rrn);
	char *cat, categorias[TAM_CATEGORIA];

	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.marca);
	printf("%s\n", j.data);
	printf("%s\n", j.ano);
	sscanf(j.desconto, "%d", &desconto);
	sscanf(j.preco, "%f", &preco);
	preco = preco * (100 - desconto);
	preco = ((int)preco) / (float)100;
	printf("%07.2f\n", preco);
	strcpy(categorias, j.categoria);

	for (cat = strtok(categorias, "|"); cat != NULL; cat = strtok(NULL, "|"))
		printf("%s ", cat);
	printf("\n");

	return 1;
}


/* =====================================================================================
   ========================== CRIAÇÃO DOS INDICES ======================================
   ===================================================================================== */

void criar_iprimary(Indice *iprimary) { //todo
	iprimary->raiz = -1;
}

void criar_ibrand(Indice *ibrand) { //todo
	ibrand->raiz = -1;
}


/* =====================================================================================
   ======================== INTERAÇÃO COM O USUÁRIO ====================================
   ===================================================================================== */

/**** INSERÇÃO ****/ //todo

void ler_entrada(Produto *novo) {
    scanf("%[^\n]s", novo->nome);
    getchar();
    scanf("%[^\n]s", novo->marca);
    getchar();
    scanf("%[^\n]s", novo->data);
    getchar();
    scanf("%[^\n]s", novo->ano);
    getchar();
    scanf("%[^\n]s", novo->preco);
    getchar();
    scanf("%[^\n]s", novo->desconto);
    getchar();
    scanf("%[^\n]s", novo->categoria);
    getchar();
}
void cadastrar(Indice *iprimary, Indice *ibrand) {

	char entrada[193];
	Produto novo;


	// Lê a entrada do usuário e gera a chave primária
	ler_entrada(&novo);
	gerar_chave(&novo);

	// Coloca os dados na string entrada[]
	sprintf(entrada, "%s@%s@%s@%s@%s@%s@%s@%s@", novo.pk, novo.nome, novo.marca, novo.data, novo.ano, novo.preco, novo.desconto, novo.categoria);

	// Completa os espaços restantes com '#'
    int necessarios = 192 - strlen(entrada);
    for (int i = 0; i < necessarios; i++)
        strcat(entrada, "#");

	// Coloca a entrada no ARQUIVO de dados
	strcat(ARQUIVO, entrada);

	exibir_registro(0);
	// inserir_registro_indices(iprimary, ibrand, novo);
	// nregistros++;


}
void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Produto P) {

	int rrn = nregistros;
	int i;

	exibir_registro(0);

	if (iprimary->raiz == -1) {
		// Árvore vazia, insere o produto na raiz
		node_Btree_ip *raiz = (node_Btree_ip*) malloc(sizeof(node_Btree_ip));
		
		for (i = 0; i < raiz->num_chaves; i++)
			i++;
		printf("passou");

		// Insere na posição i da raiz
		strcpy(raiz->chave[i].pk, P.pk);
		raiz->chave[i].rrn = rrn;

		for (int i = 0; i < ordem_ip; i++)
			raiz->desc[i] = -1;

		// printf("raiz->num_chaves: %d\n", raiz->num_chaves);
		// printf("raiz->folha: %c\n", raiz->folha);
		// printf("raiz->chave->pk: %s\n", raiz->chave->pk);
		// printf("raiz->chave->rrn: %d\n", raiz->chave->rrn);

		write_btree_ip(raiz, 0);
	}	

}

/**** ALTERAÇÃO ****/ //todo

int alterar(Indice iprimary) {

}


/**** BUSCA ****/ //todo

void buscar(Indice iprimary, Indice ibrand) {

}


/**** LISTAGEM ****/ //todo

void listar(Indice iprimary, Indice ibrand) {

}


/* =====================================================================================
   ================================ FUNÇÕES ÁRVORE-B ===================================
   ===================================================================================== */


/* Escreve um nó da árvore no arquivo de índice. O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip) {


}

void write_btree_ip(node_Btree_ip *salvar, int rrn) {


}

void write_btree_is(node_Btree_is *salvar, int rrn) {


}


/* Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, char ip) {


}

node_Btree_ip *read_btree_ip(int rrn) {


}

node_Btree_is *read_btree_is(int rrn) {


}


// Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
// É conveniente que essa função também inicialize os campos necessários com valores nulos
void *criar_no(char ip) {

}

node_Btree_ip *criar_no_ip() {

}

node_Btree_is *criar_no_is() {

}
