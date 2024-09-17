#ifndef B_TREE_REMOVAL_EXTENSION_H
#define B_TREE_REMOVAL_EXTENSION_H

//Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "b_tree_struct.h"

//Protótipos de Remoção
void remove_key(tree *, int);
void remove_from_node(node, int, int, int *, int*);
void remove_from_leaf(node, int, int *);
void remove_from_non_leaf(node, int, int, int *, int*);
int get_predecessor(node, int);
int get_successor(node, int);
void fill_node(node, int, int, int *);
void borrow_from_prev(node, int);
void borrow_from_next(node, int);
void merge_nodes(node, int, int, int *);

/*
Nome: remove_key
Objetivo: Remover uma chave da árvore B
Parâmetros:
    tree *tree - Árvore da qual a chave será removida
    int key - Chave a ser removida
Valor de retorno:
    void - Nenhum
*/
void remove_key(tree * tree, int key){
    node root = tree->header;               //Referência a raiz da árvore
    int idx;                                //Índice auxilia para percorrer o nó

    //Verifica se a árvore está vazia
    if(root == NULL){                       //Verifica se a raiz é nula
        printf("A árvore está vazia.\n");
        return;                            //Sai da função se a árovre estiver vazia
    }

    //Verificando se a chave existe na árvore
    if(!(search_key(tree->header, key))){                    //Chama função para verificar se a chave está presente
        printf("\nErro: chave para remoção não existe\n");   //Erro se a chave não existir
    }
    else{
        //Remove a chave da raiz
        remove_from_node(root, key, tree->order, &tree->node_num, &tree->key_num);  //Remove a chave do nó raiz

        //Ajusta a raiz se necessário
        if(root->key_num == 0){                                                 //Se a raiz não tiver mais chaves
            if(root->leaf){                                                     //Se for um nó folha
                //Se a raiz é uma folha e não contém chaves, remove a raiz
                free(root->key);                                                //Libera a memória da chave
                free(root->next);                                               //Libera a mmória dos ponteiros
                free(root);                                                     //Libera o nó
                tree->header = NULL;                                            //Define a raiz como nula
            }else{
                //Se a raiz tem filhos, faz o primeiro filho se tornar a nova raiz

                node old_root = root;                                          //Guarda a raiz atual
                tree->header = root->next[0];                                  //Define o primeiro filho como a nova raiz
                free(old_root->key);                                           //Libera a memória da chave antiga
                free(old_root->next);                                          //Libera a memória dos ponteiros
                free(old_root);                                                //Libera o nó antigo
            }
        }
    }
}

/*
Nome: remove_from_node
Objetivo: Remover uma chave de um nó
Parâmetros:
    node node - Nó do qual a chave será removida
    int key - Chave a ser removida
    int order - Ordem da árvore B
    int * node_num - Número de chaves da árvore que pode sofrer modificação
    int * key_num - Número de nós da árvore que pode sofrer modificação
Valor de retorno:
    void - Nenhum
*/
void remove_from_node(node node, int key, int order, int * node_num, int * key_num){
    int idx = 0;                                               //Índice para buscar a chave no nó

    //Encontra a posição da chave no nó
    while (idx < node->key_num && node->key[idx] < key){       //Busca a chave ou a posição deveria estar 
        idx++;                                                 //Incrementa o índice até encontrar ou passar do valor da chave
    }

    //Verifica se a chave foi encontrada
    if(idx < node->key_num && node->key[idx] == key){          //Se encontrou a chave
        if(node->leaf){                                        //Se o nó é uma folha
            //Remove a chave de um nó folha
            remove_from_leaf(node, idx, key_num);              //Chama a função para remover de uma folha
        } else {
            //Remove a chave de um nó não folha
            remove_from_non_leaf(node, idx, order, node_num, key_num);   //Chama a função para remover de um nó não folha
        }
    } else {
        if(node->leaf){                                         //Se o nó é uma folha e a chave não foi encontrada
            printf("Chave não encontrada na árvore.\n");
            return;                                            //Sai da função
        }

        //Verifica se o filho onde a chave deveria estar tem poucas chaves
        if(node->next[idx]->key_num < (order - 1) / 2){       //Verifica se o filho precisa ser preenchido
            fill_node(node, idx, order, node_num);            //Preenche o nó se necessário
        }

        //Se o nó filho onde a chave deveria estar foi ajustado, realiza a remoção
        if(node->next[idx]->key_num < (order - 1) / 2){      //Se após o preenchimento ainda houver erro
            printf("Erro: o nó filho ainda não possui chaves suficientes após o ajuste.\n");
        }

        //Realiza a remoção do nó filho
        remove_from_node(node->next[idx], key, order, node_num, key_num); //Chama recursivamente para remover do nó filho
    }
}

