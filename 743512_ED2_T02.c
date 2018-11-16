/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
 *
 * RA: 743512
 * Aluno: Bianca Gomes Rodrigues
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51
#define TAM_STRING_INDICE (TAM_MARCA + TAM_NOME)


#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define MAX_ORDEM 150
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas do usuário */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!"
#define INICIO_BUSCA 				"********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM				"********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 			"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO				"********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO      "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO    "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO  				 	"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 	"FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP 			"Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS 			"Busca por %s.\nNos percorridos:\n"


/* Registro do Produto */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Produto;


/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um  do Índice Secundário*/
typedef struct Chaveis{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip{
	int num_chaves;			/* numero de chaves armazenadas*/
	Chave_ip *chave;		/* vetor das chaves e rrns [m-1]*/
	int *desc;				/* ponteiros para os descendentes, *desc[m]*/
	char folha;				/* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis{
	int num_chaves;			/* numero de chaves armazenadas*/
	Chave_is *chave;		/* vetor das chaves e rrns [m-1]*/
	int *desc;				/* ponteiros para os descendentes, *desc[m]*/
	char folha;				/* flag folha da arvore*/
} node_Btree_is;

typedef struct {
	int raiz;
} Indice;

/* ---------- */

// A função "InsereIP" retorna dois dados, as informações da CHAVE PROMOVIDA, 
//ou seja, PRIMARY KEY e RRN, e o RRN do FILHO DIREITO para a função "Cadastrar". 
//Entretando, funções em C não permitem o retorno de duas informações, deste modo
//precisamos criar uma STRUCT para isso seja possível.

typedef struct{
	Chave_ip cPromovida;
	int fDireito;
}Dados;

/* ---------- */

/* Variáveis globais */
char ARQUIVO[MAX_REGISTROS * TAM_REGISTRO + 1];
char ARQUIVO_IP[2000*sizeof(Chave_ip)];
char ARQUIVO_IS[2000*sizeof(Chave_is)];
/* Ordem das arvores */
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
/*Quantidade de bytes que ocupa cada nó da árvore nos arquivos de índice:*/
int tamanho_registro_ip;
int tamanho_registro_is;
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

Produto recuperar_registro(int RRN);

void gerarChave(Produto * Novo);

//Inserir "Auxiliar"
Dados InserirIP(int RRN, Chave_ip *Chave);

/* A função splitNode recebe o RRN do NÓ(Página), a CHAVE e o RRN do FILHO DIREITO*/
Dados splitNode(int node, Chave_ip *Chave, int filhoDireito);

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */
 
 /********************FUNÇÕES DO MENU*********************/
 void cadastrar(Indice* iprimary, Indice* ibrand);

 int alterar(Indice iprimary);
 
 void buscar(Indice iprimary,Indice ibrand);
 
 void listar(Indice iprimary,Indice ibrand);
 
 /*******************************************************/
 
 /* Recebe do usuário uma string simulando o arquivo completo e retorna o número
  * de registros. */
 int carregar_arquivo();
 
/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary);
 
/* (Re)faz o índice de jogos  */
void criar_ibrand(Indice *ibrand) ;
 
/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */

//void write_btree(void *salvar, int rrn, char ip);

/* ---------------------- */
void write_btree_ip(node_Btree_ip *salvar, int rrn);

void write_btree_is(node_Btree_is *salvar, int rrn);
/* ---------------------- */

/*Lê um nó do arquivo de índice e retorna na estrutura*/
//void *read_btree(int rrn, char ip);

/* ---------------------- */
node_Btree_ip *read_btree_ip(int RRN);

node_Btree_is *read_btree_is(int RRN);
/* ---------------------- */
 
/* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
* É conveniente que essa função também inicialize os campos necessários com valores nulos*/
//void *criar_no(char ip);
 
void CriarNode_IP(node_Btree_ip *newNode);

void CriarNode_IS(node_Btree_is *newNode);

/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no(void *node, char ip);
 
