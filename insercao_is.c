PromDirIs divide_no_is(int rrnNo, Chave_is k, int rrnDireito) {

	
	node_Btree_is *X = read_btree_is(rrnNo);
	

	
	int i = X->num_chaves-1;
	int chave_alocada = 0;

	node_Btree_is *Y = criar_no_is();
	Y->folha = X->folha;
	Y->num_chaves = (ordem_ip-1) / 2;

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


	if (!chave_alocada) {
		while (i >= 0 && strcmp(k.pk, X->chave[i].pk) < 0) {
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
	retorno.filhoDireito = nregistrosip;

	// Zera os descendentes remanescentes
	if (X->num_chaves < ordem_ip) {
		for (int i = ordem_ip-1; i > X->num_chaves; i--)
			X->desc[i] = -1;
	}
	
	// Zera a posicao que contem a chave promovida
	memset(X->chave[(ordem_ip/2)].pk, 0, sizeof(X->chave[(ordem_ip/2)].pk));
	X->chave[(ordem_ip/2)].rrn = -1;
	



	write_btree_is(X, rrnNo);
	write_btree_is(Y, nregistrosip);
	// Podemos desalocar Y visto que já o escrevemos no ARQUIVO_IP
	libera_no_is(X);
	libera_no_is(Y);


	nregistrosip++;

	return retorno;
}

PromDirIs insere_aux_is(int rrnNo, Chave_is k) {

	int i;
	node_Btree_is *X = read_btree_is(rrnNo);
	
	if (X->folha == 'F') {
		if (X->num_chaves < ordem_ip-1) {
			i = X->num_chaves-1;

			while (i >= 0 && strcmp(k.pk, X->chave[i].pk) < 0) {
				X->chave[i+1] = X->chave[i];
				i--;
			}
			X->chave[i+1] = k;
			X->num_chaves++;

			PromDirIs r;
			memset(r.chave_promovida.pk, 0, sizeof(r.chave_promovida.pk));
			r.chave_promovida.rrn = -1;
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
		while (i >= 0 && strcmp(k.pk, X->chave[i].pk) < 0)
			i--;
		i++;

		PromDirIs atual = insere_aux_is(X->desc[i], k);

		if (strlen(atual.chave_promovida.pk)) {
			k = atual.chave_promovida;
			if (X->num_chaves < ordem_ip-1) {
				i = X->num_chaves-1;


				while (i >= 0 && strcmp(k.pk, X->chave[i].pk) < 0) {
					X->chave[i+1] = X->chave[i];
					X->desc[i+2] = X->desc[i+1];
					i--;
				}


				X->chave[i+1] = k;
				X->desc[i+2] = atual.filhoDireito;
				X->num_chaves++;


				PromDirIs r;
				memset(r.chave_promovida.pk, 0, sizeof(r.chave_promovida.pk));
				r.chave_promovida.rrn = -1;
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
			r.chave_promovida.rrn = -1;
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

		nregistrosip++;
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
			
			ip->raiz = nregistrosip;

			write_btree_is(X, nregistrosip);
			libera_no_is(X);

			nregistrosip++;
		}
		else {
		}

	}


}