/*
Nome: remove_from_leaf
Objetivo: Remover uma chave de um nó folha
Parâmetros:
    node node - Nó folha do qual a chave será removida
    int idx - Índice da chave a ser removida
Valor de retorno:
    void - Nenhum
*/
void remove_from_leaf(node node, int idx, int * key_num){
    //Remove a chave do nó folha
    for (int i = idx + 1; i < node->key_num; i++){  //Desloca as chaves subsequentes para a esquerda
        node->key[i - 1] = node->key[i];            //Move a chave para a posição anterior
    }

    *key_num = *key_num - 1;                        //Atualiza o número total de chaves na árvore
    node->key_num--;                                //Decrementa o número de chaves no nó
}

/*
Nome: remove_from_non_leaf
Objetivo: Remover uma chave de um nó não folha
Parâmetros:
    node node - Nó do qual a chave será removida
    int idx - Índice da chave a ser removida
    int order - Ordem da árvore B
Valor de retorno:
    void - Nenhum
*/
void remove_from_non_leaf(node node, int idx, int order, int * node_num, int * key_num){
    int key, predecessor;
    
    key = node->key[idx];                                                           //Armazena a chave a ser removida
    
    if(node->next[idx]->key_num >= (order - 1) / 2){                                //Verifica se o filho à esquerda tem chaves suficientes
        predecessor = get_predecessor(node, idx);                                   //Obtém o predecessor
        node->key[idx] = predecessor;                                               //Substitui a chave removida pelo predecessor
        remove_from_node(node->next[idx], predecessor, order, node_num, key_num);   //Remove o predecessor
    } else if(node->next[idx + 1]->key_num >= (order - 1) / 2){                     //Verifica se o filho à direita tem chaves suficientes
        int successor = get_successor(node, idx+1);                                 //Obtém o sucessor
        node->key[idx] = successor;                                                 //Substitui a chave removida pelo sucessor
        remove_from_node(node->next[idx + 1], successor, order, node_num, key_num); //Remove o sucessor
    } else{
        merge_nodes(node, idx, order, node_num);                                    //Mescla os dois nós
        remove_from_node(node->next[idx], key, order, node_num, key_num);           //Remove a chave após a fusão
    }
}

/*
Nome: get_predecessor
Objetivo: Obter o predecessor da chave de um nó
Parâmetros:
    node node - Nó que contém a chave
    int idx - Índice da chave para a qual obter o predecessor
Valor de retorno:
    int - Chave predecessora
*/
int get_predecessor(node node, int idx){
    node cur = node->next[idx];             //Vai para o filho à esquerda
    while (!cur->leaf){                     //Continua até encontrar um nó folha
        cur = cur->next[cur->key_num];      //Move-se para o próximo filho à direita
    }
    return cur->key[cur->key_num - 1];      //Retorna a última chave do nó folha
}

/*
Nome: get_successor
Objetivo: Obter o sucessor da chave de um nó
Parâmetros:
    node node - Nó que contém a chave
    int idx - Índice da chave para a qual obter o sucessor
Valor de retorno:
    int - Chave sucessora
*/
int get_successor(node node, int idx){
    node cur = node->next[idx + 1];  //Vai para o filho à direita
    while (!cur->leaf){              //Continua até encontrar um nó folha
        cur = cur->next[0];          //Move-se para o próximo filho à esquerda
    }
    return cur->key[0];              //Retorna a primeira chave do nó folha
}

/*
Nome: fill_node
Objetivo: Preencher um nó que tem menos chaves do que deveria
Parâmetros:
    node node - Nó que precisa ser preenchido
    int idx - Índice do nó filho que precisa ser ajustado
    int order - Ordem da árvore B
    int * node_num - Número de nós da árvore que pode sofrer modificação
Valor de retorno:
    void - Nenhum
*/
void fill_node(node node, int idx, int order, int * node_num){
    if(idx != 0 && node->next[idx - 1]->key_num >= (order - 1) / 2){                      //Se o nó anterior pode emprestar
        borrow_from_prev(node, idx);                                                      //Pega emprestado do nó anterior
    }else if(idx != node->key_num && node->next[idx + 1]->key_num >= (order - 1) / 2){    //Se o nó próximo pode emprestar
        borrow_from_next(node, idx);                                                      //Pega emprestado do próximo nó
    }else{
        if(idx != node->key_num){                                                         //Se não for o último nó
            merge_nodes(node, idx, order, node_num);                                      //Mescla com o próximo nó
        }else{ 
            merge_nodes(node, idx - 1, order, node_num);                                  //Mescla com o nó anterior
        }
    }
}

