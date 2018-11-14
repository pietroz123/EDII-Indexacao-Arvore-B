void imprime_prom_dir(PromDir atual) {
	printf("atual.chavePromovida: %s\n", atual.chavePromovida);
	printf("atual.filhoDireito: %d\n", atual.filhoDireito);
	printf("atual.rrnIp: %d\n", atual.rrnIp);
}


PromDir divide_no(int rrnNo, char *k, int rrnDireito) {

	
	node_Btree_ip *X = read_btree_ip(rrnNo);
	node_Btree_ip *filho_direito;
	if (rrnDireito != -1)
		filho_direito = read_btree_ip(rrnDireito);
	else
		filho_direito = criar_no_ip();
	

	
	int i = X->num_chaves-1;
	int chave_alocada = 0;

	node_Btree_ip *Y = criar_no_ip();
	Y->folha = X->folha;
	Y->num_chaves = floor((ordem_ip-1) / 2);


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


	char chave_promovida[TAM_PRIMARY_KEY];
	strcpy(chave_promovida, X->chave[(ordem_ip/2)].pk);		// Promove a chave mediana
	Y->desc[0] = X->desc[(ordem_ip/2)+1];					
	X->num_chaves = (ordem_ip / 2);							// O número de chaves é reduzido pela metade


	PromDir retorno;
	strcpy(retorno.chavePromovida, chave_promovida);
	retorno.rrnIp = X->chave[(ordem_ip/2)].rrn;
	retorno.filhoDireito = nregistrosip;

	// Zera os descendentes remanescentes
	if (X->num_chaves < ordem_ip) {
		for (int i = ordem_ip-1; i > X->num_chaves; i--)
			X->desc[i] = -1;
	}
	
	// Zera a posicao que contem a chave promovida
	memset(X->chave[(ordem_ip/2)].pk, 0, sizeof(X->chave[(ordem_ip/2)].pk));
	X->chave[(ordem_ip/2)].rrn = -1;
	

	write_btree_ip(X, rrnNo);
	write_btree_ip(Y, nregistrosip);
	nregistrosip++;

	return retorno;
}

PromDir insere_aux(int rrnNo, char *k) {

	int i;
	node_Btree_ip *X = read_btree_ip(rrnNo);
	
	if (X->folha == 'F') {
		if (X->num_chaves < ordem_ip-1) {
			i = X->num_chaves-1;

			while (i >= 0 && strcmp(k, X->chave[i].pk) < 0) {
				X->chave[i+1] = X->chave[i];
				i--;
			}
			strcpy(X->chave[i+1].pk, k);
			X->chave[i+1].rrn = nregistros;
			X->num_chaves++;

			PromDir r;  //?
			memset(r.chavePromovida, 0, sizeof(r.chavePromovida));  //?
			r.filhoDireito = -1;  //?

			write_btree_ip(X, rrnNo);

			return r; // return NULL, NULL  //?Como retornar NULL, NULL?
		} 
		else {
			return divide_no(rrnNo, k, -1);  //? return divide_no(X, k, NULL)
		} 	
	}
	else {
		i = X->num_chaves-1;
		while (i >= 0 && strcmp(k, X->chave[i].pk) < 0)
			i--;
		i++;

		PromDir atual = insere_aux(X->desc[i], k);

		if (strlen(atual.chavePromovida)) {	//? if (chave_promovida != NULL)
			strcpy(k, atual.chavePromovida);
			if (X->num_chaves < ordem_ip-1) {
				i = X->num_chaves-1;


				while (i >= 0 && strcmp(k, X->chave[i].pk) < 0) {
					X->chave[i+1] = X->chave[i];
					X->desc[i+2] = X->desc[i+1];
					i--;
				}


				strcpy(X->chave[i+1].pk, k);
				X->chave[i+1].rrn = atual.rrnIp;
				X->desc[i+2] = atual.filhoDireito;  //? fi+2[X] <-- filho_direito
				X->num_chaves++;  //? n[X] <-- x[X] + 1


				PromDir r;  //?
				memset(r.chavePromovida, 0, sizeof(r.chavePromovida));  //?
				r.filhoDireito = -1;  //?

				write_btree_ip(X, rrnNo);

				return r; // return NULL, NULL  //?Como retornar NULL, NULL?
			}
			else {
				// Não há espaço, portanto realizamos um split
				return divide_no(rrnNo, k, atual.filhoDireito);  //? return divide_no(X, k, filho_direito)
			}
		}
		else {
			PromDir r;  //?
			memset(r.chavePromovida, 0, sizeof(r.chavePromovida));  //?
			r.filhoDireito = -1;  //?
			return r; // return NULL, NULL  //?Como retornar NULL, NULL?
		}
	}


}


void insere(Indice *ip, char *k, int rrn) {


	if (ip->raiz == -1) {
		node_Btree_ip *X = criar_no_ip();
		X->folha = 'F';
		X->num_chaves = 1;
		strcpy(X->chave[0].pk, k);
		X->chave[0].rrn = nregistros;
		
		ip->raiz = 0;	//?	raiz[T] <-- X
		write_btree_ip(X, ip->raiz);
		nregistrosip++;
	}
	else {
		PromDir atual = insere_aux(ip->raiz, k);

		if (strlen(atual.chavePromovida)) {		//? if (chave_promovida != NULL)
			node_Btree_ip *X = criar_no_ip();
			X->folha = 'N';
			X->num_chaves = 1;
			strcpy(X->chave[0].pk, atual.chavePromovida);
			X->chave[0].rrn = atual.rrnIp;
			
			X->desc[0] = ip->raiz;
			X->desc[1] = atual.filhoDireito;  //? f2[X] <-- filho_direito
			
			ip->raiz = nregistrosip;	 //? raiz[T] <-- X
			write_btree_ip(X, nregistrosip);
			nregistrosip++;
		}
		else {
		}

	}


}