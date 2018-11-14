void imprime_prom_dir(PromDir atual) {
	printf("atual.chavePromovida: %s\n", atual.chavePromovida);
	printf("atual.filhoDireito: %d\n", atual.filhoDireito);
	printf("atual.rrnD: %d\n", atual.rrnD);
}


PromDir divide_no(int rrnNo, char *k, int rrnDireito) {

	printf("rrnNo: %d\nrrnDireito: %d\n", rrnNo, rrnDireito);
	
	node_Btree_ip *X = read_btree_ip(rrnNo);
	node_Btree_ip *filho_direito;
	if (rrnDireito != -1)
		filho_direito = read_btree_ip(rrnDireito);
	else
		filho_direito = criar_no_ip();
	
	printf("X:\n");
	imprimir_node_ip(X);
	printf("filho_direito:\n");
	imprimir_node_ip(filho_direito);

	
	int i = X->num_chaves-1;
	int chave_alocada = 0;

	node_Btree_ip *Y = criar_no_ip();
	Y->folha = X->folha;
	Y->num_chaves = (ordem_ip-1) / 2;
	printf("Criou Y:\n");
	imprimir_node_ip(Y);


	for (int j = Y->num_chaves-1; j >= 0; j--) {
		if (!chave_alocada && strcmp(k, X->chave[i].pk) > 0) {
			strcpy(Y->chave[j].pk, k);
			Y->chave[j].rrn = nregistros;
			Y->desc[j+1] = rrnDireito;
			chave_alocada = 1; 
		}
		else {
			Y->chave[j] = X->chave[i];
			Y->desc[j+1] = X->desc[i+1];
			i--;
		}
	}

	printf("Apos MOVIMENTACAO:\n");
	printf("X:\n");
	imprimir_node_ip(X);
	printf("Y:\n");
	imprimir_node_ip(Y);

	if (!chave_alocada) {
		while (i >= 0 && strcmp(k, X->chave[i].pk) < 0) {
			X->chave[i+1] = X->chave[i];
			X->desc[i+2] = X->desc[i+1];
			i--;
		}
		strcpy(X->chave[i+1].pk, k);
		X->chave[i+1].rrn = nregistros;
		X->desc[i+2] = rrnDireito;
	}

	printf("Apos LIBERACAO:\n");
	printf("X:\n");
	imprimir_node_ip(X);
	printf("Y:\n");
	imprimir_node_ip(Y);

	char chave_promovida[TAM_PRIMARY_KEY];
	strcpy(chave_promovida, X->chave[(ordem_ip/2)].pk);		// Promove a chave mediana
	Y->desc[0] = X->desc[(ordem_ip/2)+1];					
	X->num_chaves = (ordem_ip / 2);							// O número de chaves é reduzido pela metade


	PromDir retorno;
	strcpy(retorno.chavePromovida, chave_promovida);
	retorno.rrnD = X->chave[(ordem_ip/2)].rrn;
	retorno.filhoDireito = nregistrosip;

	// Zera os descendentes remanescentes
	if (X->num_chaves < ordem_ip) {
		for (int i = ordem_ip-1; i > X->num_chaves; i--)
			X->desc[i] = -1;
	}
	
	// Zera a posicao que contem a chave promovida
	memset(X->chave[(ordem_ip/2)].pk, 0, sizeof(X->chave[(ordem_ip/2)].pk));
	X->chave[(ordem_ip/2)].rrn = -1;
	

	printf("Apos PROMOCAO:\n");
	printf("X:\n");
	imprimir_node_ip(X);
	printf("Y:\n");
	imprimir_node_ip(Y);


	write_btree_ip(X, rrnNo);
	write_btree_ip(Y, nregistrosip);
	char *p;
	if (!*ARQUIVO_IP)
		puts(ARQUIVO_VAZIO);
	else
		for (p = ARQUIVO_IP; *p != '\0'; p += tamanho_registro_ip)
		{
			fwrite(p, 1, tamanho_registro_ip, stdout);
			puts("");
		}
	nregistrosip++;

	return retorno;
}

