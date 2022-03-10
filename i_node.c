#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include "structures.h"
#include "i_node.h"
struct i_node i_node_table[96];


void initialize_i_node_table(){

    
    struct i_node** ptr = (struct i_node**) malloc(sizeof(struct i_node*));
    for(int i = 0;i<96;i++){
        (*ptr) = &i_node_table[i];
        (*ptr)->avaliable = 1;
        (*ptr)->single_ptr = -1;
    }
    (*ptr)= &i_node_table[0];
    (*ptr)->avaliable = 0;
    (*ptr)->block_ptr[0] = 7;
    free(ptr);
}

struct i_node retrieve_i_node_index(int index){
    //assumed that this will always be called on an initialized i_node;
    struct i_node ai_node = i_node_table[index];
    return ai_node;
}

int initialize_i_node(){
    struct i_node** ptr = (struct i_node**) malloc(sizeof(struct i_node*));
    for(int i = 0;i<96;i++){
       (*ptr) = &i_node_table[i];
        if((*ptr)->avaliable == 1){
            (*ptr)->avaliable = 0;
            free(ptr);
            return i;
        }
    }
    free(ptr);
    return -1;
}

void purge_i_node(int index){
    struct i_node** ptr = (struct i_node**) malloc(sizeof(struct i_node*));
    (*ptr) = &i_node_table[index];
    (*ptr)->avaliable = 1;
    for(int i = 0;i<12;i++){
        (*ptr)->block_ptr[i] = -1;
    }
    (*ptr)->size = 0;
    (*ptr)->single_ptr = -1;
    free(ptr);
}