/*
*   Caro aluno,
*   caso não queira trabalhar com void*, é permitido dividir as funções que utilizam
* em duas, sendo uma para cada índice...
* Um exemplo, a write_btree e read_btree ficariam como:
*
*   void write_btree_ip(node_Btree_ip *salvar, int rrn),  node_Btree_ip *read_btree_ip(int rrn),
*   void write_btree_is(node_Btree_is *salvar, int rrn) e node_Btree_is *read_btree_is(int rrn).
*/
 
/* Atualiza os dois índices com o novo registro inserido */
//void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Jogo j);
 
/* Exibe o jogo */
int exibir_registro(int rrn);

int main()
{
	char *p; /* # */
  /* Arquivo */
	int carregarArquivo = 0; nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d\n", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip = ordem_ip*3 + 4 + (-1 + ordem_ip)*14;
	tamanho_registro_is = ordem_is*3 + 4 + (-1 + ordem_is)* (	TAM_STRING_INDICE +9);

	/* Índice primário */
	Indice iprimary ;
	criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Produtos */
	Indice ibrand;
	//criar_ibrand(&ibrand);

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao) {
		case 1: /* Cadastrar um novo Produto */
			cadastrar(&iprimary, &ibrand);
			break;
		case 2: /* Alterar o desconto de um Produto */
			printf(INICIO_ALTERACAO);
			// if (alterar(iprimary))
			// 	printf(SUCESSO);
			// else
			// 	printf(FALHA);
			break;
		case 3: /* Buscar um Produto */
			printf(INICIO_BUSCA);
			//buscar(iprimary, ibrand);
			break;
		case 4: /* Listar todos os Produtos */
			printf(INICIO_LISTAGEM);
			//listar(iprimary, ibrand);
			break;
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		case 6: /* Imprime o Arquivo de Índice Primário*/
			printf(INICIO_INDICE_PRIMARIO);
			if(!*ARQUIVO_IP)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IP; *p!='\0'; p+=tamanho_registro_ip)
				{
					fwrite( p , 1 ,tamanho_registro_ip,stdout);
					puts("");
				}
			break;
		case 7: /* Imprime o Arquivo de Índice Secundário*/
			printf(INICIO_INDICE_SECUNDARIO);
			if(!*ARQUIVO_IS)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IS; *p!='\0'; p+=tamanho_registro_is)
				{
					fwrite( p , 1 ,tamanho_registro_is,stdout);
					puts("");

				}
			break;
		case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
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

Produto recuperar_registro(int RRN){
	
	char Registro[193], *p;
	strncpy(Registro, ARQUIVO + ((RRN)*192), 192);
	Registro[192] = '\0';

	Produto A;
	p = strtok(Registro,"@");
	strcpy(A.pk,p);
	p = strtok(NULL,"@");
	strcpy(A.nome,p);
	p = strtok(NULL,"@");
	strcpy(A.marca,p);
	p = strtok(NULL,"@");
	strcpy(A.data,p);
	p = strtok(NULL,"@");
	strcpy(A.ano,p);
	p = strtok(NULL,"@");
	strcpy(A.preco,p);
	p = strtok(NULL,"@");
	strcpy(A.desconto,p);
	p = strtok(NULL,"@");
	strcpy(A.categoria,p);
	
	return A;

}

/* Exibe o Produto */
int exibir_registro(int rrn)
{
	if(rrn < 0)
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
	sscanf(j.desconto,"%d",&desconto);
	sscanf(j.preco,"%f",&preco);
	preco = preco *  (100-desconto);
	preco = ((int) preco)/ (float) 100 ;
	printf("%07.2f\n",  preco);
	strcpy(categorias, j.categoria);
	
	for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    	printf("%s ", cat);
	printf("\n");

	return 1;
}

 /*================================= FUNÇÕES ================================*/

/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary){
	/*Indica que a ARVORE-B está VAZIA*/
	iprimary->raiz = -1;
}
 