PromDir insere_aux(int rrnNo, char *k) {

	int i;
	node_Btree_ip *X = read_btree_ip(rrnNo);
	imprimir_node_ip(X);
	
	if (X->folha == 'F') {
		printf("X eh folha\n");
		if (X->num_chaves < ordem_ip-1) {
			printf("Existe espaco\n");
			i = X->num_chaves-1;

			while (i >= 0 && strcmp(k, X->chave[i].pk) < 0) {
				X->chave[i+1] = X->chave[i];
				i--;
			}
			strcpy(X->chave[i+1].pk, k);
			X->chave[i+1].rrn = nregistros;
			X->num_chaves++;
			printf("Feita a alteracao em X\n");
			imprimir_node_ip(X);

			PromDir r;
			memset(r.chavePromovida, 0, sizeof(r.chavePromovida));
			r.filhoDireito = -1;
			imprime_prom_dir(r);

			write_btree_ip(X, rrnNo);

			return r; // return NULL, NULL
		} 
		else {
			printf("Nao existe espaco. Vai dividir\n");
			return divide_no(rrnNo, k, -1);
		} 	
	}
	else {
		printf("X nao eh folha\n");
		i = X->num_chaves-1;
		while (i >= 0 && strcmp(k, X->chave[i].pk) < 0)
			i--;
		i++;

		printf("Vai chamar insere_aux\n");
		PromDir atual = insere_aux(X->desc[i], k);
		printf("Chamou insere_aux\n");
		imprime_prom_dir(atual);

		if (strlen(atual.chavePromovida)) {
			printf("Houve overflow\n");
			strcpy(k, atual.chavePromovida);
			printf("Como esta X:\n");
			imprimir_node_ip(X);
			if (X->num_chaves < ordem_ip-1) {
				printf("Existe espaco\n");
				i = X->num_chaves-1;

				printf("X antes da movimentacao:\n");
				imprimir_node_ip(X);

				while (i >= 0 && strcmp(k, X->chave[i].pk) < 0) {
					X->chave[i+1] = X->chave[i];
					X->desc[i+2] = X->desc[i+1];
					i--;
				}

				printf("Como esta X apos movimentacao:\n");
				imprimir_node_ip(X);

				strcpy(X->chave[i+1].pk, k);
				X->chave[i+1].rrn = atual.rrnD;
				X->desc[i+2] = atual.filhoDireito;
				X->num_chaves++;

				printf("X apos insercao:\n");
				imprimir_node_ip(X);

				PromDir r;
				memset(r.chavePromovida, 0, sizeof(r.chavePromovida));
				r.filhoDireito = -1;
				printf("retorno:\n");
				imprime_prom_dir(r);

				write_btree_ip(X, rrnNo);

				return r; // return NULL, NULL
			}
			else {
				printf("Nao existe espaco. Vai dividir.\n");
				// Não há espaço, portanto realizamos um split
				return divide_no(rrnNo, k, atual.filhoDireito);
			}
		}
		else {
			printf("Nao houve overflow\n");
			PromDir r;
			memset(r.chavePromovida, 0, sizeof(r.chavePromovida));
			r.filhoDireito = -1;
			printf("retorno:\n");
			imprime_prom_dir(r);
			return r; // return NULL, NULL
		}
	}


}


void insere(Indice *ip, char *k, int rrn) {

	printf("------ Vai inserir '%s' ------\n", k);

	if (ip->raiz == -1) {
		printf("ip->raiz == -1\n");
		node_Btree_ip *X = criar_no_ip();
		X->folha = 'F';
		X->num_chaves = 1;
		strcpy(X->chave[0].pk, k);
		X->chave[0].rrn = nregistros;
		
		ip->raiz = 0;
		write_btree_ip(X, ip->raiz);
		nregistrosip++;
	}
	else {
		printf("ip->raiz != -1 => %d\n", ip->raiz);
		printf("Primeira chamada do insere_aux em insere\n");
		PromDir atual = insere_aux(ip->raiz, k);
		printf("Voltou para insere()\n");
		printf("Retorno:\n");
		imprime_prom_dir(atual);

		if (strlen(atual.chavePromovida)) {
			printf("Ocorreu overflow na raiz. Vai criar uma nova\n");
			node_Btree_ip *X = criar_no_ip();
			X->folha = 'N';
			X->num_chaves = 1;
			strcpy(X->chave[0].pk, atual.chavePromovida);
			X->chave[0].rrn = atual.rrnD;
			
			X->desc[0] = ip->raiz;
			X->desc[1] = atual.filhoDireito;
			
			ip->raiz = nregistrosip;
			write_btree_ip(X, nregistrosip);
			nregistrosip++;
		}
		else {
			printf("Nao ocorreu overflow\n");
		}

	}


}