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
    char n=7;
    write(fd1,&n,sizeof(char));
    char *s = "CONNECT";
    for(int i=0;i<strlen(s);i++){
        write(fd1,&s[i],sizeof(char));
    }
    int shmFd ;
    int fdFiser = -1;
    int size = 0;
    unsigned int numberOfBytes=0;
    volatile char *sharedChar = NULL;
    volatile char *mapFisier = NULL;
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
            read(fd2,&offset,sizeof(unsigned int));
            read(fd2,&value,sizeof(unsigned int));
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
            char n=0;
            char fileName[250];
            read(fd2,&n,sizeof(char));
            for(int i=0;i<(int)n;i++){
                read(fd2,&fileName[i],sizeof(char));
            }
            fileName[(int)n]='\0';
            fdFiser = open(fileName,O_RDONLY);
            if(fdFiser == -1){
                char n=5;
                char *error = "ERROR";
                write(fd1,&n,sizeof(char));
                for(int i=0;i<n;i++){
                    write(fd1,&error[i],sizeof(char));
                }
            }
            size = lseek(fdFiser,0,SEEK_END);
            lseek(fdFiser,0,SEEK_SET);
            mapFisier = (char*)mmap(NULL,size,PROT_READ,MAP_PRIVATE,fdFiser,0);
            if(mapFisier == (void*)-1){
                char n=5;
                char *error = "ERROR";
                write(fd1,&n,sizeof(char));
                for(int i=0;i<n;i++){
                    write(fd1,&error[i],sizeof(char));
                }
            }
            n=7;
            char *success = "SUCCESS";
            write(fd1,&n,sizeof(char));
            for(int i=0;i<n;i++){
                write(fd1,&success[i],sizeof(char));
            }
        }
        if(strcmp(string,"READ_FROM_FILE_OFFSET")==0){
            char *s = "READ_FROM_FILE_OFFSET";
            char nrShm = strlen(s);
            write(fd1,&nrShm,sizeof(char));
            for(int i=0;i<nrShm;i++){
                write(fd1,&s[i],sizeof(char));
            }
            unsigned int offset = 0;
            unsigned int no_of_bytes = 0;
            read(fd2,&offset,sizeof(unsigned int));
            read(fd2,&no_of_bytes,sizeof(unsigned int));
            //char *value = NULL;
            if(offset+no_of_bytes >size){
                char n=5;
                char *error = "ERROR";
                write(fd1,&n,sizeof(char));
                for(int i=0;i<n;i++){
                    write(fd1,&error[i],sizeof(char));
                }
            }else{
                int i=0;
                while(i<no_of_bytes){
                    sharedChar[i] = mapFisier[offset+i];
                    i++;
                }
                char *success = "SUCCESS";
                write(fd1,&n,sizeof(char));
                for(int i=0;i<n;i++){
                    write(fd1,&success[i],sizeof(char));
                }

            }
        }
        if(strcmp(string,"READ_FROM_FILE_SECTION")==0){
            char *s = "READ_FROM_FILE_SECTION";
            char nrShm = strlen(s);
            write(fd1,&nrShm,sizeof(char));
            for(int i=0;i<nrShm;i++){
                write(fd1,&s[i],sizeof(char));
            }
            unsigned int section_no=0;
            unsigned int adresaCitire=0;
            unsigned int nrDeBytes=0;
            read(fd2,&section_no,sizeof(unsigned int));
            read(fd2,&adresaCitire,sizeof(unsigned int));
            read(fd2,&nrDeBytes,sizeof(unsigned int));
            // printf("offset  = %d\n ",offset);
            // printf("no_of_bytes = %d",nrDeBytes);
            // unsigned char *headerSizeChar= NULL;
            // headerSizeChar[0] = mapFisier[size-3];
            // headerSizeChar[1] = mapFisier[size-2];
            // headerSizeChar[2] = '\0';
            // printf("%c \n",headerSizeChar[0]);
            // printf("%d %d %d",size,mapFisier[size-2],mapFisier[size-3]);
            // unsigned int headerSize = (unsigned int)((mapFisier[size-3]<<8) + mapFisier[size-2]);
            // printf(" %d ",headerSize);
            off_t offset_size = size - 2;
            unsigned char *ptr = (unsigned char *)mapFisier + offset_size;
            unsigned char penultimate_byte = *(ptr - 1);
            unsigned char antepenultimate_byte = *(ptr - 2);
            unsigned int headerSize = (unsigned int)((antepenultimate_byte << 8) | penultimate_byte);
            //printf("%d\n",headerSize);
            unsigned char *index = (unsigned char *)mapFisier +size - headerSize+1;
            unsigned int no_of_sections = *(index);
            //printf("no of sections = %d\n",no_of_sections);
            index = (unsigned char *) index + 1;
            //printf("%c\n",*(index));
            if(section_no > no_of_sections){
                char n=5;
                char *error = "ERROR";
                write(fd1,&n,sizeof(char));
                for(int i=0;i<n;i++){
                    write(fd1,&error[i],sizeof(char));
                }
            }else{
                int i=1;
                while(i<section_no){
                    index = (unsigned char *) index + 24;
                    i++;
                    //printf("i = %d \n",i);
                }
                //printf("%d ",i);
                index = (unsigned char *) index + 16;
                //printf("%c\n",*(index));
                unsigned char sectOffset1 = *(index);
                unsigned char sectOffset2 = *(index +1);
                unsigned char sectOffset3 = *(index+ 2);
                unsigned char sectOffset4 = *(index+ 3);
                unsigned int sectOffset = (unsigned int)((sectOffset4<<24) | (sectOffset3<<16) | (sectOffset2<<8) |sectOffset1);
                //printf("%d\n",sectOffset);
                //unsigned char *pointerSectiune = (unsigned char*)mapFisier + sectOffset+offset;
                int j=0;
                // printf("offset  = %d\n ",offset);
                // printf("no_of_bytes = %d",no_of_bytes);
                while(j<=nrDeBytes){
                    //printf("%c",mapFisier[sectOffset+adresaCitire+j]);
                    sharedChar[j] = mapFisier[sectOffset+adresaCitire+j];
                    //printf("%c",sharedChar[j]);
                    j++;
                }
                //printf("%d\n",j);
                n=7;
                char *success = "SUCCESS";
                write(fd1,&n,sizeof(char));
                for(int i=0;i<n;i++){
                    write(fd1,&success[i],sizeof(char));
                }
            }
        }
        if(strcmp(string,"READ_FROM_LOGICAL_SPACE_OFFSET")==0){
            char *s = "READ_FROM_LOGICAL_SPACE_OFFSET";
            char nrShm = strlen(s);
            write(fd1,&nrShm,sizeof(char));
            for(int i=0;i<nrShm;i++){
                write(fd1,&s[i],sizeof(char));
            }
            n=7;
            char *success = "SUCCESS";
            write(fd1,&n,sizeof(char));
            for(int i=0;i<n;i++){
                write(fd1,&success[i],sizeof(char));
            }
        }
        if(strcmp(string,"EXIT")==0){
            munmap((void*)mapFisier,size);
            munmap((void*)sharedChar,numberOfBytes);
            sharedChar = NULL;
            mapFisier=NULL;
            close(shmFd);
            close(fdFiser);
            close(fd2);
            close (fd1);
            return 0;
        }
    }
    
    return 0;
}