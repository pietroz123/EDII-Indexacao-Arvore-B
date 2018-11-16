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


// Struct para as funções de inserção

typedef struct prom_dir_ip {
	Chave_ip chave_promovida;
	int filhoDireito;
} PromDirIp;

typedef struct prom_dir_is {
	Chave_is chave_promovida;
	int filhoDireito;
} PromDirIs;


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
void libera_no_ip(node_Btree_ip *x);
void libera_no_is(node_Btree_is *x);

// Buscar na árvore
int buscar_btree_ip(Indice *ip, char *chave, int modo);
char *buscar_btree_is(Indice *ip, char *chave);

// Imprimir
void pre_ordem(Indice ip);

// Funções de inserção

/* iprimary */
void insere_ip(Indice *ip, Chave_ip k);
PromDirIp insere_aux_ip(int rrnNo, Chave_ip k);
PromDirIp divide_no_ip(int rrnNo, Chave_ip k, int rrnDireito);

/* ibrand */
void insere_is(Indice *ip, Chave_is k);
PromDirIs insere_aux_is(int rrnNo, Chave_is k);
PromDirIs divide_no_is(int rrnNo, Chave_is k, int rrnDireito);



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


	char chave[TAM_PRIMARY_KEY];


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


//!DELETAR
void imprimir_node_ip(node_Btree_ip *no) {
	printf("%d\n", no->num_chaves);
	for (int i = 0; i < ordem_ip-1; i++)
		if (strlen(no->chave[i].pk) == 10)
			printf("%s %d\n", no->chave[i].pk, no->chave[i].rrn);
		else
			printf("NULL %d\n", no->chave[i].rrn);
	for (int i = 0; i < ordem_ip; i++)
		printf("%d ", no->desc[i]);
	printf("\n%c\n", no->folha);	
}
void imprimir_node_is(node_Btree_is *no) {
	printf("%d\n", no->num_chaves);
	for (int i = 0; i < ordem_is-1; i++)
		if (strlen(no->chave[i].pk) == 10)
			printf("%s %s\n", no->chave[i].pk, no->chave[i].string);
		else
			printf("NULL NULL\n", no->chave[i].string);
	for (int i = 0; i < ordem_is; i++)
		printf("%d ", no->desc[i]);
	printf("\n%c\n", no->folha);	
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
   ========================== FUNÇÕES DE COMPARAÇÃO ====================================
   ===================================================================================== */

int comparacao_Ip(const void *a, const void *b) {

	Chave_ip A = *(Chave_ip*)a;
	Chave_ip B = *(Chave_ip*)b;

	if (strcmp(A.pk, B.pk) == 0)
		return 0;
	else if (strcmp(A.pk, B.pk) > 0)
		return 1;
	else
		return -1;

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

	Chave_ip kp;
	strcpy(kp.pk, novo.pk);
	kp.rrn = nregistros;


	Chave_is ks;

	char string[TAM_STRING_INDICE];
	memset(string, 0, TAM_STRING_INDICE);
	strcat(string, novo.marca);
	strcat(string, "$");
	strcat(string, novo.nome);	

	int restantes = 101 - strlen(string);
    for (int i = 0; i < restantes; i++)
        strcat(string, "#");

	strcpy(ks.pk, novo.pk);
	strcpy(ks.string, string);
	


	if (iprimary->raiz == -1) {
		insere_ip(iprimary, kp);
		insere_is(ibrand, ks);		
	}
	else {
		int resultadoBusca = buscar_btree_ip(iprimary, novo.pk, 0);
		if (resultadoBusca == -1) {
			insere_ip(iprimary, kp);
			insere_is(ibrand, ks);
		} else {
			printf(ERRO_PK_REPETIDA, novo.pk);
			return;
		}
	}

	// Completa os espaços restantes com '#'
    int necessarios = 192 - strlen(entrada);
    for (int i = 0; i < necessarios; i++)
        strcat(entrada, "#");

	// Coloca a entrada no ARQUIVO de dados
	strcat(ARQUIVO, entrada);


	nregistros++;

	// printf("nregistros: %d\n", nregistros);  		//!?!
	// printf("nregistrosip: %d\n", nregistrosip);  	//!?!
	// printf("nregistrosis: %d\n", nregistrosis);  	//!?!
	// printf("iprimary->raiz: %d\n", iprimary->raiz);	//!?!

}


/**** ALTERAÇÃO ****/ //todo

int alterar(Indice iprimary) {

}


/**** BUSCA ****/ //todo

void buscar(Indice iprimary, Indice ibrand) {

	int opcaoBusca;
	scanf("%d", &opcaoBusca);
	getchar();

	int resultadoBuscaIp;
	char *resultadoBuscaIs;
	char chave[TAM_PRIMARY_KEY];

	char marca[TAM_MARCA];
	char nome[TAM_NOME];
	
	switch (opcaoBusca)
	{
		case 1:

			scanf("%[^\n]s", chave);
			getchar();
			
			printf(NOS_PERCORRIDOS_IP, chave);

			resultadoBuscaIp = buscar_btree_ip(&iprimary, chave, 1);
			if (resultadoBuscaIp != -1) {
				printf("\n");
				exibir_registro(resultadoBuscaIp);
			}
			else {
				printf("\n");
				printf(REGISTRO_N_ENCONTRADO);
				return;
			}

			break;

		case 2:

			scanf("%[^\n]s", marca);
			getchar();
			scanf("%[^\n]s", nome);
			getchar();

			// Gera a string de busca
			char string[TAM_STRING_INDICE];
			memset(string, 0, TAM_STRING_INDICE);
			strcat(string, marca);
			strcat(string, "$");
			strcat(string, nome);	

			// Antes de preencher com '#'
			printf(NOS_PERCORRIDOS_IS, string);

			int restantes = 101 - strlen(string);
			for (int i = 0; i < restantes; i++)
				strcat(string, "#");

			resultadoBuscaIs = buscar_btree_is(&ibrand, string);
			if (strcmp(resultadoBuscaIs, "-1")) {
				// printf("res: %s\n", resultadoBuscaIs);	//!?!
				resultadoBuscaIp = buscar_btree_ip(&iprimary, resultadoBuscaIs, 0);
				printf("\n");
				exibir_registro(resultadoBuscaIp);
			}
			else {
				printf("\n");
				printf(REGISTRO_N_ENCONTRADO);
				return;
			}


			break;
	
		default:
			break;
	}

}


/**** LISTAGEM ****/ //todo

void listar(Indice iprimary, Indice ibrand) {

	int opcaoListagem;
	scanf("%d", &opcaoListagem);

	
	switch (opcaoListagem)
	{
		case 1:
			
			// Caso nao exista nenhum registro
			if (iprimary.raiz == -1) {
				printf(REGISTRO_N_ENCONTRADO);
				return;
			}

			pre_ordem(iprimary);



			break;
	
		default:
			break;
	}

}


/* =====================================================================================
   ================================ FUNÇÕES ÁRVORE-B ===================================
   ===================================================================================== */


/* Escreve um nó da árvore no arquivo de índice. O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip) {


}


// Sobrescreve o registro de um nó na posição do RRN do Arquivo de Índices Primários
void write_btree_ip(node_Btree_ip *salvar, int rrn) {

	char *r = ARQUIVO_IP + rrn*tamanho_registro_ip;
	char registroIp[tamanho_registro_ip+1];		/* String para armazenar o novo registro */
	memset(registroIp, 0, sizeof(registroIp));	// "Zera" a string

	char nChaves[4];
	char chavePrimaria[TAM_PRIMARY_KEY];
	char RRN[5];
	char folha = salvar->folha;
	char descendente[4];

	// 3 bytes para o NÚMERO DE CHAVES 
	snprintf(nChaves, sizeof(nChaves), "%03d", salvar->num_chaves);
	strcat(registroIp, nChaves);

	// 10 bytes da CHAVE PRIMÁRIA e 4 bytes para o RRN
	for (int i = 0; i < ordem_ip-1; i++) {
		if (salvar->chave[i].rrn != -1) {
			sprintf(chavePrimaria, "%s", salvar->chave[i].pk);
			snprintf(RRN, sizeof(RRN), "%04d", salvar->chave[i].rrn);
		} else {
			sprintf(chavePrimaria, "##########");
			sprintf(RRN, "####");
		}

		strcat(registroIp, chavePrimaria);
		strcat(registroIp, RRN);
	}

	// 1 byte FOLHA (F) ou NÃO (N)
	if (folha == 'F')
		strcat(registroIp, "F");
	else
		strcat(registroIp, "N");

	// 3 bytes para os DESCENDENTES
	for (int i = 0; i < ordem_ip; i++) {
		if (salvar->desc[i] != -1) {
			snprintf(descendente, sizeof(descendente), "%03d", salvar->desc[i]);
		} else {
			sprintf(descendente, "***");
		}
		strcat(registroIp, descendente);
	}

	/* Coloca no ARQUIVO PRIMÁRIO */
	strncpy(r, registroIp, tamanho_registro_ip);

}