node_Btree_ip *read_btree_ip(int RRN){
	
	//Recupera do ARQUIVO_IP
	// char Pagina[tamanho_registro_ip+1];
	// strncpy(Pagina, ARQUIVO_IP + ((RRN)*tamanho_registro_ip), tamanho_registro_ip);
	// Pagina[tamanho_registro_ip] = '\0';

	node_Btree_ip * noAtual = (node_Btree_ip*)malloc(sizeof(node_Btree_ip));
	CriarNode_IP(noAtual);

	char *P = ARQUIVO_IP + ((RRN)*tamanho_registro_ip);

	char nChaves[4];
	nChaves[3] = '\0';

	/*Recupera o NÚMERO DE CHAVES que ocupam as 3 primeiras posições da PÁGINA*/
	strncpy(nChaves, P, 3);
	/*Por conseguinte, precisamos atualizar está informação no noAtual*/
	noAtual->num_chaves = atoi(nChaves);
	
	/*COMENTAR*/
	//printf("\nnChaves %s\n", nChaves);
	//printf("noAtual->num_chaves %d\n\n", noAtual->num_chaves);
	
	/*Após recuperar o NÚMERO de CHAVES, movimento três posições para frente o ponteiro*/
	for(int i=0; i<3; i++)
		*P++;
	
	char PrimaryKey[TAM_PRIMARY_KEY];

	int j = 0;	
	while(j < ordem_ip-1){

		/*Recuperamos a PRIMARY KEY de 10 bytes*/
		strncpy(noAtual->chave[j].pk, P, 10);
		
		/*COMENTAR*/
		//printf("noAtual->chave[j].pk %s\n", noAtual->chave[j].pk);
		
		/*Após isso movimentamos o ponteiro*/
		int k = 0;
		for(k=0; k<10; k++ )
			*P++;
		
		/*Auxiliar para recuperar o RRN de 4 bytes*/
		char RRN[5];
		RRN[4] = '\0';
		strncpy(RRN, P, 4);
		
		/*COMENTAR*/
		//printf("RRN %s\n", RRN);
		if(strcmp(RRN, "####")==0){
			noAtual->chave[j].rrn = -1;	
		}
		else
			noAtual->chave[j].rrn = atoi(RRN);

		/*COMENTAR*/
		//printf("noAtual->chave[j].rrn %d\n\n", noAtual->chave[j].rrn);

		/*Novamente, após isso movimentamos o ponteiro*/
		k = 0;
		for(k=0; k<4; k++)
			*P++;

		j++;
	}

	char Folha[2];
	Folha[1] = '\0';
	strncpy(Folha, P, 1);

	/*COMENTAR*/
	//printf("Folha %s\n", Folha);

	if(strcmp(Folha, "F")== 0)
		noAtual->folha = 'F';
	else	
		noAtual->folha = 'N';

	/*COMENTAR*/
	//printf("noAtual->folha %c\n\n", noAtual->folha);

	*P++;

	j =0;
	/*DESCENTES - ORDEM_IP*/
	while(j < ordem_ip){
		
		char Descendentes[4];
		Descendentes[3] = '\0';
		strncpy(Descendentes, P, 3);

		/*COMENTAR*/
		//printf("Descendentes %s\n", Descendentes);

		if(strcmp(Descendentes, "***")==0)
			noAtual->desc[j] = -1;
		else
			noAtual->desc[j] = atoi(Descendentes);

	 	/*COMENTAR*/
		//printf("noAtual->desc[j] %d\n\n", 	noAtual->desc[j]);

		for(int k = 0; k <3; k++)
			*P++;

		j++;
	}
	return noAtual;
}

