#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
const int BUFFER_SIZE = 8192;

void copy_read_write(int fd_from, int fd_to);
void copy_mmap(int fd_from, int fd_to);
void help();

int main(int argc, char **argv){
	int opt = 0;
	int fd_from, fd_to;

	if (argc>2){
		fd_from = open (argv[argc-2], O_RDONLY);
    	if (fd_from == -1) {
           	printf("Failed to open source file\n");
           	return 1;
    	}
		fd_to = open(argv[argc-1], O_RDWR | O_CREAT, 0666);
    	if (fd_to == -1){
        	printf("Failed to open destination file\n");
        	return 1;
    	}
	}

	while ((opt = getopt(argc, argv, ":mh::")) != -1) {
		switch(opt){
			case 'h':
				help();
				return 0;
			case 'm':
				copy_mmap(fd_from, fd_to);
				return 0;
			default:
				printf("Wrong option. Check ./copy -h for more information\n");
				return 0;
		}
	}
	if (argc>2)
		copy_read_write(fd_from, fd_to);
	else
		help();

	return 0;
}

void copy_read_write(int fd_from, int fd_to){
	char buffer[BUFFER_SIZE];
    ssize_t size = 0;
	ssize_t filesize_from, filesize_out;
    //reading until we get -1 and writing to destination
    while((filesize_from = read (fd_from, &buffer, BUFFER_SIZE)) > 0){
            filesize_out = write (fd_to, &buffer, (ssize_t) filesize_from);
            if(filesize_out != filesize_from){
                printf("Write error occured\n");
                exit(0);
            }
            size+=filesize_from;
    }
    //change file size to proper one
	ftruncate(fd_to, size);
    //cleanup
    close(fd_from);
    close(fd_to);

    printf("File copying (r/w) has been successfully completed\n");
}

void copy_mmap(int fd_from, int fd_to){
	struct stat status;
	fstat(fd_from, &status);

	char* source;
	//map in virtual address space
	source=mmap(0, status.st_size, PROT_READ, MAP_SHARED, fd_from, 0);
	if (source==MAP_FAILED){
    	printf("Error mapping source: %d\n", errno);
    	exit(1);
	}

	char* destination;
	//change file size to proper one
	ftruncate(fd_to, status.st_size);
	//map in virtual address space
	destination=mmap(0, status.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_to, 0);
	if (destination == MAP_FAILED){
    	printf("Error mapping destination: %d\n", errno);
    	exit(1);
	}
    //copy
	memcpy(destination, source, status.st_size);
	//cleanup
	munmap(destination, status.st_size);
	munmap(source, status.st_size);

    //cleanup
    close(fd_from);
    close(fd_to);

    printf("File copying(mmap) has been successfully completed\n");
}

void help(){
	printf("\nThis program copies one file to another\n");
	printf("./copy -m <file_name> <new_file_name> - copy the files by memory mapping\n");
	printf("./copy <file_name> <new_file_name> - copy the files by reading and writing\n");
	printf("./copy -h - show help\n");
	exit(0);
}
