#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>

#include "disk_emu.h"
#include "i_node.h"
#include "structures.h"

int maxFileNameSize = 20;
struct directory_entry root_directory[96];
unsigned char free_bit_map[1024];
struct superblock super;

void write_initial_structures(){
    
    for(int i =0;i<8;i++){
        free_bit_map[i]=0;
    }
    for(int i=8;i<1023;i++){
        free_bit_map[i]=1;
    }
    free_bit_map[1023] = 0;
    struct directory_entry** ptr = (struct directory_entry**) malloc(sizeof(struct directory_entry*));
    for(int i = 0;i<96;i++){
        
        (*ptr) = &root_directory[i];
        (*ptr)->empty = 1;
        }
    
    
    void* buffer = &root_directory;
    write_blocks(7,1,buffer);
    buffer = &free_bit_map;
    write_blocks(1023,1,buffer);
    free(ptr);
}

int find_free_block(){
    for(int i = 8;i<1024;i++){
        if(free_bit_map[i]==1){
            free_bit_map[i]=0;
            return i;
        }
    }
    return -1;
}
void free_block(int i){
    free_bit_map[i] = 1;
}




// searches for an empty spot in the directory and changes the directory entry to the file name, and the i_node_index
int put_file_name_in_directory(char* name){
    struct directory_entry** ptr = (struct directory_entry**) malloc(sizeof(struct directory_entry*));
    for(int i = 0;i<96;i++){       
        (*ptr) = &root_directory[i];
        if((*ptr)->empty== 1){
            (*ptr)->i_node_index = initialize_i_node();
            strcpy((*ptr)->file_name, name);
            if((*ptr)->i_node_index == -1){
                printf("i_node_table full\n");
                free(ptr);
                return -1;
            }
            (*ptr)->empty=0;
            free(ptr);
            return i;
        }
    }
    free(ptr);
    return -1;
}
//returns index of old directory entry if exists.
int file_name_exists(char* name){
    struct directory_entry** ptr = (struct directory_entry**) malloc(sizeof(struct directory_entry*));
    for(int i = 0;i<96;i++){
        (*ptr) = &root_directory[i];
        if((*ptr)->empty==0){
            if(strcmp((*ptr)->file_name,name)==0){
                free(ptr);
                return i;
            }
        }
    }
    free(ptr);
    return -1;
}

// returns the index of a spot already taken by the name.
int directory_i_node_index(char* name){
    for(int i = 0;i<96;i++){
        struct directory_entry** ptr = (struct directory_entry**) malloc(sizeof(struct directory_entry*));
        (*ptr) = &root_directory[i];
        if(strcmp(((*ptr)->file_name),name)== 0){
            return ((*ptr)->i_node_index);
        }
    }
    return -1;
}


// read and write to memory structures
void memory_to_disk(void* buffer, unsigned long int size_of_struct, int index, int num_blocks){
    unsigned long int space_in_blocks = (unsigned long int) num_blocks*1024;
    char* block_buffer = calloc(space_in_blocks,1);

    memcpy(block_buffer,buffer,size_of_struct);
    write_blocks(index, num_blocks, block_buffer);
    free(block_buffer);
}

void disk_to_memory(void* buffer, unsigned long int size_of_struct, int index, int num_blocks){
    unsigned long int space_in_blocks = (unsigned long int) num_blocks*1024;
    char* block_buffer = calloc(space_in_blocks,1);

    read_blocks(index, num_blocks, block_buffer);
    memcpy(buffer,block_buffer,size_of_struct);
    free(block_buffer);
}