void write_btree_is(node_Btree_is *salvar, int rrn) {

	char *r = ARQUIVO_IS + rrn*tamanho_registro_is;
	char registroIs[tamanho_registro_is+1];		/* String para armazenar o novo registro */
	memset(registroIs, 0, sizeof(registroIs));	// "Zera" a string

	char nChaves[4];
	char chavePrimaria[TAM_PRIMARY_KEY];
	char string[TAM_STRING_INDICE];
	char folha = salvar->folha;
	char descendente[4];

	// 3 bytes para o NÚMERO DE CHAVES 
	snprintf(nChaves, sizeof(nChaves), "%03d", salvar->num_chaves);
	strcat(registroIs, nChaves);


	// 10 bytes da CHAVE PRIMÁRIA e 101 bytes para a STRING
	for (int i = 0; i < ordem_is-1; i++) {
		if (strlen(salvar->chave[i].pk)) {

			sprintf(chavePrimaria, "%s", salvar->chave[i].pk);
			sprintf(string, "%s", salvar->chave[i].string);
		
		} else {
		
			sprintf(chavePrimaria, "##########");
			sprintf(string, "#####################################################################################################");
		
		}

		strcat(registroIs, chavePrimaria);
		strcat(registroIs, string);
	}

	// 1 byte FOLHA (F) ou NÃO (N)
	if (folha == 'F')
		strcat(registroIs, "F");
	else
		strcat(registroIs, "N");

	// 3 bytes para os DESCENDENTES
	for (int i = 0; i < ordem_is; i++) {
		if (salvar->desc[i] != -1) {
			snprintf(descendente, sizeof(descendente), "%03d", salvar->desc[i]);
		} else {
			sprintf(descendente, "***");
		}
		strcat(registroIs, descendente);
	}

	/* Coloca no ARQUIVO PRIMÁRIO */
	strncpy(r, registroIs, tamanho_registro_is);

}


/* Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, char ip) {


}

node_Btree_ip *read_btree_ip(int rrn) {

	// Recupera o registro e armazena em temp
	char temp[tamanho_registro_ip+1], *p;
	temp[tamanho_registro_ip] = '\0';
	strncpy(temp, ARQUIVO_IP + ((rrn)*tamanho_registro_ip), tamanho_registro_ip);


	int i = 0;
	char nChaves[4];
	char chavePrimaria[TAM_PRIMARY_KEY];
	char descendente[4];
	char RRN[5];
	char folha;								
	char *r = temp;

	// Cria um nó
	node_Btree_ip *recuperar = criar_no_ip();

	// Recupera o NÚMERO DE CHAVES
	snprintf(nChaves, 4, "%s", r);
	recuperar->num_chaves = atoi(nChaves);
	r += 3;
	

	// Recupera as CHAVES e RRNS
	for (int n = 0; n < ordem_ip-1; n++) {

		snprintf(chavePrimaria, TAM_PRIMARY_KEY, "%s", r);
		if (strcmp(chavePrimaria, "##########") != 0)
			strcpy(recuperar->chave[n].pk, chavePrimaria);
		r += 10;
		snprintf(RRN, 5, "%s", r);
		if (strcmp(RRN, "####") != 0)
			recuperar->chave[n].rrn = atoi(RRN);
		else
			recuperar->chave[n].rrn = -1;
		r += 4;

	}
	
	// Recupera FOLHA ou NÃO
	recuperar->folha = *r;
	r += 1;

	// Recupera os DESCENDENTES
	for (int n = 0; n < ordem_ip; n++) {

		snprintf(descendente, 4, "%s", r);
		if (strcmp(descendente, "***"))
			recuperar->desc[n] = atoi(descendente);
		else
			recuperar->desc[n] = -1;
		if (n < ordem_ip-1)
			r += 3;

	}

	return recuperar;

}

node_Btree_is *read_btree_is(int rrn) {

	// Recupera o registro e armazena em temp
	char temp[tamanho_registro_is+1], *p;
	temp[tamanho_registro_is] = '\0';
	strncpy(temp, ARQUIVO_IS + ((rrn)*tamanho_registro_is), tamanho_registro_is);


	int i = 0;
	char nChaves[4];
	char chavePrimaria[TAM_PRIMARY_KEY];
	char string[TAM_STRING_INDICE];
	char descendente[4];
	char folha;								
	char *r = temp;

	// Cria um nó
	node_Btree_is *recuperar = criar_no_is();

	// Recupera o NÚMERO DE CHAVES
	snprintf(nChaves, 4, "%s", r);
	recuperar->num_chaves = atoi(nChaves);
	r += 3;
	

	// Recupera as CHAVES e STRINGS
	for (int n = 0; n < ordem_is-1; n++) {

		snprintf(chavePrimaria, TAM_PRIMARY_KEY, "%s", r);
		if (strcmp(chavePrimaria, "##########") != 0)
			strcpy(recuperar->chave[n].pk, chavePrimaria);
		r += 10;
		snprintf(string, TAM_STRING_INDICE, "%s", r);
		if (strcmp(string, "#####################################################################################################") != 0)
			strcpy(recuperar->chave[n].string, string);
		r += 101;

	}
	
	// Recupera FOLHA ou NÃO
	recuperar->folha = *r;
	r += 1;

	// Recupera os DESCENDENTES
	for (int n = 0; n < ordem_is; n++) {

		snprintf(descendente, 4, "%s", r);
		if (strcmp(descendente, "***"))
			recuperar->desc[n] = atoi(descendente);
		else
			recuperar->desc[n] = -1;
		if (n < ordem_is-1)
			r += 3;

	}

	return recuperar;

}


// Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
// É conveniente que essa função também inicialize os campos necessários com valores nulos
void *criar_no(char ip) {

}

node_Btree_ip *criar_no_ip() {
	
	// Aloca o novo nó
	node_Btree_ip *novo = (node_Btree_ip*) malloc(sizeof(node_Btree_ip));
	
	// Aloca os vetores
	novo->chave = (Chave_ip*) malloc((ordem_ip-1) * sizeof(Chave_ip));	/* MAXchaves = M-1 */
	novo->desc = (int*) malloc((ordem_ip) * sizeof(int));				/* MAXfilhos = M */

	// Inicializa os rrn's das chaves com -1 e os descendentes com -1
	for (int i = 0; i < ordem_ip-1; i++)
		novo->chave[i].rrn = -1;
	for (int i = 0; i < ordem_ip; i++)
		novo->desc[i] = -1;
	
	// Cria o nó como folha
	novo->folha = 'F';
	
	// O numero de chaves inicialmente é 0
	novo->num_chaves = 0;

	return novo;
}

