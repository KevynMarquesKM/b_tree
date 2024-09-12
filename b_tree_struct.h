#ifndef B_TREE_STRUCT_H
#define B_TREE_STRUCT_H

//Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//Cria a estrutura do tipo nó
struct nodeStruct{
    int * key;                      //Array de chaves
    struct nodeStruct ** next;      //Array de ponteiros para os filhos
    int key_num;                    //Número de chaves do nó
    bool leaf;                      //Indicador de nó folha
} nodeStruct;

//Tipo nó
typedef struct nodeStruct * node;

//Estrutura e tipo árvore B
typedef struct tree{
    int order;                      //Ordem da árvore
    int key_num;                    //Número de chaves na árvore
    int node_num;                   //Número de nós da árvore
    int height;                     //Altura da árvore
    int max_key;                    //Maior chave presente na árvore
    int min_key;                    //Menor chave presente na árvore
    node header;                    //Cabeçalho da árvore, isto é, a raiz
    bool asterisk;                  //Identificador do tipo de árvore
} tree;

#endif