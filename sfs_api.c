#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include "disk_emu.h"
#include "structures.h"
#include "i_node.h"
#include "file_descriptor.h"

int block_size = 1024;
int num_blocks = 1024;
int i_node_length = 6; // blocks
int magic_number = 12345;

// the total size of each string is 20, 16 chars plus 3 extension + 1 .    

// we can write the file name in the directory and then retrive the index when we search for it
// this won't be particualry fast because we are just searching from start to finish.

void mksfs(int fresh){
    if(fresh ==0){//condition where a valid file system exists within the disk
        init_disk("file_system.txt", block_size, num_blocks);
    }else if(fresh==1){//construct new file system from scratch
        if(init_fresh_disk("file_system.txt", block_size, num_blocks)==-1){
            exit(-1);
        }
        //defining permanenant disk structures.   
        // the total size of each string is 20, 16 chars plus 3 extension + 1 .    
        // we can write the file name in the directory and then retrive the index when we search for it
        // this won't be particualry fast because we are just searching from start to finish.
        void* buffer = (void*) malloc(1024);
       
        //create a super block to be put in the first block of the fileSystem
        struct superblock** ptr = (struct superblock**) malloc(sizeof(struct superblock*));
        *ptr = &super;
        (*ptr)->magic = magic_number;
        (*ptr)->block_size = block_size;
        (*ptr)->num_blocks = num_blocks;
        (*ptr)->i_node_length = i_node_length;
        (*ptr)->root_directory = 7;
       
        //superblock writing;
        buffer = &super;
        write_blocks(0,1, buffer);
        //free_bit_map writing and root_directory;
        write_initial_structures();
        initialize_i_node_table();
        initialize_file_descriptor_table();

        //i_node_table

        //file_descriptor table
       
    }
} 


int sfs_getnextfilename(char *fname){
    return 0;
} // get the name of the next file in directory

int sfs_getfilesize(const char* path){
    return 0;
}  // get the size of the given file

int sfs_fopen(char *name){
    if(strlen(name)==0){
        return -1;
    }
    int x = 0,y = 0;
    while(name[x] != '.'){
        if(x >= maxFileNameSize || name[x] == '\0') return -1; 
        x++;
    }
    x++;
    while(name[x+y] != '\0'){
        if(y >= 3) return -1;
        y++;
    }
    //valid name
    int i = (file_name_exists( name)); 
    int lock=0;
    if(i == -1){
        //doesn' texist
        int directory_index = put_file_name_in_directory(name);
        if(directory_index==-1){
            return -1;
        }
        if(avaliable_spot_fdt()!=1){
            return -1;
        }
        i = directory_index;
        lock = 1;
    }
     struct directory_entry** ptr = (struct directory_entry**) malloc(sizeof(struct directory_entry*));
     (*ptr)=&root_directory[i];
     if(lock){

     }

     int file_index = serach_file_descriptor_by_index((*ptr)->i_node_index);
     if(file_index==-1){
         //not in table yet
         file_index = insert_index((*ptr)->i_node_index);
         if(file_index==-1){
             return -1;
         }
     }
    return file_index;

     


}  

int sfs_fclose(int fileID){
    struct file_descriptor_entry** ptr = (struct file_descriptor_entry**) malloc(sizeof(struct file_descriptor_entry*));
    (*ptr) = &file_descriptor_table[fileID];
    purge_i_node((*ptr)->i_node_index);
    (*ptr)->i_node_index = -1;
    (*ptr)->avaliable =1;
    (*ptr)->write_pointer = 0;
    (*ptr)->read_pointer = 0;
    free(ptr);
    return 0;
}  // closes the given file

