#ifndef B_TREE_FILE_EXTENSION_H
#define B_TREE_FILE_EXTENSION_H

//Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "b_tree_struct.h"

//Protótipos de Arquivo/Disco
void save_node_to_file(FILE *, node, int);
void save_tree_to_file(const char *, tree *);
node load_node_from_file(FILE *, int);
void load_tree_from_file(const char *, tree *);

/*
Nome: save_node_to_file
Objetivo: Salvar um nó em um arquivo binário
Parâmetros:
    FILE * file - Arquivo a receber o nó
    node n - Nó a ser salvo no arquivo
    int order - Ordem do nó a ser inserido
Valor de retorno:
    void - Nenhum
*/
void save_node_to_file(FILE * file, node n, int order){
    //Variáveis locais
    int i;

    //verificando se o nó a ser salvo é válido
    if(n == NULL){
        return;
    }

    //Salvando o nó
    fwrite(&(n->key_num), sizeof(int), 1, file);
    fwrite(&(n->leaf), sizeof(bool), 1, file);
    fwrite(n->key, sizeof(int), n->key_num, file);
    if(!n->leaf){
        for(i = 0; i <= n->key_num; i++){
            save_node_to_file(file, n->next[i], order);
        }
    }
}

/*
Nome: save_node_to_file
Objetivo: Carregar um nó de um arquivo binário
Parâmetros:
    FILE * file - Arquivo que contém o nó
    int order - Ordem do nó a ser lido
Valor de retorno:
    node - Nó lido
*/
node load_node_from_file(FILE * file, int order){
    //Variáveis locais
    node n;
    int i;

    //Importando o nó do arquivo
    n = create_node(order, false);
    fread(&(n->key_num), sizeof(int), 1, file);
    fread(&(n->leaf), sizeof(bool), 1, file);
    n->key = (int *)realloc(n->key, (order - 1) * sizeof(int));
    fread(n->key, sizeof(int), n->key_num, file);
    if(!n->leaf){
        n->next = (node *)malloc((order) * sizeof(node));
        for(i = 0; i <= n->key_num; i++){
            n->next[i] = load_node_from_file(file, order);
        }
    }

    //Retornando o nó importado
    return n;
}

/*
Nome: save_tree_to_file
Objetivo: Salvar uma árvore em um arquivo binário
Parâmetros:
    const char * filename - Arquivo a receber a árvore
    tree * t - Árvore a ser salva no arquivo
Valor de retorno:
    void - Nenhum
*/
void save_tree_to_file(const char * filename, tree * t){
    //Variáveis locais
    FILE * file;
    
    //Abrindo o arquivo e verificando se a abertura foi bem sucedida
    file = fopen(filename, "wb");
    if(file == NULL){
        printf("ERROR: save_tree_to_file - fopen fail!\n");
        return;
    }

    //Salvando a árvore no arquivo
    fwrite(&(t->order), sizeof(int), 1, file);
    fwrite(&(t->key_num), sizeof(int), 1, file);
    fwrite(&(t->node_num), sizeof(int), 1, file);
    fwrite(&(t->height), sizeof(int), 1, file);
    fwrite(&(t->max_key), sizeof(int), 1, file);
    fwrite(&(t->min_key), sizeof(int), 1, file);
    save_node_to_file(file, t->header, t->order);

    //Fechando o arquivo
    fclose(file);
}

/*
Nome: load_tree_from_file
Objetivo: Carregar uma árvore de um arquivo binário
Parâmetros:
    const char *filename - Arquivo que contém a árvore
    tree * t - Ponteiro para árvore a receber os dados
Valor de retorno:
    void - Nenhum
*/
void load_tree_from_file(const char * filename, tree * t){
    //Variáveis locais
    FILE * file;

    //Abrindo o arquivo e verificando se a abertura foi bem sucedida
    file = fopen(filename, "rb");
    if(file == NULL){
        printf("ERROR: load_tree_from_file - fopen fail!\n");
        return;
    }

    //Importando a árvore do arquivo
    fread(&(t->order), sizeof(int), 1, file);
    fread(&(t->key_num), sizeof(int), 1, file);
    fread(&(t->node_num), sizeof(int), 1, file);
    fread(&(t->height), sizeof(int), 1, file);
    fread(&(t->max_key), sizeof(int), 1, file);
    fread(&(t->min_key), sizeof(int), 1, file);
    t->header = load_node_from_file(file, t->order);

    //Fechando o arquivo
    fclose(file);
}

#endif