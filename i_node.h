#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>

struct i_node{
    int mode;
    int avaliable;
    int size;
    int block_ptr[12];
    //include to 12 if this stratagy persists.
    int single_ptr;
};

extern struct i_node i_node_table[96];


void initialize_i_node_table();

int initialize_i_node();

struct i_node retrieve_i_node_given_index(int index);

void purge_i_node(int index);