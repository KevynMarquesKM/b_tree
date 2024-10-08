#ifndef B_TREE_BASIC_H
#define B_TREE_BASIC_H

//Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "b_tree_struct.h"

//Protótipos Básicos
node create_node(int, bool);
void create_tree(tree *, int, bool);
void split_node(node, int, int, int *);
void redistribution(node, int, int);
void insert_non_full(node, int, int, int *, bool);
void insert_key(tree *, int);
int search_key(node, int);
void print_spaces(int);
void print_spaces(int);
void print_tree_graphically(node);
void print_tree_graphically_recursive(node, int);
void print_tree_metadata(tree);

/*
Nome: create_node
Objetivo: Criar um nó
Parâmetros:
    int order - Ordem do nó a ser criado
    bool leaf - Identificador de nó folha
Valor de retorno:
    node - Nó criado
*/
node create_node(int order, bool leaf){
    //Variáveis locais
    node new_node;
    int i;

    //Aloca memória para o novo nó
    new_node = (node)malloc(sizeof(nodeStruct));

    //Verifica se a alocação foi bem sucedida
    if(new_node != NULL){
        new_node->key = (int *)malloc((order - 1) * sizeof(int));       //Aloca memória para as chaves
        new_node->next = (node *)malloc(order * sizeof(node));          //Aloca memória para os nós filhos
        //Verifica se a alocação foi bem sucedida
        if((new_node->key != NULL) && (new_node->next != NULL)){
            new_node->key_num = 0;                                      //O novo nó é inicializado com 0 chaves
            new_node->leaf = leaf;                                      //A identificação de nó folha segue o parâmetro passado
        }
        else{
            printf("ERROR: create_node - Memory allocation failure!\n");
            free(new_node->key);
            free(new_node->next);
            free(new_node);
        }
    }
    else{
        printf("ERROR: create_node - Memory allocation failure!\n");
        free(new_node);
    }

    //Retorna o novo nó alocado
    return new_node;
}

/*
Nome: create_tree
Objetivo: Inicializar uma árvore
Parâmetros: 
    tree * tree - Árvore a ser inicializada passada por referência
    int order - Ordem da árvore
Valor de retorno:
    void - Nenhum
*/
void create_tree(tree * tree, int order, bool asterisk){
    //Variáveis locais

    //Defini a ordem e tipo da árvore
    tree->order = order;
    tree->asterisk = asterisk;

    //Inicializa os metadados da árvore
    tree->key_num = 0;
    tree->node_num = 1;
    tree->height = 1;
    tree->max_key = 0;
    tree->min_key = 0;

    //Cria o primeiro nó da árvore
    tree->header = create_node(tree->order, true);
}

/*
Nome: split_node
Objetivo: Dividir um nó em dois
Parâmetros: 
    node parent - Nó que possui o nó filho que será dividido
    int i - Posição do vetor de ponteiros do nó passado como parâmetro correspondente ao nó filho a ser dividido
    int order - Ordem do nó a ser dividido
    int * node_num - Número de nós da árvore passado por referência para eventuais modificações
Valor de retorno:
    void - Nenhum
*/
void split_node(node parent, int i, int order, int * node_num){
    //Variáveis locais
    node aux, new_node;
    int j;

    //Incrementa o número de nós da árvore
    *node_num = *node_num + 1;

    //A variável auxiliar aponta para o nó filho a ser dividido
    aux = parent->next[i];

    //Cria e organiza o novo nó da divisão
    new_node = create_node(order, parent->next[i]->leaf); 
    new_node->key_num = ((order - 1)/2);                        //O novo nó terá metade das chaves do nó cheio
    for(j = 0; j < ((order - 1)/2); j++){                       //Copia a metade superior das chaves do nó cheio para o novo nó
        new_node->key[j] = aux->key[j + ((order + 1) / 2)];
    }

    //Se o nó filho dividido não é uma folha, ele possui filhos, portanto, também é necessário copiá-los para o novo nó
    if(!(aux->leaf)){
        for(j = 0; j <= ((order - 1)/2); j++){
            new_node->next[j] = aux->next[j + ((order + 1)/2)];
        }
    }

    //Corrige o número de chaves no nó dividido
    aux->key_num = (order - 1) / 2;

    //Corrige as posições dos ponteiros do nó pai para inserir o novo nó
    for(j = parent->key_num; j >= (i + 1); j--){
        parent->next[j+1] = parent->next[j];
    }
    //Insere o novo nó como filho do nó pai
    parent->next[i+1] = new_node;

    //Corrige as posições das chaves do nó pai para inserir a chave do meio do nó filho dividido
    for(j = parent->key_num - 1; j >= i; j--){
        parent->key[j+1] = parent->key[j];
    }
    parent->key[i] = aux->key[(order - 1)/2];

    //Incrementa o número de chaves no nó pai
    parent->key_num++;
}