node_Btree_is *criar_no_is() {

	// Aloca o novo nó
	node_Btree_is *novo = (node_Btree_is*) malloc(sizeof(node_Btree_is));
	
	// Aloca os vetores
	novo->chave = (Chave_is*) malloc((ordem_is-1) * sizeof(Chave_is));	/* MAXchaves = M-1 */
	novo->desc = (int*) malloc((ordem_is) * sizeof(int));				/* MAXfilhos = M */

	// Inicializa as chaves (pk), nomes (string) e descententes
	for (int i = 0; i < ordem_is-1; i++) {
		memset(novo->chave[i].pk, 0, TAM_PRIMARY_KEY);
		memset(novo->chave[i].string, 0, TAM_STRING_INDICE);
	}
	for (int i = 0; i < ordem_is; i++)
		novo->desc[i] = -1;
	
	// Cria o nó como folha
	novo->folha = 'F';
	
	// O numero de chaves inicialmente é 0
	novo->num_chaves = 0;

	return novo;

}





/* PSEUDOCÓDIGOS -> CÓDIGO ÁRVORE B */


/* ==========================================
   ================ BUSCA ===================
   ========================================== */


/* ========= BUSCA INDICE PRIMARIO - iprimary ========== */

void imprimir_no_buscar_ip(node_Btree_ip *x) {
	for (int i = 0; i < x->num_chaves; i++) {
		if (i == x->num_chaves-1)
			printf("%s", x->chave[i].pk);
		else
			printf("%s, ", x->chave[i].pk);
	}
	printf("\n");
}
int buscar_btree_ip_privado(int rrn, char *chave, int modo) {

	node_Btree_ip *atual = read_btree_ip(rrn);
	if (modo == 1)
		imprimir_no_buscar_ip(atual);

	int i = 0;
	while (i < atual->num_chaves && strcmp(chave, atual->chave[i].pk) > 0) {
		i++;
	}

	if (i < atual->num_chaves && strcmp(chave, atual->chave[i].pk) == 0) {
		int resultado = atual->chave[i].rrn;
		libera_no_ip(atual);
		return resultado;
	}

	if (atual->folha == 'F') {
		libera_no_ip(atual);
		return -1;
	}
	else { 
		int resultado = buscar_btree_ip_privado(atual->desc[i], chave, modo);
		libera_no_ip(atual);
		return resultado;
	}


}
int buscar_btree_ip(Indice *ip, char *chave, int modo) {
	return buscar_btree_ip_privado(ip->raiz, chave, modo);
}



/* ========= BUSCA INDICE SECUNDARIO - ibrand ========== */

