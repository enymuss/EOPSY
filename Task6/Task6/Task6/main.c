//
//  main.c
//  Task6
//
//  Created by Richard Szczerba on 05/06/2020.
//  Copyright © 2020 Richard Szczerba. All rights reserved.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void copy_read_write(int fd_from, int fd_to);
void copy_mmap(int fd_from, int fd_to);

int main(int argc, const char * argv[]) {
    // possible options for program
    int mflag = 0;
    int hflag = 0;
    const char* sourceFilename = NULL;
    const char* destFilename = NULL;
    int c;
    // get the options from command line
    while ((c = getopt (argc, argv, "hm:")) != -1) {
        switch (c) {
            case 'm':
                mflag = 1;
                sourceFilename = optarg;
                destFilename = argv[optind];
                break;
            case 'h':
                hflag = 1;
                break;
            case '?':
                if (optopt == 'm')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n",
                             optopt);
                return 1;
            default:
                abort ();
        }
    }
    // if no options, then get the fileNames
    if (mflag == 0 && hflag == 0) {
        if (argc == 3) {
            sourceFilename = argv[1];
            destFilename = argv[2];
        } else {
            printf("Ths program needs two arguments\n");
        }
    }
    
    // if -h or empty/incorrect commandline arguemnts;
    if (hflag || sourceFilename == NULL) {
        printf("copy [-m] <file_name> <new_file_name>\n");
        printf("copy: copy from file_name to new_file_name contents using read() and write()\n");
        printf("-m: use mmap and memcpy to copy file contents\n");
        printf("copy -h for help \n");
        return 0;
    }
    
    // debugging
    printf ("hflag = %d, mflag = %d, sourceFilename = %s, destFilename = %s\n", hflag, mflag, sourceFilename, destFilename);
    
    // create file descriptor for source file
    int fd_source = open(sourceFilename, O_RDONLY);
    if (fd_source < 0) {
        perror("Open source file failed");
        return -1;
    }
    
    // create file descriptor for destination file
    int fd_dest = open(destFilename, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd_dest < 0) {
        perror("Open destination file failed");
        close(fd_source);
        if (fd_dest >= 0)
            close(fd_dest);
        return -1;
    }
    
    //print ints of file descriptors.
    printf("fd_source: %d, fd_dest :%d \n", fd_source, fd_dest);
    
    if (mflag) {
        // if run with -m option, use mmap
        copy_mmap(fd_source,  fd_dest);
    } else {
        copy_read_write(fd_source, fd_dest);
    }
    
    return 0;
}

void copy_read_write(int fd_from, int fd_to) {
    printf("Copy read write\n");
    char buf[1024];
    ssize_t bytesRead;
    // walk through source file in steps of buf
    while ((void)(bytesRead = read(fd_from, buf, sizeof buf)), bytesRead > 0) {
        ssize_t bytesWritten;
        //write bytes to destination file
        bytesWritten = write(fd_to, buf, bytesRead);
        if (bytesWritten < 0) {
            perror("write() failed");
        }
    }
    // all bytes read
    if (bytesRead == 0) {
        if (close(fd_to) == -1) {
            fd_to = -1;
            perror("close fd_to failed");
        }
        if (close(fd_from) == -1) {
            fd_from = -1;
            perror("close fd_from failed");
        }
    }
}

void copy_mmap(int fd_from, int fd_to) {
    printf("Copy mmap\n");
    struct stat fd_stat;
    
    // get source file size
    if (fstat(fd_from, &fd_stat) != 0) {
        perror("file stat failed");
    }
    
    // if file is empty, nothing to copy
    if (fd_stat.st_size == 0) {
        exit(EXIT_SUCCESS);
    }
    
    char *data;
    char *dst;
    // place file in memory defined by system
    data = mmap(NULL, fd_stat.st_size, PROT_READ, MAP_PRIVATE, fd_from, 0);
    if (data == MAP_FAILED) {
        perror("mmap input data failed");
    }
    
    // resize dest file to fit with source file
    if (ftruncate(fd_to, fd_stat.st_size) == -1) {
        perror("File size change failed");
    }
    
    // map destination file to memory
    dst = mmap(NULL, fd_stat.st_size, PROT_WRITE, MAP_SHARED, fd_to, 0);
    if (dst == MAP_FAILED) {
        perror("mmap dst failed");
    }
    
    // copy chuck of memory of size st_size starting at data to dst
    memcpy(dst, data, fd_stat.st_size);
    
    // free unused memory
    munmap(data, fd_stat.st_size);
    munmap(dst, fd_stat.st_size);
}
