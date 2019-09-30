# Objetivo

Indexação com Árvores-B é um sistema de banco de dados totalmente funcional implementado em C que utiliza Árvores-B como estrutura de dados para armazenar e gerenciar os índices de produtos e marcas de uma loja de tecnologia, a qual foi dado o nome UFSBum.

Os dois índices criados foram:
 - **iprimary**: índice primário ( Árvore-B), contendo as chaves primárias e os RRNs dos registros no arquivo de dados.  
 - **ibrand**: índice secundário (Árvore-B), contendo a chave primária, a marca e o nome dos produtos.
> Mais detalhes sobre os índices no PDF de requisitos

# Comandos

Nesta seção serão descritos os comandos para utilização do sistema. É importante ressaltar que como na época foi utilizado um juíz online que não suportava manipulação de arquivos, todos os índices foram armazenados em uma strings globais. Ao iniciar o programa, o usuário poderia selecionar **1** caso quisesse criar as árvores à partir de uma string, ou **0** caso quisesse inserir os produtos manualmente. 

## Sobre os casos de teste

Os casos de teste sem arquivo inicial vão do 0 até o 10 e com arquivo inicial do 11 até o 20. Já os casos de teste do 21 ao 30 são um mix de testes com e sem arquivo, porém com um número significativamente maior de dados.

> Independente da escolha de inserir a partir de um arquivo inicial ou não, o usuário deveria escolher as ordens ***m*** e ***n*** da árvore.
> Por exemplo, para criar uma árvore-B de ordem 3 por 3, sem arquivo inicial:
> 0
> 3 3

## Inserção (comando 1)

Para inserir um produto no arquivo de dados, o usuário deveria digitar o **nome do produto ou modelo**, **marca**, **data de registro**, **ano de lançamento**, **preço-base**, **desconto** e **categorias**.

Exemplo de inserção:
````
1
GEFORCE GTX 1080 TI
MSI
23/09/2018
17
8752.82
010
PLACA DE VIDEO|HARDWARE|GAMER
````

## Alterar desconto do produto (comando 2)

O comando **2** permite a alteração do desconto de um produto à partir do fornecimento de sua chave primária.

Exemplo de alteração:
````
2
********************************ALTERAR*******************************
MOLO301217
045
OPERACAO REALIZADA COM SUCESSO!
````

## Buscar por código (comando 3-1)

Ao digitar a combinação "3 1" o usuário pode buscar um produto à partir da sua chave primária. Independente de o produto existir ou não, o método de busca imprime todos os nós no caminho percorrido pela busca.

Exemplo de busca:
````
3
********************************BUSCAR********************************
1
TEWA090116
Busca por TEWA090116. Nos percorridos:
MOLO301217
PRIN080918, TEWA090116

TEWA090116
TECLADO GAMER SEMI MECANICO ABNT2
WARRIOR
09/01/2018
16
0172.20
TECLADO GAMER PERIFERICO
````

## Buscar por marca e nome (comando 3-2)

Ao digitar a combinação "3 2" o usuário pode buscar um produto à partir da sua marca e nome. Independente de o produto existir ou não, o método de busca imprime todos os nós no caminho percorrido pela busca.

Exemplo de busca:
````
3
********************************BUSCAR********************************
2
ASUS
PLACA-MAE PARA INTEL LGA 1151 MATX
Busca por ASUS$PLACA-MAE PARA INTEL LGA 1151 MATX.
Nos percorridos:
INTEL$PROCESSADOR INTEL CORE I5-8400 8GE
CRUCIAL$MEMORIA RAM 8GB 2400MHZ DDR4
ASUS$PLACA-MAE PARA INTEL LGA 1151 MATX, CORSAIR$FONTE  600W 80 PLUS

PLAS081117
PLACA-MAE PARA INTEL LGA 1151 MATX
ASUS
08/11/2018
17
0423.42
PLACA MAE HARDWARE
````

## Listagem do índice primário (comando 4-1)