void imprimir_no_buscar_is(node_Btree_is *x) {
	char *p;
	for (int i = 0; i < x->num_chaves; i++) {
		char string[TAM_STRING_INDICE];
		strcpy(string, x->chave[i].string);
		p = strtok(string, "#");
		if (i == x->num_chaves-1)
			printf("%s", p);
		else
			printf("%s, ", p);
	}
	printf("\n");
}
char *buscar_btree_is_privado(int rrn, char *chave) {

	node_Btree_is *atual = read_btree_is(rrn);
	imprimir_no_buscar_is(atual);

	int i = 0;
	while (i < atual->num_chaves && strcmp(chave, atual->chave[i].string) > 0) {
		// printf("chave: %s\natual: %s\n", chave, atual->chave[i].string);
		i++;
	}

	if (i < atual->num_chaves && strcmp(chave, atual->chave[i].string) == 0) {
		libera_no_is(atual);
		return atual->chave[i].pk;
	}

	if (atual->folha == 'F') {
		libera_no_is(atual);
		return "-1";
	}
	else { 
		char *resultado = buscar_btree_is_privado(atual->desc[i], chave);
		libera_no_is(atual);
		return resultado;
	}


}
char *buscar_btree_is(Indice *ip, char *chave) {
	return buscar_btree_is_privado(ip->raiz, chave);
}


/* ==========================================
   =============== PERCURSO =================
   ========================================== */


void imprimir_no_listar_ip(node_Btree_ip *x, int nivel) {
	for (int i = 0; i < x->num_chaves; i++) {
		if (i == 0)
			if (i+1 == x->num_chaves)
				printf("%d - %s", nivel, x->chave[i].pk);
			else
				printf("%d - %s, ", nivel, x->chave[i].pk);
		else
			if (i == x->num_chaves-1)
				printf("%s", x->chave[i].pk);
			else
				printf("%s, ", x->chave[i].pk);
	}
	printf("\n");
}
void pre_ordem_privado(int rrn, int nivel) {

	if (rrn == -1)
		return;

	node_Btree_ip *atual = read_btree_ip(rrn);
	imprimir_no_listar_ip(atual, nivel);

	for (int i = 0; i <= atual->num_chaves; i++) {
		pre_ordem_privado(atual->desc[i], nivel+1);
	}

	libera_no_ip(atual);

}
void pre_ordem(Indice ip) {
	pre_ordem_privado(ip.raiz, 1);
}


/* ==========================================
   =============== INSERÇÃO =================
   ========================================== */



void imprime_prom_dir_ip(PromDirIp atual) {
	printf("atual.chavePromovida.pk: %s\n", atual.chave_promovida.pk);
	printf("atual.chave_promovida.rrn: %d\n", atual.chave_promovida.pk);
	printf("atual.filhoDireito: %d\n", atual.filhoDireito);
}


/* =========================================================== 
   ===== INSERÇÃO ÁRVORE DE ÍNDICES PRIMÁRIOS - iprimary ===== 
   =========================================================== */