void write_btree_ip(node_Btree_ip *salvar, int rrn){
		
	/*Nova PÁGINA*/	
	char Filho[tamanho_registro_ip+1];

	memset(Filho, 0, sizeof(Filho));

	/*Auxiliar para transformar, caso seja necessário, o número "nregistrosip" em um número de três bytes*/
	char nChaves[4];

	snprintf(nChaves, sizeof(nChaves), "%03d", salvar->num_chaves);
	
	strcat(Filho, nChaves);

	/*Auxiliar para transformar, caso seja necessário, o "rrn" em um número de quatro bytes*/ 
	char RRN[5];

	int i = 0;
	int Contador = 0;
	
	while(i < ordem_ip-1){
		if(salvar->chave[i].rrn == -1){
			Contador++;
		}
		else{
			strcat(Filho, salvar->chave[i].pk);
			snprintf(RRN, sizeof(RRN), "%04d", salvar->chave[i].rrn);
			strcat(Filho, RRN);	
		}
		i++;
	}

	/*Número de "#" que preciso para preencher o vetor*/
	for(int i = 0; i < Contador*14; i++){
		strcat(Filho, "#");
	}

	/*Adicionar se a página é FOLHA*/
	if(salvar->folha == 'N')
		strcat(Filho, "N");
	else	
		strcat(Filho, "F");

	// printf("%c\n", salvar->folha);

	i = 0;
	Contador = 0;
	while(i < ordem_ip){
		if(salvar->desc[i] == -1){
			Contador++;
		}
		else{
			/*RRN Índice Primário*/
			snprintf(RRN, sizeof(RRN), "%03d", salvar->desc[i]);
			strcat(Filho, RRN);	
		}
		i++;
	}
	/*Número de "*" que preciso para preencher o vetor*/ 
	for(int i = 0; i < Contador*3; i++){
		strcat(Filho, "*");
	}

	char *P = ARQUIVO_IP + ((rrn)*tamanho_registro_ip);

	strncpy(P, Filho, tamanho_registro_ip);

 }


void CriarNode_IP(node_Btree_ip *newNode){

	//node_Btree_ip * newNode = (node_Btree_ip*)malloc(sizeof(node_Btree_ip));
	
	newNode->num_chaves = 0;	
	
	int numChaves = ordem_ip-1;

	newNode->chave = (Chave_ip*)malloc(numChaves*sizeof(Chave_ip));

	for(int i = 0; i < numChaves; i++)
		newNode->chave[i].rrn = -1;

	/* Número máximo de descendentes: "ordem_ip"*/
	/* Número mínimo de descendentes: "[ordem_ip/2]" (exceto raiz e folhas)*/
	newNode->desc = (int*)malloc(ordem_ip*sizeof(int));

	for(int i = 0; i < ordem_ip; i++)
		newNode->desc[i] = -1;

	// strcpy(newNode->folha, 'F');
	newNode->folha = 'F';
	//printf("newNode->folha %c", newNode->folha);

}

node_Btree_is *read_btree_is(int RRN){

}

void write_btree_is(node_Btree_is *salvar, int rrn){


}

