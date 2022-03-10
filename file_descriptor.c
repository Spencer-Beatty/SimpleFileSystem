#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include "file_descriptor.h"

struct file_descriptor_entry file_descriptor_table[96];

void initialize_file_descriptor_table(){
    
    struct file_descriptor_entry** ptr = (struct file_descriptor_entry**) malloc(sizeof(struct file_descriptor_entry*));
    
    for(int i = 0;i<96;i++){
        (*ptr) = &file_descriptor_table[i];
        (*ptr)->avaliable = 1;
    }
    free(ptr);
}
int serach_file_descriptor_by_index(int i_node_index){
    struct file_descriptor_entry** ptr = (struct file_descriptor_entry**) malloc(sizeof(struct file_descriptor_entry*));
    for(int i =0;i<96;i++){
        (*ptr) = &file_descriptor_table[i];
        if((*ptr)->i_node_index == i_node_index){
            free(ptr);
            return i;
        }
    }
        return -1;
    
}

int insert_index(int i_node_index){
    struct file_descriptor_entry** ptr = (struct file_descriptor_entry**) malloc(sizeof(struct file_descriptor_entry*));
    for(int i = 0;i<96;i++){
        (*ptr) = &file_descriptor_table[i];
        if((*ptr)->avaliable){
            (*ptr)->avaliable = 0;
            (*ptr)->i_node_index = i_node_index;
            (*ptr)->read_pointer = 0;
            (*ptr)->write_pointer = 0;
            free(ptr);
            return i;
        }
    }
    free(ptr);
    return -1;
}
int avaliable_spot_fdt(){
  struct file_descriptor_entry** ptr = (struct file_descriptor_entry**) malloc(sizeof(struct file_descriptor_entry*));
    for(int i = 0;i<96;i++){
        (*ptr) = &file_descriptor_table[i];
        if((*ptr)->avaliable){
            return 1;
        }
    }
    return -1;
    }

int get_read_pointer(struct file_descriptor_entry fdt){
    struct file_descriptor_entry** ptr = (struct file_descriptor_entry**) malloc(sizeof(struct file_descriptor_entry*));
    (*ptr) = &fdt;
    int i = (*ptr)->read_pointer;
    return i;
}
int get_write_pointer(struct file_descriptor_entry fdt){
    struct file_descriptor_entry** ptr = (struct file_descriptor_entry**) malloc(sizeof(struct file_descriptor_entry*));
    (*ptr) = &fdt;
    int i = (*ptr)->write_pointer;
    return i;
}