PromDirIp divide_no_ip(int rrnNo, Chave_ip k, int rrnDireito) {

	// printf("rrnNo: %d\nrrnDireito: %d\n", rrnNo, rrnDireito);   //!?!
	
	node_Btree_ip *X = read_btree_ip(rrnNo);
	
	// printf("X:\n");   //!?!
	// imprimir_node_ip(X);   //!?!

	
	int i = X->num_chaves-1;
	int chave_alocada = 0;

	node_Btree_ip *Y = criar_no_ip();
	Y->folha = X->folha;
	Y->num_chaves = (ordem_ip-1) / 2;
	// printf("Criou Y:\n");   //!?!
	// imprimir_node_ip(Y);   //!?!

	for (int j = Y->num_chaves-1; j >= 0; j--) {
		if (!chave_alocada && strcmp(k.pk, X->chave[i].pk) > 0) {
			Y->chave[j] = k;
			Y->desc[j+1] = rrnDireito;
			chave_alocada = 1; 
		}
		else {
			Y->chave[j] = X->chave[i];
			Y->desc[j+1] = X->desc[i+1];
			i--;
		}
	}

	// printf("Apos MOVIMENTACAO:\n");   //!?!
	// printf("X:\n");   //!?!
	// imprimir_node_ip(X);   //!?!
	// printf("Y:\n");   //!?!
	// imprimir_node_ip(Y);   //!?!

	if (!chave_alocada) {
		while (i >= 0 && strcmp(k.pk, X->chave[i].pk) < 0) {
			X->chave[i+1] = X->chave[i];
			X->desc[i+2] = X->desc[i+1];
			i--;
		}
		X->chave[i+1] = k;
		X->desc[i+2] = rrnDireito;
	}

	// printf("Apos LIBERACAO:\n");   //!?!
	// printf("X:\n");   //!?!
	// imprimir_node_ip(X);   //!?!
	// printf("Y:\n");   //!?!
	// imprimir_node_ip(Y);   //!?!

	Chave_ip chave_promovida;								// Promove a chave mediana
	chave_promovida = X->chave[(ordem_ip/2)];

	Y->desc[0] = X->desc[(ordem_ip/2)+1];					
	X->num_chaves = (ordem_ip / 2);							// O número de chaves é reduzido pela metade


	PromDirIp retorno;
	retorno.chave_promovida = chave_promovida;
	retorno.filhoDireito = nregistrosip;

	// Zera os descendentes remanescentes
	if (X->num_chaves < ordem_ip) {
		for (int i = ordem_ip-1; i > X->num_chaves; i--)
			X->desc[i] = -1;
	}
	
	// Zera a posicao que contem a chave promovida
	memset(X->chave[(ordem_ip/2)].pk, 0, sizeof(X->chave[(ordem_ip/2)].pk));
	X->chave[(ordem_ip/2)].rrn = -1;
	

	// printf("Apos PROMOCAO:\n");   //!?!
	// printf("X:\n");   //!?!
	// imprimir_node_ip(X);   //!?!
	// printf("Y:\n");   //!?!
	// imprimir_node_ip(Y);   //!?!


	write_btree_ip(X, rrnNo);
	write_btree_ip(Y, nregistrosip);
	// Podemos desalocar Y visto que já o escrevemos no ARQUIVO_IP
	libera_no_ip(X);
	libera_no_ip(Y);


	// char *p;   //!?!
	// if (!*ARQUIVO_IP)   //!?!
		// puts(ARQUIVO_VAZIO);   //!?!
	// else   //!?!
		// for (p = ARQUIVO_IP; *p != '\0'; p += tamanho_registro_ip)   //!?!
		// {   //!?!
			// fwrite(p, 1, tamanho_registro_ip, stdout);   //!?!
			// puts("");   //!?!
		// }   //!?!
	nregistrosip++;

	return retorno;
}

PromDirIp insere_aux_ip(int rrnNo, Chave_ip k) {

	int i;
	node_Btree_ip *X = read_btree_ip(rrnNo);
	// imprimir_node_ip(X);   //!?!
	
	if (X->folha == 'F') {
		// printf("X eh folha\n");   //!?!
		if (X->num_chaves < ordem_ip-1) {
			// printf("Existe espaco\n");   //!?!
			i = X->num_chaves-1;

			while (i >= 0 && strcmp(k.pk, X->chave[i].pk) < 0) {
				X->chave[i+1] = X->chave[i];
				i--;
			}
			X->chave[i+1] = k;
			X->num_chaves++;
			// printf("Feita a alteracao em X\n");   //!?!
			// imprimir_node_ip(X);   //!?!

			PromDirIp r;
			memset(r.chave_promovida.pk, 0, sizeof(r.chave_promovida.pk));
			r.chave_promovida.rrn = -1;
			r.filhoDireito = -1;
			// imprime_prom_dir_ip(r);   //!?!

			write_btree_ip(X, rrnNo);
			libera_no_ip(X);

			return r; // return NULL, NULL
		} 
		else {
			// printf("Nao existe espaco. Vai dividir\n");   //!?!
			libera_no_ip(X);
			return divide_no_ip(rrnNo, k, -1);
		} 	
	}
	else {
		// printf("X nao eh folha\n");   //!?!
		i = X->num_chaves-1;
		while (i >= 0 && strcmp(k.pk, X->chave[i].pk) < 0)
			i--;
		i++;

		// printf("Vai chamar insere_aux_ip\n");   //!?!
		PromDirIp atual = insere_aux_ip(X->desc[i], k);
		// printf("Chamou insere_aux_ip\n");   //!?!
		// imprime_prom_dir_ip(atual);   //!?!

		if (strlen(atual.chave_promovida.pk)) {
			// printf("Houve overflow\n");   //!?!
			k = atual.chave_promovida;
			// printf("Como esta X:\n");   //!?!
			// imprimir_node_ip(X);   //!?!
			if (X->num_chaves < ordem_ip-1) {
				// printf("Existe espaco\n");   //!?!
				i = X->num_chaves-1;

				// printf("X antes da movimentacao:\n");   //!?!
				// imprimir_node_ip(X);   //!?!

				while (i >= 0 && strcmp(k.pk, X->chave[i].pk) < 0) {
					X->chave[i+1] = X->chave[i];
					X->desc[i+2] = X->desc[i+1];
					i--;
				}

				// printf("Como esta X apos movimentacao:\n");   //!?!
				// imprimir_node_ip(X);   //!?!

				X->chave[i+1] = k;
				X->desc[i+2] = atual.filhoDireito;
				X->num_chaves++;

				// printf("X apos insercao:\n");   //!?!
				// imprimir_node_ip(X);   //!?!

				PromDirIp r;
				memset(r.chave_promovida.pk, 0, sizeof(r.chave_promovida.pk));
				r.chave_promovida.rrn = -1;
				r.filhoDireito = -1;
				// printf("retorno:\n");   //!?!
				// imprime_prom_dir_ip(r);   //!?!

				write_btree_ip(X, rrnNo);
				libera_no_ip(X);

				return r; // return NULL, NULL
			}
			else {
				// printf("Nao existe espaco. Vai dividir.\n");   //!?!
				// Não há espaço, portanto realizamos um split
				libera_no_ip(X);
				return divide_no_ip(rrnNo, k, atual.filhoDireito);
			}
		}
		else {
			// printf("Nao houve overflow\n");   //!?!
			PromDirIp r;
			memset(r.chave_promovida.pk, 0, sizeof(r.chave_promovida.pk));
			r.chave_promovida.rrn = -1;
			r.filhoDireito = -1;
			// printf("retorno:\n");   //!?!
			// imprime_prom_dir_ip(r);   //!?!

			libera_no_ip(X);
			
			return r; // return NULL, NULL
		}
	}


}