int sfs_fwrite(int fileID,char *buf, int length){
     int bytes_written = 0;
     int bytes_to_be_written = length;
     int block_index;
     int offset;
     int write_pointer;
     int word_load;
     struct pointer_block indirect;
     int write_indirect_pointer;
    // Pointer to file_decriptor entry, gives access to i_node_index, write and read pointers.
    struct file_descriptor_entry** ptr = (struct file_descriptor_entry**) malloc(sizeof(struct file_descriptor_entry*));
    struct i_node** ptr2 = (struct i_node**) malloc(sizeof(struct i_node*));
    (*ptr) = &file_descriptor_table[fileID];// filedescriptor pointer
    (*ptr2)= &i_node_table[(*ptr)->i_node_index];//inode pointer
    if((*ptr)->avaliable == 1) return -1;//these are uninitialized
    if((*ptr2)->avaliable ==1) return -1;
    
    //----------------------------------------------
    //find out what block we should be writing in.
    //----------------------------------------------
    write_pointer = (*ptr)->write_pointer;
    block_index = write_pointer/block_size;
    offset = write_pointer%block_size;
    char* block_buffer = malloc(block_size);
    //Initialize I_node indirect if it is not there
    if((*ptr2)->single_ptr ==-1){//uninitialized
        //int block[block_size/4] = malloc(sizeof(block_size));
        (*ptr2)->single_ptr = find_free_block();
        //single pointer set up with a block;
    }else{
        read_blocks((*ptr2)->single_ptr,1,&indirect);
    }

    //loop through until all blocks are written one at a time
    while(bytes_to_be_written>0){

        //reset the buffer after each runthrough
        memset(block_buffer,'\0',1024);

        // reset block index and offset !!

        //set how much should be written into this block, depends on how much is left
        if(bytes_to_be_written>(block_size-offset)){
            word_load = block_size-offset;
        }else{
            word_load = bytes_to_be_written;
        }
        int temp_index;
        if(block_index>12){
            write_indirect_pointer = 1;
            block_index = block_index-12;
            struct pointer_block** ptr3 = (struct pointer_block**) malloc(sizeof(struct pointer_block*));
            (*ptr3) = &indirect;
            temp_index = (*ptr3)->ptrs[block_index];
            if(temp_index<=0){//uninitialized
                temp_index = find_free_block();//check if added or not
                (*ptr3)->ptrs[block_index]=temp_index;
            }else{
                read_blocks(temp_index,1,block_buffer);
            }
            free(ptr3);
        }else{
            temp_index = (*ptr2)->block_ptr[block_index];
            if(temp_index<0){//uninitialized
                temp_index = find_free_block();
                (*ptr2)->block_ptr[block_index] = temp_index;
            }else{
                read_blocks(temp_index,1,block_buffer);
            }
            
        }
        //at this point the buffer block_buffer is either filled with the information in the block or is new information.
        memcpy(block_buffer+offset,buf,word_load);
        write_blocks(temp_index,1,block_buffer);

        write_pointer += word_load;
        block_index = (int) write_pointer/block_size;
        offset = write_pointer%block_size;

        bytes_written +=word_load;
        bytes_to_be_written-=word_load;

        buf+= word_load;
    }
    (*ptr)->write_pointer = write_pointer;
    (*ptr2)->size = (*ptr)->write_pointer;
    
    if(write_indirect_pointer ==1){
        write_blocks((*ptr2)->single_ptr,1,&indirect);
    }else{
        free_block((*ptr2)->single_ptr);
        (*ptr2)->single_ptr = -1;
    }

    //void* buffer, unsigned long int size_of_struct, int index, int number_blocks);
    memory_to_disk(i_node_table, sizeof(i_node_table), 1, 6);
    memory_to_disk(free_bit_map, sizeof(free_bit_map), 1023, 1);
   
    
    free(ptr);
    free(ptr2);
    free(block_buffer);

    return bytes_written;
    
} 