void CriarNode_IS(node_Btree_is *newNode){

}
/* A função splitNode recebe o RRN do NÓ(Página), a CHAVE e o RRN do FILHO DIREITO*/
Dados splitNode(int node, Chave_ip *Chave, int filhoDireito){

	/*COMENTAR*/
	// printf("Chave->pk %s\n", Chave->pk);
	// printf("Chave->rrn %d\n", Chave->rrn);
	// printf("node (RRN) %d\n", node);
	// printf("filhoDireito (RRN) %d\n", filhoDireito);

	//X
	node_Btree_ip *Node = read_btree_ip(node);
	
	int i = 0;
	i = Node->num_chaves-1;

	//Flag para CHAVE_ALOCADA - O valor ZERO indica FALSO - Linha 3 do PSEUDO CÓDIGO
	int flag = 0;

	//Y
	node_Btree_ip *newNode = (node_Btree_ip*)malloc(sizeof(node_Btree_ip));
	CriarNode_IP(newNode);

	newNode->folha = Node->folha;

	//Preciso utilizar uma função para pegar o 'piso'?
	newNode->num_chaves = (ordem_ip-1)/2;

	// j > 0 ou j >= 0 ?
	for(int j = newNode->num_chaves; j > 0; j--){
		
		if(!flag && strcmp(Chave->pk, Node->chave[j].pk)>0){
			
			strcpy(newNode->chave[j].pk, Chave->pk);
			newNode->chave[j].rrn = Chave->rrn;

			newNode->desc[j+1] = filhoDireito;

			flag = 1;

		}
		else{
			strcpy(newNode->chave[j].pk, Node->chave[j].pk);
			newNode->chave[j].rrn = Node->chave[j].rrn;

			newNode->desc[j+1] = Node->desc[j+1];

			i--;
		}
	}

	if(!flag){

		while(i >= 1 && strcmp(Chave->pk, Node->chave[i].pk) < 0){

			strcpy(Node->chave[i+1].pk, Node->chave[i].pk);
			Node->chave[i+1].rrn = Node->chave[i].rrn;

			Node->desc[i+2] = Node->desc[i+1];

			i--;
		}

		strcpy(Node->chave[i+1].pk, Chave->pk);
		Node->chave[i+1].rrn = Chave->rrn;

		Node->desc[i+2] = filhoDireito;

	}

	Dados Resultado;
	strcpy(Resultado.cPromovida.pk, Node->chave[ordem_ip/2].pk);
	Resultado.cPromovida.rrn = Node->chave[ordem_ip/2].rrn;


	Node->chave[ordem_ip/2].rrn = -1;


	newNode->desc[0] = Node->desc[(ordem_ip/2)+1];
	Node->num_chaves = ordem_ip/2;


	write_btree_ip(Node, node);
	write_btree_ip(newNode, nregistrosip);
	nregistrosip++;

	Resultado.fDireito = nregistrosip;
	return Resultado; 
	
}

//Inserir "Auxiliar" - Recebe um nó NÃO VAZIA, no caso recebemos o RRN para recuperarmos o nó com a função "read_btree_ip",
// e uma Chave(do tipo Chave_ip)
Dados InserirIP(int RRN, Chave_ip *Chave){

	/*COMENTAR*/
	//printf("Chave->pk %s\n", Chave->pk);
	//printf("Chave->rrn %d\n", Chave->rrn);
	
	node_Btree_ip *Node = read_btree_ip(RRN);
	
	int i = 0;

	if(Node->folha == 'F'){
		if(Node->num_chaves < ordem_ip-1){
			
			i = Node->num_chaves-1;
			
			//Comparamos a PrimaryKey da Chave com a do Node até encontrarmos o local correto para inserir
			while(i >= 0 && strcmp(Chave->pk, Node->chave[i].pk)< 0){ // && k < ci[x] 
				strcpy(Node->chave[i+1].pk, Node->chave[i].pk);
				Node->chave[i+1].rrn = Node->chave[i].rrn;
				i--;
			}
			//Insiro a CHAVE e incremento o NÚMERO de CHAVES
			strcpy(Node->chave[i+1].pk, Chave->pk);
			Node->chave[i+1].rrn = Chave->rrn;
			Node->num_chaves++;
			
			Dados Resultado;
			// O valor -1 neste caso representa o retorno NULL
			Resultado.cPromovida.rrn = -1;
			Resultado.fDireito = -1;

			write_btree_ip(Node, RRN);
			//return (NULL, NULL);
			return (Resultado);
		}
		else{
			//O valor -1 para o RRN do FILHO DIREITO na função splitNode é utilizado para representar NULL
			return splitNode(RRN, Chave, -1);
		}
	}
	else{
		
		//Caso a PÁGINA não esteja marcada como FOLHA, procuramos a FOLHA em que desejamos inserir */
		i = Node->num_chaves-1;

		while(i >= 0 && strcmp(Chave->pk, Node->chave[i].pk)< 0){
			i--;
		}
		i++;

		Dados Resultado = InserirIP(Node->desc[i], Chave);

		//Verificamos se a CHAVE PROMOVIDA é diferente de NULL, neste caso verificamos se o RRN não é -1.
		//Caso seja atribuimos suas informações a CHAVE.
		if(Resultado.cPromovida.rrn != -1){
			
			strcpy(Chave->pk, Resultado.cPromovida.pk);
			Chave->rrn = Resultado.cPromovida.rrn;

			if(Node->num_chaves < ordem_ip-1){
				
				i = Node->num_chaves-1;

				while(i>=0 && strcmp(Chave->pk, Node->chave[i].pk)< 0){
					
					strcpy(Node->chave[i+1].pk, Node->chave[i].pk);
					Node->chave[i+1].rrn = Node->chave[i].rrn;

					Node->desc[i+2] = Node->desc[i+1];

					i--;
				}

				strcpy(Node->chave[i].pk, Chave->pk);
				Node->chave[i].rrn = Chave->rrn;

				Node->desc[i+2] = Resultado.fDireito;

				Node->num_chaves++;

				Dados Resultado;
				// O valor -1 neste caso representa o retorno NULL
				Resultado.cPromovida.rrn = -1;
				Resultado.fDireito = -1;

				write_btree_ip(Node, RRN);
				//return (NULL, NULL);
				return (Resultado);

			}
			else{
				//O valor -1 para o RRN do FILHO DIREITO na função splitNode é utilizado para representar NULL
				return splitNode(RRN, Chave, -1);
						
			}
		}
		else{
			Dados Resultado;
			// O valor -1 neste caso representa o retorno NULL
			Resultado.cPromovida.rrn = -1;
			Resultado.fDireito = -1;

			write_btree_ip(Node, RRN);
			//return (NULL, NULL);
			return (Resultado);
		}
	}


}