void insere_ip(Indice *ip, Chave_ip k) {

	// printf("------ Vai inserir '%s' ------\n", k.pk);   //!?!

	if (ip->raiz == -1) {
		// printf("ip->raiz == -1\n");   //!?!
		node_Btree_ip *X = criar_no_ip();
		X->folha = 'F';
		X->num_chaves = 1;
		X->chave[0] = k;
		
		ip->raiz = 0;

		write_btree_ip(X, ip->raiz);
		libera_no_ip(X);

		nregistrosip++;
	}
	else {
		// printf("ip->raiz != -1 => %d\n", ip->raiz);   //!?!
		// printf("Primeira chamada do insere_aux_ip em insere_ip\n");   //!?!
		PromDirIp atual = insere_aux_ip(ip->raiz, k);
		// printf("Voltou para insere_ip()\n");   //!?!
		// printf("Retorno:\n");   //!?!
		// imprime_prom_dir_ip(atual);   //!?!

		if (strlen(atual.chave_promovida.pk)) {
			// printf("Ocorreu overflow na raiz. Vai criar uma nova\n");   //!?!
			node_Btree_ip *X = criar_no_ip();
			X->folha = 'N';
			X->num_chaves = 1;
			X->chave[0] = atual.chave_promovida;
			
			X->desc[0] = ip->raiz;
			X->desc[1] = atual.filhoDireito;
			
			ip->raiz = nregistrosip;

			write_btree_ip(X, nregistrosip);
			libera_no_ip(X);

			nregistrosip++;
		}
		else {
			// printf("Nao ocorreu overflow\n");   //!?!
		}

	}


}


/* =========================================================== 
   ===== INSERÇÃO ÁRVORE DE ÍNDICES SECUNDÁRIOS - ibrand ===== 
   =========================================================== */


PromDirIs divide_no_is(int rrnNo, Chave_is k, int rrnDireito) {

	
	node_Btree_is *X = read_btree_is(rrnNo);
	

	
	int i = X->num_chaves-1;
	int chave_alocada = 0;

	node_Btree_is *Y = criar_no_is();
	Y->folha = X->folha;
	Y->num_chaves = (ordem_ip-1) / 2;

	for (int j = Y->num_chaves-1; j >= 0; j--) {
		if (!chave_alocada && strcmp(k.string, X->chave[i].string) > 0) {
			Y->chave[j] = k;
			Y->desc[j+1] = rrnDireito;
			chave_alocada = 1; 
		}
		else {
			Y->chave[j] = X->chave[i];
			Y->desc[j+1] = X->desc[i+1];
			i--;
		}
	}


	if (!chave_alocada) {
		while (i >= 0 && strcmp(k.string, X->chave[i].string) < 0) {
			X->chave[i+1] = X->chave[i];
			X->desc[i+2] = X->desc[i+1];
			i--;
		}
		X->chave[i+1] = k;
		X->desc[i+2] = rrnDireito;
	}


	Chave_is chave_promovida;								// Promove a chave mediana
	chave_promovida = X->chave[(ordem_ip/2)];

	Y->desc[0] = X->desc[(ordem_ip/2)+1];					
	X->num_chaves = (ordem_ip / 2);							// O número de chaves é reduzido pela metade


	PromDirIs retorno;
	retorno.chave_promovida = chave_promovida;
	retorno.filhoDireito = nregistrosis;

	// Zera os descendentes remanescentes
	if (X->num_chaves < ordem_ip) {
		for (int i = ordem_ip-1; i > X->num_chaves; i--)
			X->desc[i] = -1;
	}
	
	// Zera a posicao que contem a chave promovida
	memset(X->chave[(ordem_ip/2)].pk, 0, sizeof(X->chave[(ordem_ip/2)].pk));
	memset(X->chave[(ordem_is/2)].string, 0, TAM_STRING_INDICE);
	


	write_btree_is(X, rrnNo);
	write_btree_is(Y, nregistrosis);
	// Podemos desalocar Y visto que já o escrevemos no ARQUIVO_IP
	libera_no_is(X);
	libera_no_is(Y);


	nregistrosis++;

	return retorno;
}