int sfs_fread(int fileID,char *buf, int length){
     int bytes_read = 0;
     int bytes_to_be_read = length;
     int block_index;
     int offset;
     int read_pointer;
     int word_load;
     struct pointer_block indirect;
     //int write_indirect_pointer;
    // Pointer to file_decriptor entry, gives access to i_node_index, write and read pointers.
    struct file_descriptor_entry** ptr = (struct file_descriptor_entry**) malloc(sizeof(struct file_descriptor_entry*));
    struct i_node** ptr2 = (struct i_node**) malloc(sizeof(struct i_node*));
    (*ptr) = &file_descriptor_table[fileID];// filedescriptor pointer
    (*ptr2)= &i_node_table[(*ptr)->i_node_index];//inode pointer
    if((*ptr)->avaliable == 1) return -1;//these are uninitialized
    if((*ptr2)->avaliable ==1) return -1;

    
    read_pointer = (*ptr)->read_pointer;
    block_index = (int) read_pointer/block_size;
    offset = read_pointer%block_size;
    char* block_buffer = malloc(block_size);

    if(read_pointer+length>(*ptr2)->size){
        bytes_to_be_read = (*ptr2)->size - read_pointer;
    }else{
        bytes_to_be_read = length;
    }
    
    if(((*ptr2)->single_ptr != -1)){
       
        read_blocks((*ptr2)->single_ptr,1,&indirect);

    }
   //-----------------------------------------------
    //Loop starts here
   //-----------------------------------------------
    while(bytes_to_be_read>0){
        memset(block_buffer,'\0',1024);

        if((bytes_to_be_read > (block_size-offset))){
            word_load = block_size - offset;
        }else{
            word_load = bytes_to_be_read;
        }
        
        if(block_index >12){
            struct pointer_block** ptr3 = (struct pointer_block**) malloc(sizeof(struct pointer_block*));
            (*ptr3) = &indirect;
            read_blocks( (*ptr3)->ptrs[block_index-12],1,block_buffer);
            free(ptr3);
        }else{
            read_blocks((*ptr2)->block_ptr[block_index],1,block_buffer);
        }
        
        memcpy(buf, block_buffer+offset,word_load);
        
        read_pointer += word_load;
        bytes_read += word_load;
        bytes_to_be_read  -= word_load;

        buf += word_load;
        block_index = (int) read_pointer/block_size;
        offset = read_pointer%block_size;
    }
    buf -=bytes_read;
    (*ptr)->read_pointer = read_pointer;
    free(block_buffer);
    free(ptr);
    free(ptr2);
    return bytes_read;

    

}  // read characters from disk into buf

int sfs_fseek(int fileID, int loc, int write_read){
    struct file_descriptor_entry** ptr = (struct file_descriptor_entry**) malloc(sizeof(struct file_descriptor_entry*));
    (*ptr) = &file_descriptor_table[fileID];
    if((fileID<0)||(loc>=1032*1024)){
        return -1;
    }
    if(write_read == 0){
        (*ptr)->write_pointer = loc;
    }else{
        (*ptr)->read_pointer = loc;
    }
    return 0;
}  // seek to the location from beginning

int sfs_remove(char *file){
    int directory_index = file_name_exists(file);
    struct directory_entry** ptr = (struct directory_entry**) malloc(sizeof(struct directory_entry*));
    struct i_node** ptr2 = (struct i_node**) malloc(sizeof(struct i_node*));
    if(directory_index==-1){
        return -1;
    }
    (*ptr) = &root_directory[directory_index];
    (*ptr2) = &i_node_table[(*ptr)->i_node_index];
    //initial structurs and setting up empty blcok
    char* empty_block = malloc(1024);
    //next section is clearing out direct and then indirect pointers blocks
    memset(empty_block,'\0',1024);
    for(int i =0;i<12;i++){
        if((*ptr2)->block_ptr[i]!=-1){
            write_blocks((*ptr2)->block_ptr[i],1,empty_block); 
            free_block((*ptr2)->block_ptr[i]);
        }
    }
    if((*ptr2)->single_ptr != -1){
        struct pointer_block indirect;
        struct pointer_block** ptr3 = (struct pointer_block**) malloc(sizeof(struct pointer_block*));
        read_blocks((*ptr2)->single_ptr,1,&indirect);
        (*ptr3) = &indirect;
        for(int i = 0;i<1024/4;i++){
            if((*ptr3)->ptrs[i]!=0){
                if((*ptr3)->ptrs[i]>num_blocks){
                    return-1;
                }
                write_blocks((*ptr3)->ptrs[i],1,empty_block); 
                free_block((*ptr3)->ptrs[i]); 
            }
        }
        free(ptr3);
    }
    
    purge_i_node((*ptr)->i_node_index);
    
    strcpy((*ptr)->file_name,"");
    (*ptr)->empty=1;

    struct file_descriptor_entry** ptr3 = (struct file_descriptor_entry**) malloc(sizeof(struct file_descriptor_entry*));
    int file_descriptor = serach_file_descriptor_by_index((*ptr)->i_node_index);
    (*ptr3) = &file_descriptor_table[file_descriptor];
    if((*ptr3)->avaliable != 1){
        sfs_fclose((*ptr)->i_node_index);
    }
    (*ptr)->i_node_index = -1;

    memory_to_disk(i_node_table, sizeof(i_node_table), 1, 6);
    memory_to_disk(free_bit_map, sizeof(free_bit_map), 1023, 1);
    memory_to_disk(root_directory, sizeof(root_directory), 8, 1);
    
    
    free(ptr);
    free(ptr2);
    free(ptr3);
    free(empty_block);
    return 0;

    


} 

