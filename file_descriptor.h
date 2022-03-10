#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>

struct file_descriptor_entry{
    int read_pointer;
    int write_pointer;
    int i_node_index;
    int avaliable;
};

extern struct file_descriptor_entry file_descriptor_table[96];


void initialize_file_descriptor_table();
int insert_index(int index);
int serach_file_descriptor_by_index(int i_node_index);
int get_read_pointer(struct file_descriptor_entry fdt);
int get_write_pointer(struct file_descriptor_entry fdt);
int avaliable_spot_fdt();