PromDirIs insere_aux_is(int rrnNo, Chave_is k) {

	int i;
	node_Btree_is *X = read_btree_is(rrnNo);
	
	if (X->folha == 'F') {
		if (X->num_chaves < ordem_ip-1) {
			i = X->num_chaves-1;

			while (i >= 0 && strcmp(k.string, X->chave[i].string) < 0) {
				X->chave[i+1] = X->chave[i];
				i--;
			}
			X->chave[i+1] = k;
			X->num_chaves++;

			PromDirIs r;
			memset(r.chave_promovida.pk, 0, sizeof(r.chave_promovida.pk));
			memset(r.chave_promovida.string, 0, TAM_STRING_INDICE);
			r.filhoDireito = -1;

			write_btree_is(X, rrnNo);
			libera_no_is(X);

			return r; // return NULL, NULL
		} 
		else {
			libera_no_is(X);
			return divide_no_is(rrnNo, k, -1);
		} 	
	}
	else {
		i = X->num_chaves-1;
		while (i >= 0 && strcmp(k.string, X->chave[i].string) < 0)
			i--;
		i++;

		PromDirIs atual = insere_aux_is(X->desc[i], k);

		if (strlen(atual.chave_promovida.pk)) {
			k = atual.chave_promovida;
			if (X->num_chaves < ordem_ip-1) {
				i = X->num_chaves-1;


				while (i >= 0 && strcmp(k.string, X->chave[i].string) < 0) {
					X->chave[i+1] = X->chave[i];
					X->desc[i+2] = X->desc[i+1];
					i--;
				}


				X->chave[i+1] = k;
				X->desc[i+2] = atual.filhoDireito;
				X->num_chaves++;


				PromDirIs r;
				memset(r.chave_promovida.pk, 0, sizeof(r.chave_promovida.pk));
				memset(r.chave_promovida.string, 0, TAM_STRING_INDICE);
				r.filhoDireito = -1;

				write_btree_is(X, rrnNo);
				libera_no_is(X);

				return r; // return NULL, NULL
			}
			else {
				// Não há espaço, portanto realizamos um split
				libera_no_is(X);
				return divide_no_is(rrnNo, k, atual.filhoDireito);
			}
		}
		else {
			PromDirIs r;
			memset(r.chave_promovida.pk, 0, sizeof(r.chave_promovida.pk));
			memset(r.chave_promovida.string, 0, TAM_STRING_INDICE);
			r.filhoDireito = -1;

			libera_no_is(X);
			
			return r; // return NULL, NULL
		}
	}


}


void insere_is(Indice *ip, Chave_is k) {


	if (ip->raiz == -1) {
		node_Btree_is *X = criar_no_is();
		X->folha = 'F';
		X->num_chaves = 1;
		X->chave[0] = k;
		
		ip->raiz = 0;

		write_btree_is(X, ip->raiz);
		libera_no_is(X);

		nregistrosis++;
	}
	else {
		PromDirIs atual = insere_aux_is(ip->raiz, k);

		if (strlen(atual.chave_promovida.pk)) {
			node_Btree_is *X = criar_no_is();
			X->folha = 'N';
			X->num_chaves = 1;
			X->chave[0] = atual.chave_promovida;
			
			X->desc[0] = ip->raiz;
			X->desc[1] = atual.filhoDireito;
			
			ip->raiz = nregistrosis;

			write_btree_is(X, nregistrosis);
			libera_no_is(X);

			nregistrosis++;
		}
		else {
		}

	}


}



/* ==========================================
   =============== DESALOCAR ================
   ========================================== */


void libera_no_ip(node_Btree_ip *x) {
	free(x->chave);
	free(x->desc);
	free(x);
}
void libera_no_is(node_Btree_is *x) {
	free(x->chave);
	free(x->desc);
	free(x);
}