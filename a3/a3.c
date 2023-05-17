#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/mman.h>

#define PIPE_NAME "RESP_PIPE_21546"
#define TESTER_PYPE "REQ_PIPE_21546"

int main(){
    int fd1 = -1;
    int fd2 = -1;
    
    if(mkfifo(PIPE_NAME,0600)!=0){
        perror("Error creating pipe!");
        return 1;
    }
    fd2 = open(TESTER_PYPE,O_RDONLY);
    if(fd2 == -1){
        perror("Could not open tester pipe!");
        return 1;
    }
    fd1 = open(PIPE_NAME,O_WRONLY);
    if(fd1 == -1){
        perror("Could not open my pipe!");
    }
    
    //write(fd1,&"07434f4e4e454354",8); //CONNECT
    char n=7;
    write(fd1,&n,sizeof(char));
    char *s = "CONNECT";
    for(int i=0;i<strlen(s);i++){
        write(fd1,&s[i],sizeof(char));
    }
    int shmFd ;
    int fdFiser = -1;
    unsigned int numberOfBytes=0;
    volatile char *sharedChar = NULL;
    while(1){
        char dimensiuneRequest;
        read(fd2, &dimensiuneRequest,sizeof(char));
        char string[250];
        for(int i=0;i<(int)dimensiuneRequest;i++){
            read(fd2,&string[i],sizeof(char));
        }
        string[(int)dimensiuneRequest] = '\0';
        if(strcmp(string,"ECHO")==0){
            char k=4;
            write(fd1,&k,sizeof(char));
            char*q = "ECHO";
            for(int i=0;i<strlen(q);i++){
                write(fd1,&q[i],sizeof(char));
            }
            unsigned int variant = 21546;
            char m=7;
            char *t = "VARIANT";
            write(fd1,&m,sizeof(char));
            for(int i=0;i<strlen(t);i++){
                write(fd1,&t[i],sizeof(char));
            }
            write(fd1,&variant,sizeof(unsigned int));
        }
        if(strcmp(string,"CREATE_SHM")==0){
            char *s = "CREATE_SHM";
            char nrShm = strlen(s);
            
            read(fd2,&numberOfBytes,sizeof(unsigned int));
            shmFd = shm_open("/SAuHGpX",O_CREAT | O_RDWR, 0644);
            if(shmFd < 0){
                write(fd1,&nrShm,sizeof(char));
                for(int i=0;i<nrShm;i++){
                    write(fd1,&s[i],sizeof(char));
                }
                char n=5;
                char *error = "ERROR";
                write(fd1,&n,sizeof(char));
                for(int i=0;i<n;i++){
                    write(fd1,&error[i],sizeof(char));
                }
            }
            ftruncate(shmFd,numberOfBytes);
            sharedChar = (volatile char *)mmap(0,numberOfBytes,PROT_READ|PROT_WRITE,MAP_SHARED,shmFd,0);
            if(sharedChar == (void*)-1){
                 write(fd1,&nrShm,sizeof(char));
                for(int i=0;i<nrShm;i++){
                    write(fd1,&s[i],sizeof(char));
                }
                char n=5;
                char *error = "ERROR";
                write(fd1,&n,sizeof(char));
                for(int i=0;i<n;i++){
                    write(fd1,&error[i],sizeof(char));
                }
            }
            write(fd1,&nrShm,sizeof(char));
            for(int i=0;i<nrShm;i++){
                write(fd1,&s[i],sizeof(char));
            }
            char n=7;
            char *success = "SUCCESS";
            write(fd1,&n,sizeof(char));
            for(int i=0;i<n;i++){
                write(fd1,&success[i],sizeof(char));
            }
        }
        if(strcmp(string,"WRITE_TO_SHM")==0){
            char *s = "WRITE_TO_SHM";
            char nrShm = strlen(s);
            write(fd1,&nrShm,sizeof(char));
            for(int i=0;i<nrShm;i++){
                write(fd1,&s[i],sizeof(char));
            }
            unsigned int offset = 0;
            unsigned int value =0 ;
            // char *t = NULL;
            read(fd2,&offset,sizeof(unsigned int));
            // read(fd2,&t[0],sizeof(char));
            // read(fd2,&t[1],sizeof(char));
            // read(fd2,&t[2],sizeof(char));
            // read(fd2,&t[3],sizeof(char));
            // printf("t[0] = %d\n",t[0]);
            // printf("t[1] = %d\n",t[1]);
            // printf("t[2] = %d\n",t[2]);
            // printf("t[3] = %d\n",t[3]);
            read(fd2,&value,sizeof(unsigned int));
            // t[4]='\0';
            if(offset >= 0 && (offset+sizeof(unsigned int))<=numberOfBytes){
                sharedChar[offset+3]=(value >> 24) & 0xFF;
                sharedChar[offset+2]=(value >> 16) & 0xFF;
                sharedChar[offset+1]=(value >> 8) & 0xFF;
                sharedChar[offset]=value & 0xFF;
                char n=7;
                char *success = "SUCCESS";
                write(fd1,&n,sizeof(char));
                for(int i=0;i<n;i++){
                    write(fd1,&success[i],sizeof(char));
                }
            }else{
                char n=5;
                char *error = "ERROR";
                write(fd1,&n,sizeof(char));
                for(int i=0;i<n;i++){
                    write(fd1,&error[i],sizeof(char));
                }
            }
        }
        if(strcmp(string,"MAP_FILE")==0){
            char *s = "MAP_FILE";
            char nrShm = strlen(s);
            write(fd1,&nrShm,sizeof(char));
            for(int i=0;i<nrShm;i++){
                write(fd1,&s[i],sizeof(char));
            }
            char n;
            char fileName[250];
            read(fd2,&n,sizeof(char));
            for(int i=0;i<(int)n;i++){
                read(fd2,&fileName[i],sizeof(char));
            }
            fdFiser = open(fileName,O_RDONLY);
            if(fdFiser == -1){
                char n=5;
                char *error = "ERROR";
                write(fd1,&n,sizeof(char));
                for(int i=0;i<n;i++){
                    write(fd1,&error[i],sizeof(char));
                }
            }
            int size = lseek(fdFiser,0,SEEK_END);
            lseek(fdFiser,0,SEEK_SET);
            sharedChar = (char*)mmap(NULL,size,PROT_READ,MAP_PRIVATE,fdFiser,0);
            n=7;
            char *success = "SUCCESS";
            write(fd1,&n,sizeof(char));
            for(int i=0;i<n;i++){
                write(fd1,&success[i],sizeof(char));
            }

        }
        if(strcmp(string,"EXIT")==0){
            munmap((void*)sharedChar,numberOfBytes);
            sharedChar = NULL;
            close(shmFd);
            close(fd2);
            close (fd1);
            return 0;
        }
    }
    
    return 0;
}