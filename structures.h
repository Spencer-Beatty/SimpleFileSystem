#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>

struct superblock{
    int magic;
    int block_size;
    int num_blocks;
    int i_node_length;
    int root_directory; //i node number where root directory is contained
};

struct directory_entry{
    char file_name[20];
    int i_node_index; //this is -1 if there is no inode assigned
    int empty; // 1 if empty, 0 if full;
};

struct pointer_block{
    int ptrs[1024/4];
};


//global variables
extern int maxFileNameSize;
extern struct superblock super;
extern struct directory_entry root_directory[96];
extern unsigned char free_bit_map[1024];

void free_block();
int find_free_block();
void write_initial_structures();
int put_file_name_in_directory(char* name);
int file_name_exists(char* name);
int directory_i_node_index(char* name);

//read and write structures to the memory
void memory_to_disk(void* buffer, unsigned long int size_of_struct, int index, int number_blocks);
void disk_to_memory(void* buffer, unsigned long int size_of_struct, int index, int number_blocks);