A combinação "4 1" permite a visualização do índice primário à partir de uma varredura em **pré-ordem** da árvore-B.

Exemplo de visualização:
````
4
********************************LISTAR********************************
1
1 - MOLO301217
2 - GEMS230917
3 - FOCO170912, FOEV280214
3 - MECR251216, MEKI090716
2 - PRIN080918
3 - PLAS081117
3 - TEWA090116
````

## Listagem do índice primário (comando 4-2)

A combinação "4 2" permite a visualização do índice secundário à partir de uma varredura **em-ordem** da árvore-B.

Exemplo de visualização:
````
4
********************************LISTAR********************************
2
ASUS---------------------------------------------- PLACA-MAE PARA INTEL LGA 1151 MATX----------------
CORSAIR------------------------------------------- FONTE  600W 80 PLUS-------------------------------
CRUCIAL------------------------------------------- MEMORIA RAM 8GB 2400MHZ DDR4----------------------
EVGA---------------------------------------------- FONTE 600W 80 PLUS--------------------------------
INTEL--------------------------------------------- PROCESSADOR INTEL CORE I5-8400 8GE----------------
KINGSTON HYPERX----------------------------------- MEMORIA RAM 8GB 2400MHZ DDR4----------------------
LOGITECH------------------------------------------ MOUSE GAMER 12000DPI------------------------------
MSI----------------------------------------------- GEOFORCE GTX 1080 TI------------------------------
WARRIOR------------------------------------------- TECLADO GAMER SEMI MECaNICO ABNT2-----------------
````

## Imprimir o arquivo de dados (comando 5)

Com esse comando é possível visualizar o arquivo de dados criado após a inserção dos produtos no sistema.

Exemplo de visualização:
````
5
********************************ARQUIVO*******************************
GEMS230917@GEFORCE GTX 1080 TI@MSI@23/09/2018@17@8752.82@010@PLACA DE VIDEO|HARDWARE|GAMER@####################################################################################################MOLO301217@MOUSE GAMER 12000DPI@LOGITECH@30/12/2017@17@249.29@020@MOUSE|GAMER|PERIFERICO@#######################################################################################################
````

## Imprimir índice primário (comando 6)

Com o comando **6** é possível visualizar o arquivo de índice primário (*iprimary*). Os primeiros 3 bytes de cada registro correspondem ao número de registros naquele nó da árvore.

Exemplo de visualização:
````
6
002FOCO1709120004FOEV2802140007F*********
001PLAS0811170008##############F*********
001GEMS2309170000##############N000003***
002MECR2512160005MEKI0907160006F*********
001TEWA0901160002##############F*********
001PRIN0809180003##############N001004***
001MOLO3012170001##############N002005***
````

## Imprimir índice secundário (comando 7)

Com o comando **7** é possível visualizar o arquivo de índice secundário (*ibrand*). Os primeiros 3 bytes de cada registro correspondem ao número de registros naquele nó da árvore.

Exemplo de visualização:
````
7
002PLAS081117ASUS$PLACA-MAE PARA INTEL LGA 1151 MATX##############################################################FOCO170912CORSAIR$FONTE  600W 80 PLUS##########################################################################F*********
001TEWA090116WARRIOR$TECLADO GAMER SEMI MECANICO ABNT2###########################################################################################################################################################################F*********
001MECR251216CRUCIAL$MEMORIA RAM 8GB 2400MHZ DDR4################################################################################################################################################################################N000004***
002MEKI090716KINGSTON HYPERX$MEMORIA RAM 8GB 2400MHZ DDR4#########################################################MOLO301217LOGITECH$MOUSE GAMER 12000DPI########################################################################F*********
001FOEV280214EVGA$FONTE 600W 80 PLUS#############################################################################################################################################################################################F*********
001GEMS230917MSI$GEOFORCE GTX 1080 TI############################################################################################################################################################################################N003001***
001PRIN080918INTEL$PROCESSADOR INTEL CORE I5-8400 8GE############################################################################################################################################################################N002005***
````