/*
Nome: redistribution
Objetivo: Redistribui se possível chaves entre dois nós irmãos
Parâmetros: 
    node parent - Nó pai que contém os dois nós irmãos participantes da tentativa de reditribuição
    int i - Índice no vetor de ponteiros do nó pai para o nó onde a chave será inserida
    int order - Ordem da árvore
    int * node_num - Número de nós da árvore passado por referência para eventuais modificações
Valor de retorno:
    void - Nenhum
*/
void redistribution(node parent, int i, int order){
    //Variáveis locais
    node left, right;
    int j;

    //Atribui os nós direito e esquerdo que são irmãos
    left = parent->next[i];
    right = parent->next[i + 1];

    //Se o nó esquerdo tiver menos chaves que o direito, redistribui do direito para o esquerdo
    if(left->key_num < right->key_num){
        left->key[left->key_num] = parent->key[i];      //Move uma chave do nó pai para o nó esquerdo
        left->key_num++;                                //Atualiza o contador de chaves do nó esquerdo
        parent->key[i] = right->key[0];                 //Move a primeira chave do nó direito para o nó pai

        //Ajusta as chaves no nó direito
        for(j = 0; j < right->key_num - 1; j++){
            right->key[j] = right->key[j + 1];
        }

        //Se o nó direito não for folha, ajusta os ponteiros dos filhos
        if(!right->leaf){
            left->next[left->key_num] = right->next[0];
            for(j = 0; j < right->key_num; j++){
                right->next[j] = right->next[j + 1];
            }
        }

        //Atualiza o contador de chaves do nó direito
        right->key_num--;
    }
    //Se o nó direito tiver menos chaves que o esquerdo, redistribui do esquerdo para o direito
    else if(left->key_num > right->key_num){
        //Ajusta as chaves no nó direito para receber uma chave
        for(j = right->key_num; j > 0; j--){
            right->key[j] = right->key[j - 1];
        }
        right->key[0] = parent->key[i];                 //Move uma chave do nó pai para o nó direito
        right->key_num++;                               //Atualiza o contador de chaves do nó direito

        parent->key[i] = left->key[left->key_num - 1];  //Move a última chave do nó esquerdo para o nó pai
        left->key_num--;                                //Atualiza o contador de chaves do nó esquerdo

        //Se o nó esquerdo não for folha, ajusta os ponteiros dos filhos
        if(!left->leaf){
            for(j = right->key_num; j > 0; j--){
                right->next[j] = right->next[j - 1];
            }
            right->next[0] = left->next[left->key_num + 1];
        }
    }
}

/*
Nome: insert_non_full
Objetivo: Insere uma chave em um nó não cheio
Parâmetros: 
    node node - Nó a receber a chave a ser inserida
    int key - Chave a ser inserida
    int order - Ordem do nó a receber a chave a ser inserida
    int * node_num - Número de nós da árvore passado por referência para eventuais modificações
Valor de retorno:
    void - Nenhum
*/
void insert_non_full(node node, int key, int order, int * node_num, bool asterisk){
    //Variáveis locais
    int i;

    //Inicializa a variável que vai corresponder às posições da chave no nó
    i = node->key_num - 1;

    //Se o nó for uma folha
    if(node->leaf){
        //Encontra a posição onde a chave deve ser inserida
        while(i >= 0 && node->key[i] > key){
            node->key[i + 1] = node->key[i];
            i--;
        }

        //Insere a nova chave
        node->key[i + 1] = key;

        //Incrementa o número de chaves do nó
        node->key_num++;

        //Mensagem de inserção bem sucedida
        printf("%d inserted\n", key);
    }
    else{
        //Se o nó não é uma folha precisamos encontrar o filho adequado
        while(i >= 0 && node->key[i] > key){
            i--;
        }
    
        //Incrementa para selecionar o ponteiro direito do filho adequado encontrado
        i++;

        //Verifica se o filho onde a chave deve ser inserida está cheio
        if(node->next[i]->key_num == order - 1){
            //Se a árvore for do tipo B* tenta realizar uma redistribuição antes de dividir o nó
            if(asterisk){
                //Tenta redistribuir com o nó irmão esquerdo ou direito
                if(i > 0 && node->next[i - 1]->key_num < order - 1){
                    redistribution(node, i - 1, order);
                }
                else if(i < node->key_num && node->next[i + 1]->key_num < order - 1){
                    redistribution(node, i, order);
                }
            }

            //Se o nó continuar cheio, o divide
            if(node->next[i]->key_num == order - 1){
                //Divide o filho
                split_node(node, i, order, node_num);

                //Define em qual dos dois filhos da divisão a chave será inserida
                if(node->key[i] < key){
                    i++;
                }
            }
        }

        //Insere no filho adequado
        insert_non_full(node->next[i], key, order, node_num, asterisk);
    }
}

