#ifndef B_TREE_REMOVAL_EXTENSION_H
#define B_TREE_REMOVAL_EXTENSION_H

//Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <C:\Users\UFSCar\Documents\Disciplinas\ORI\t\b_tree_struct.h>

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
void remove_key(tree *tree, int key){
    //Variáveis locais
    node root = tree->header;
    int idx;

    //Verifica se a árvore está vazia
    if(root == NULL){
        printf("A árvore está vazia.\n");
        return;
    }

    //Remove a chave da raiz
    remove_from_node(root, key, tree->order, &tree->node_num, &tree->key_num);

    //Ajusta a raiz se necessário
    if(root->key_num == 0){
        if(root->leaf){
            //Se a raiz é uma folha e não contém chaves, remove a raiz
            free(root->key);
            free(root->next);
            free(root);
            tree->header = NULL;
        } else {
            //Se a raiz tem filhos, faz o primeiro filho se tornar a nova raiz
            node old_root = root;
            tree->header = root->next[0];
            free(old_root->key);
            free(old_root->next);
            free(old_root);
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
    int idx = 0;

    //Encontra a posição da chave no nó
    while (idx < node->key_num && node->key[idx] < key){
        idx++;
    }

    //Verifica se a chave foi encontrada
    if(idx < node->key_num && node->key[idx] == key){
        if(node->leaf){
            //Remove a chave de um nó folha
            remove_from_leaf(node, idx, key_num);
        } else {
            //Remove a chave de um nó não folha
            remove_from_non_leaf(node, idx, order, node_num, key_num);
        }
    } else {
        if(node->leaf){
            printf("Chave não encontrada na árvore.\n");
            return;
        }

        //Verifica se o filho onde a chave deveria estar tem poucas chaves
        if(node->next[idx]->key_num < (order - 1) / 2){
            fill_node(node, idx, order, node_num);
        }

        //Se o nó filho onde a chave deveria estar foi ajustado, realiza a remoção
        if(node->next[idx]->key_num < (order - 1) / 2){
            printf("Erro: o nó filho ainda não possui chaves suficientes após o ajuste.\n");
        }

        //Realiza a remoção do nó filho
        remove_from_node(node->next[idx], key, order, node_num, key_num);
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
    for (int i = idx + 1; i < node->key_num; i++){
        node->key[i - 1] = node->key[i];
    }

    *key_num = *key_num - 1;
    node->key_num--;
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
    int key = node->key[idx];
    
    if(node->next[idx]->key_num >= (order - 1) / 2){
        int predecessor = get_predecessor(node->next[idx], idx);
        node->key[idx] = predecessor;
        remove_from_node(node->next[idx], predecessor, order, node_num, key_num);
    } else if(node->next[idx + 1]->key_num >= (order - 1) / 2){
        int successor = get_successor(node->next[idx + 1], idx);
        node->key[idx] = successor;
        remove_from_node(node->next[idx + 1], successor, order, node_num, key_num);
    } else {
        merge_nodes(node, idx, order, node_num);
        remove_from_node(node->next[idx], key, order, node_num, key_num);
    }
}

/*
Nome: get_predecessor
Objetivo: Obter o predecessor da chave de um nó
Parâmetros:
    node node - Nó que contém a chave
    int idx - Índice da chave para a qual o predecessor será obtido
Valor de retorno:
    int - O valor do predecessor
*/
int get_predecessor(node node, int idx){
    //Vai até o nó folha mais à direita da subárvore
    struct nodeStruct * current = node->next[idx];
    while (!current->leaf){
        current = current->next[current->key_num];
    }

    //Retorna a maior chave no nó folha
    return current->key[current->key_num - 1];
}

/*
Nome: get_successor
Objetivo: Obter o sucessor da chave de um nó
Parâmetros:
    node node - Nó que contém a chave
    int idx - Índice da chave para a qual o sucessor será obtido
Valor de retorno:
    int - O valor do sucessor
*/
int get_successor(node node, int idx){
    //Vai até o nó folha mais à esquerda da subárvore
    struct nodeStruct * current = node->next[idx + 1];
    while (!current->leaf){
        current = current->next[0];
    }

    //Retorna a menor chave no nó folha
    return current->key[0];
}

/*
Nome: fill_node
Objetivo: Preencher um nó que tem menos chaves do que deveria
Parâmetros:
    node node - Nó a ser preenchido
    int idx - Índice do filho a ser preenchido
    int order - Ordem da árvore B
Valor de retorno:
    void - Nenhum
*/
void fill_node(node node, int idx, int order, int * node_num){
    if(idx != 0 && node->next[idx - 1]->key_num >= (order - 1) / 2){
        borrow_from_prev(node, idx);
    } else if(idx != node->key_num && node->next[idx + 1]->key_num >= (order - 1) / 2){
        borrow_from_next(node, idx);
    } else {
        if(idx != node->key_num){
            merge_nodes(node, idx, order, node_num);
        } else {
            merge_nodes(node, idx - 1, order, node_num);
        }
    }
}

/*
Nome: borrow_from_prev
Objetivo: Emprestar uma chave do nó anterior
Parâmetros:
    node node - Nó que irá emprestar a chave
    int idx - Índice do filho que irá emprestar a chave
Valor de retorno:
    void - Nenhum
*/
void borrow_from_prev(node node, int idx){
    struct nodeStruct * child = node->next[idx];
    struct nodeStruct * sibling = node->next[idx - 1];

    //Move todas as chaves e filhos do filho para a direita
    for (int i = child->key_num - 1; i >= 0; i--){
        child->key[i + 1] = child->key[i];
    }

    if(!child->leaf){
        for (int i = child->key_num; i >= 0; i--){
            child->next[i + 1] = child->next[i];
        }
    }

    //Move a chave do nó pai para o filho
    child->key[0] = node->key[idx - 1];

    if(!child->leaf){
        child->next[0] = sibling->next[sibling->key_num];
    }

    node->key[idx - 1] = sibling->key[sibling->key_num - 1];

    child->key_num++;
    sibling->key_num--;
}

/*
Nome: borrow_from_next
Objetivo: Emprestar uma chave do próximo nó
Parâmetros:
    node node - Nó que irá emprestar a chave
    int idx - Índice do filho que irá emprestar a chave
Valor de retorno:
    void - Nenhum
*/
void borrow_from_next(node node, int idx){
    struct nodeStruct * child = node->next[idx];
    struct nodeStruct * sibling = node->next[idx + 1];

    //Move todas as chaves e filhos do filho para a esquerda
    child->key[child->key_num] = node->key[idx];

    if(!(child->leaf)){
        child->next[child->key_num + 1] = sibling->next[0];
    }

    node->key[idx] = sibling->key[0];

    for (int i = 1; i < sibling->key_num; i++){
        sibling->key[i - 1] = sibling->key[i];
    }

    if(!sibling->leaf){
        for (int i = 1; i <= sibling->key_num; i++){
            sibling->next[i - 1] = sibling->next[i];
        }
    }

    child->key_num++;
    sibling->key_num--;
}

/*
Nome: merge_nodes
Objetivo: Mesclar dois nós
Parâmetros:
    node node - Nó pai do nó que será mesclado
    int idx - Índice do filho que será mesclado
    int order - Ordem da árvore B
Valor de retorno:
    void - Nenhum
*/
void merge_nodes(node node, int idx, int order, int * node_num){
    struct nodeStruct * child = node->next[idx];
    struct nodeStruct * sibling = node->next[idx + 1];
    
    //Move a chave do nó pai para o filho
    child->key[(order - 1) / 2] = node->key[idx];

    //Move as chaves do irmão para o filho
    for (int i = 0; i < sibling->key_num; i++){
        child->key[i + (order - 1) / 2 + 1] = sibling->key[i];
    }

    //Move os ponteiros do irmão para o filho
    if(!(child->leaf)){
        for (int i = 0; i <= sibling->key_num; i++){
            child->next[i + sibling->key_num + 1] = sibling->next[i];
        }
    }

    //Move a quantidade de chaves do filho
    child->key_num += sibling->key_num + 1;

    //Move as chaves do nó pai e os ponteiros para a esquerda
    for (int i = idx + 1; i < node->key_num; i++){
        node->key[i - 1] = node->key[i];
        node->next[i] = node->next[i + 1];
    }

    *node_num = *node_num - 1;
    node->key_num--;

    //Libera a memória do irmão
    free(sibling->key);
    free(sibling->next);
    free(sibling);
}

#endif