void gerarChave(Produto * Novo){

	Novo->pk[0] = Novo->nome[0];
	Novo->pk[1] = Novo->nome[1];
	Novo->pk[2] = Novo->marca[0];
	Novo->pk[3] = Novo->marca[1];
	Novo->pk[4] = Novo->data[0];
	Novo->pk[5] = Novo->data[1];
	Novo->pk[6] = Novo->data[3];
	Novo->pk[7] = Novo->data[4];
	Novo->pk[8] = Novo->ano[0];
	Novo->pk[9] = Novo->ano[1]; 
	Novo->pk[10] = '\0';

}

 void cadastrar(Indice* iprimary, Indice* ibrand){

	//Código - NÃO é inserido pelo usuário 
	// char pk[TAM_PRIMARY_KEY];
	// gerarChave(novo);

	/*-----------------------*/

	/* Interação com o Usuário */
	Produto Novo;
	/* CAMPOS DE TAMANHO VARIÁVEL */
	
	//Nome do Produto ou Modelo
	// char Nome[TAM_NOME];
	scanf("%[^\n]s", Novo.nome);
	getchar();
	//Marca
	// char Marca[TAM_MARCA];
	scanf("%[^\n]s", Novo.marca);
	getchar();
	/*-----------------------*/

	/* CAMPOS DE TAMANHO FIXO */

	//Data de Registro
	// char Data[TAM_DATA];	/* DD/MM/AAAA */
	scanf("%[^\n]s", Novo.data);
	getchar();
	//Ano de Lançamento
	// char Ano[TAM_ANO];
	scanf("%[^\n]s", Novo.ano);
	getchar();
	//Preço-Base
	// char Preço[TAM_PRECO];
	scanf("%[^\n]s", Novo.preco);
	getchar();
	//Desconto
	// char Desconto[TAM_DESCONTO];
	scanf("%[^\n]s", Novo.desconto);
	getchar();
	//Categorias
	// char Categoria[TAM_CATEGORIA];
	scanf("%[^\n]s", Novo.categoria);
	getchar();
	/*-----------------------*/

	gerarChave(&Novo);

	//Verifica se o PRODUTO existe
	// if(Busca != NULL) {
			// printf(ERRO_PK_REPETIDA, Novo->pk);
			// return;
 	// }

	// else{
		
		nregistros++;

		//Registro Auxiliar
		char rAuxiliar[193]; //TAM_REGISTRO
		rAuxiliar[192] = '\0';

		sprintf(rAuxiliar, "%s@%s@%s@%s@%s@%s@%s@%s@", Novo.pk, Novo.nome, Novo.marca, Novo.data, Novo.ano, Novo.preco, Novo.desconto, Novo.categoria);

		//Precisamos obter o TAMANHO do REGISTRO AUXILIAR (rAuxiliar) para sabermos quantos "bytes" faltam para preencher totalmento o REGISTRO.
		int Tamanho = strlen(rAuxiliar);

		// printf("\nTamanho = %d\n", Tamanho);

		int i;
		//Preenchendo o REGISTRO por completo (192bytes)
		for(i = Tamanho; i < 192; i++)
			rAuxiliar[i] = '#';

		// printf("\nTamanho - Final = %d", strlen(rAuxiliar));
		// printf("\n Registro: %s \n", rAuxiliar);

		strcat(ARQUIVO, rAuxiliar);

		//int tRegistro = strlen(rAuxiliar);
		//printf("%d\n", tRegistro);
		
		//printf("%s\n", rAuxiliar);
		
		//printf("%s\n", ARQUIVO);
		

		/* Caso 1 - Insere na Raiz (Raiz == -1 Indica que a ARVORE-B está VAZIA) */
		if(iprimary->raiz == -1){
			node_Btree_ip * newNode_IP = (node_Btree_ip*)malloc(sizeof(node_Btree_ip));
			CriarNode_IP(newNode_IP);
			
			// node_Btree_is * newNode_IS = (node_Btree_is*)malloc(sizeof(node_Btree_is));
			// CriarNode_IS(newNode_IS);
			
			newNode_IP->num_chaves++;
			strcpy(newNode_IP->chave[0].pk, Novo.pk);
			newNode_IP->chave[0].rrn = nregistros-1;
			
			/*Indica que a ARVORE-B NÃO está mais VAZIA*/
			iprimary->raiz = 0;
			
			/*O número de registros já foi incrementado, então precisa preciso subtrair um */
			write_btree_ip(newNode_IP, 0);
			nregistrosip++;
			// write_btree_is(newNode_IS, 0);

		}
		// /*Caso 2 - A ARVORE-B NÃO está mais VAZIA*/
		else{
			Chave_ip Chave;
			strcpy(Chave.pk, Novo.pk);
			Chave.rrn = nregistros-1;

			/*COMENTAR*/
			// printf("Chave.pk %s\n", Chave.pk);
			// printf("Chave.rrn %d\n", Chave.rrn);

			//Qual RRN utilizar?
			Dados Resultado = InserirIP(iprimary->raiz, &Chave);

			/*COMENTAR*/
			// printf("Resultado.cPromovida->pk %s\n", Resultado.cPromovida->rrn);
			// printf("%d\n", Resultado.fDireito);

			//Verificamos se CHAVE PROMOVIDA é diferente de NULL, neste caso verificamos se o RRN é diferente de -1
			if(Resultado.cPromovida.rrn != -1){
				node_Btree_ip * newNode_IP = (node_Btree_ip*)malloc(sizeof(node_Btree_ip));
				CriarNode_IP(newNode_IP);

				newNode_IP->folha = 'N';

				newNode_IP->num_chaves = 1;

				
				strcpy(newNode_IP->chave[0].pk, Resultado.cPromovida.pk);
				newNode_IP->chave[0].rrn = Resultado.cPromovida.rrn;

				/*?*/
				newNode_IP->desc[0] = iprimary->raiz;

				newNode_IP->desc[1] = Resultado.fDireito;

				/*? - Linha 21 do PSEUDO CÓDIGO*/
				iprimary->raiz = nregistrosip;

				write_btree_ip(newNode_IP, nregistrosip);
				nregistrosip++;
			}
		}
	// }
	
}

//  int alterar(Indice iprimary){

//  }
 
//  void buscar(Indice iprimary,Indice ibrand){

//  }
 
//  void listar(Indice iprimary,Indice ibrand){

//  }