/*
Nome: borrow_from_prev
Objetivo: Pegar emprestado uma chave do nó anterior
Parâmetros:
    node node - Nó que precisa pegar emprestado
    int idx - Índice do nó filho que precisa do ajuste
Valor de retorno:
    void - Nenhum
*/
void borrow_from_prev(node node, int idx){
    node child = node->next[idx];         //Obtém o nó filho atual
    node sibling = node->next[idx - 1];   //Obtém o nó irmão anterior

    //Desloca as chaves e os ponteiros do filho para a direita
    for (int i = child->key_num - 1; i >= 0; --i){  //Move as chaves para a direita
        child->key[i + 1] = child->key[i];          //Desloca a chave
    }

    if (!child->leaf){  // Se o filho não é folha, move os ponteiros
        for (int i = child->key_num; i >= 0; --i){  //Move os ponteiros para a direita
            child->next[i + 1] = child->next[i];    //Desloca o ponteiro
        }
    }

    //Coloca a chave do nó pai no filho
    child->key[0] = node->key[idx - 1];  //A chave do pai passa para o nó filho

    if (!node->leaf){                                       //Se o nó pai não é folha
        child->next[0] = sibling->next[sibling->key_num];   //Atualiza o ponteiro do filho
    }

    //Move a última chave do irmão para o pai
    node->key[idx - 1] = sibling->key[sibling->key_num - 1];  //O irmão empresta sua última chave

    child->key_num++;     //Incrementa o número de chaves no filho
    sibling->key_num--;   //Decrementa o número de chaves no irmão
}

/*
Nome: borrow_from_next
Objetivo: Pegar emprestado uma chave do próximo nó
Parâmetros:
    node node - Nó que precisa pegar emprestado
    int idx - Índice do nó filho que precisa do ajuste
Valor de retorno:
    void - Nenhum
*/
void borrow_from_next(node node, int idx){
    node child = node->next[idx];                 //Obtém o nó filho atual
    node sibling = node->next[idx + 1];           //Obtém o nó irmão seguinte

    //Coloca a chave do pai no final do filho
    child->key[child->key_num] = node->key[idx];  //A chave do pai passa para o nó filho

    if (!child->leaf){                                       //Se o filho não é folha, move o ponteiro
        child->next[child->key_num + 1] = sibling->next[0];  //Atualiza o ponteiro do filho
    }

    //Move a primeira chave do irmão para o pai
    node->key[idx] = sibling->key[0];  //O irmão empresta sua primeira chave

    //Desloca as chaves do irmão para a esquerda
    for (int i = 1; i < sibling->key_num; ++i){  //Move as chaves do irmão
        sibling->key[i - 1] = sibling->key[i];   //Desloca a chave
    }

    if (!sibling->leaf){  //Se o irmão não é folha, move os ponteiros
        for (int i = 1; i <= sibling->key_num; ++i){  //Move os ponteiros do irmão
            sibling->next[i - 1] = sibling->next[i];  //Desloca o ponteiro
        }
    }

    child->key_num++;     //Incrementa o número de chaves no filho
    sibling->key_num--;   //Decrementa o número de chaves no irmão
}

/*
Nome: merge_nodes
Objetivo: Mesclar dois nós
Parâmetros:
    node node - Nó pai dos nós a serem mesclados
    int idx - Índice dos nós filhos a serem mesclados
    int order - Ordem da árvore B
Valor de retorno:
    void - Nenhum
*/
void merge_nodes(node node, int idx, int order, int * node_num){
    node child = node->next[idx];         //Obtém o nó filho atual
    node sibling = node->next[idx + 1];   //Obtém o nó irmão seguinte

    //Coloca a chave do pai no nó filho
    child->key[(order - 1) / 2] = node->key[idx];  //A chave do pai é inserida no meio do nó filho

    //Copia as chaves e ponteiros do irmão para o filho
    for (int i = 0; i < sibling->key_num; ++i){                  //Copia as chaves do irmão
        child->key[i + (order - 1) / 2 + 1] = sibling->key[i];  //Insere as chaves no nó filho
    }

    if (!child->leaf){  // Se o filho não é folha, copia os ponteiros
        for (int i = 0; i <= sibling->key_num; ++i){                  //Copia os ponteiros do irmão
            child->next[i + (order - 1) / 2 + 1] = sibling->next[i];  //Insere os ponteiros no nó filho
        }
    }

    //Desloca as chaves do pai para a esquerda
    for (int i = idx + 1; i < node->key_num; ++i){  //Move as chaves do pai
        node->key[i - 1] = node->key[i];            //Desloca a chave
    }

    //Desloca os ponteiros do pai para a esquerda
    for (int i = idx + 2; i <= node->key_num; ++i){  //Move os ponteiros do pai
        node->next[i - 1] = node->next[i];           //Desloca o ponteiro
    }

    //Atualiza o número de chaves nos nós
    child->key_num += sibling->key_num + 1;  //O filho agora tem todas as chaves do irmão e do pai
    node->key_num--;                         //O pai perde uma chave

    free(sibling->key);    //Libera a memória das chaves do irmão
    free(sibling->next);   //Libera a memória dos ponteiros do irmão
    free(sibling);         //Libera o nó irmão

    (*node_num)--;         //Decrementa o número total de nós na árvore
}

#endif 