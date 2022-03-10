extern struct superblock super;
void mksfs(int fresh); // creates the file system

int sfs_getnextfilename(char *fname); // get the name of the next file in directory

int sfs_getfilesize(const char* path); // get the size of the given file

int sfs_fopen(char *name); // opens the given file

int sfs_fclose(int fileID); // closes the given file

int sfs_fwrite(int fileID,char *buf, int length); // write buf characters into disk

int sfs_fread(int fileID,char *buf, int length); // read characters from disk into buf

int sfs_fseek(int fileID, int loc, int read_write); // seek to the location from beginning 1 for read 0 for write

int sfs_remove(char *file); // removes a file from the filesystem