/*
Nome: insert_key
Objetivo: Inserir uma chave em uma árvore B
Parâmetros: 
    tree * tree -  Árvore a receber a chave
    int key - Chave a ser inserida na árvore
Valor de retorno:
    void - Nenhum
*/
void insert_key(tree * tree, int key){
    //Variaveis locais
    int verify;
    node aux, new_node;

    //Verifica se a chave já não existe na árvore
    if(search_key(tree->header, key)){
        printf("ERROR: insert_key - Key already inserted!\n");
    }
    else{
        //Nosso nó auxiliar aponta para o nó raiz da árvore
        aux = tree->header;

        //Caso onde a raiz está cheia
        if(aux->key_num == tree->order - 1){
            //Cria um novo nó para se tornar a nova raiz
            new_node = create_node(tree->order, false);

            //A nova raiz tem como filho a antiga raiz
            new_node->next[0] = aux;

            //Divide a antiga raiz, promove a sua chave do meio para a nova raiz
            split_node(new_node, 0, tree->order, &tree->node_num);

            //Insere a nova chave no nó adequado
            insert_non_full(new_node, key, tree->order, &tree->node_num, tree->asterisk);

            tree->header = new_node;            //Atualiza a árvore com a nova raiz
            tree->height++;                     //Aumenta a altura da árvore
            tree->node_num++;                   //Aumenta o número de nós na árvore
        }
        else{     
            //Obs:
            //Esse caso trata quando á raiz não está cheia e também quando a árvore está sem chaves
            
            //Insere na raiz
            insert_non_full(aux, key, tree->order, &tree->node_num, tree->asterisk);
        }

        //Atualiza a chave mínima e máxima da árvore se necessário e também incrementa seu número de chaves
        if(tree->key_num == 0 || key > tree->max_key){
            tree->max_key = key;
        }
        if(tree->key_num == 0 || key < tree->min_key){
            tree->min_key = key;
        }
        tree->key_num++;
    }
}

/*
Nome: search_key
Objetivo: Buscar uma chave em uma árvore B
Parâmetros: 
    node * root - Nó raiz da árvore a ser utilizada na busca
    int key - Chave a ser buscada na árvore
Valor de retorno:
    int - 0 se a busca for mal-sucedida e um valor inteiro correspondente à altura do nó da chave se a busca for bem sucedida
*/
int search_key(node root, int key){
    //Variáveis locais
    int height, i;

    //Inicializa a variável de iteração
    i = 0;

    //Encontra a primeira chave que é maior ou igual à chave buscada
    while((i < root->key_num) && (key > root->key[i])){
        i++;
    }

    //Se a chave é encontrada, retorna o nó
    if(root->key[i] == key){
        return 1;
    }
    else{
        if(root->leaf){                                     //Verifica se o nó é folha
            return 0;                                       //Retorna 0 (falso) para a busca
        }
        else{                                               //Se o nó não é folha
            return search_key(root->next[i], key) ? search_key(root->next[i], key) + 1: 0;      //Busca no filho correspondente e incrementa o nível a ser retornado
        }
    }
}

/*
Nome: print_spaces
Objetivo: Imprimir espaços de acordo com o nível da árvore
Parâmetros:
    int level - Nível da árvore
Valor de retorno:
    void - Nenhum
*/
void print_spaces(int level){
    //Variáveis locais
    int i;

    //Imprime os espaços
    for(i = 0; i < level; i++){
        printf("   ");
    }
}

/*
Nome: print_tree_graphically
Objetivo: Imprimir uma árvore de forma "gráfica" no terminal
Parâmetros:
    node root - Nó raiz da árvore a ser impressa
Valor de retorno:
    void - Nenhum
*/
void print_tree_graphically(node root){
    //Verifica se a árvore não é vazia
    if(root == NULL){
        printf("ERROR: print_tree_graphically - root equal NULL!:\n");
        return;
    }

    //Mensagem de execução 
    printf("\nTree drawing:\n");

    //Chamada da função recursiva para impressão da árvore
    print_tree_graphically_recursive(root, 0);
}

/*
Nome: print_tree_graphically_recursive
Objetivo: Imprimir uma árvore de forma "gráfica" no terminal
Parâmetros:
    node node - Nó da árvore a ser impressa
    int level - Nível atual da árvore para controlar a identação (permitindo recursão)
Valor de retorno:
    void - Nenhum
*/
void print_tree_graphically_recursive(node node, int level){
    //Variáveis locais
    int i;

    //Imprime o nó atual com indentação baseada no nível
    print_spaces(level);
    printf("|--");

    //Imprime as chaves do nó atual
    for(i = 0; i < node->key_num; i++){
        printf("%d ", node->key[i]);
    }
    printf("\n");                           //Apenas para embelezamento

    //Se o nó não é folha, imprime recursivamente os filhos
    if(!node->leaf){
        for(i = 0; i <= node->key_num; i++){
            print_tree_graphically_recursive(node->next[i], level + 1);
        }
    }
}

/*
Nome: print_tree_metadata
Objetivo: Imprimir as informações de cabeçalho de uma árvore
Parâmetros:
    tree tree - Árvore a ter seus dados impressos
Valor de retorno:
    void - Nenhum
*/
void print_tree_metadata(tree tree){
    printf("\nTree Data\n");
    printf("Order: %d\n", tree.order);
    printf("Number of Nodes: %d\n", tree.node_num);
    printf("Number of Keys: %d\n", tree.key_num);
    printf("Height: %d\n", tree.height);
    printf("Max Key: %d\n", tree.max_key);
    printf("Min Key: %d\n", tree.min_key);
}

#endif