//Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "b_tree_basic.h"
#include "b_tree_removal_extension.h"
#include "b_tree_file_extension.h"

int main(){
    //Variáveis locais
    tree my_tree, file_tree;
    int i, random_key, result, num_keys;
    const char * filename = "b_tree.dat"; // Nome do arquivo para salvar e carregar a árvore

    //Criação da árvore com ordem 4
    create_tree(&my_tree, 6, true);

    //Criando um vetor de números para inserir nas árvores
    int keys_to_insert[] = {
    23, 56, 12, 234, 145, 76, 98, 45, 67, 89,
    111, 204, 78, 15, 190, 267, 31, 299, 22, 154,
    89, 277, 68, 123, 299, 110, 235, 50, 77, 43,
    185, 203, 8, 176, 255, 88, 165, 48, 140, 192,
    56, 64, 91, 182, 5, 250, 127, 36, 14, 199,
    232, 144, 120, 190, 87, 9, 218, 66, 35, 180,
    104, 22, 246, 129, 82, 47, 95, 240, 170, 300,
    26, 41, 60, 130, 208, 140, 19, 239, 21, 29,
    78, 204, 11, 189, 137, 250, 93, 65, 154, 218,
    200, 103, 159, 30, 189, 202, 16, 28, 101, 177};
    num_keys = sizeof(keys_to_insert) / sizeof(keys_to_insert[0]);

    //Inserindo chaves na árvore
    printf("\nInserting keys into tree:\n");
    for(i = 0; i < num_keys; i++){
        printf("\nInserting key: %d\n", keys_to_insert[i]);
        insert_key(&my_tree, keys_to_insert[i]);
    }

    //Desenhando a árvore no terminal
    print_tree_graphically(my_tree.header);

    //Imprimindo os metadados da árvore no terminal
    print_tree_metadata(my_tree);

    //folhas


    //Removendo algumas chaves
    remove_key(&my_tree, 137);
    remove_key(&my_tree, 177);
    remove_key(&my_tree, 76); 
    remove_key(&my_tree, 77);
    remove_key(&my_tree, 68);

    //Desenhando a árvore no terminal
    print_tree_graphically(my_tree.header);

    //Imprimindo os metadados da árvore no terminal
    print_tree_metadata(my_tree);

    //Salva a árvore em um arquivo
    save_tree_to_file(filename, &my_tree);

    //Cria uma nova árvore e carrega os dados do arquivo
    create_tree(&file_tree, my_tree.order, false);

    //Exibindo os metadados da árvore recém criada
    printf("\nNew tree info:\n");
    print_tree_metadata(file_tree);

    //Carregando a árvore recem criada com dados da outra árvore
    load_tree_from_file(filename, &file_tree);

    //Imprimindo os metadados da árvore recém criada novamente
    printf("\nNew tree info after load from file:\n");
    print_tree_metadata(file_tree);

    //Finalização do programa
    return 0;
}
