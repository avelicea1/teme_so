#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int listDir(const char *path, int recursive,const char *name_start_with, const char *permissions,int *k){
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;
    dir = opendir (path);
    long number = 0;
    int numberOctal = 0;
    if(dir == NULL){
        closedir(dir);
        perror("Could not open dir");
        return -1;
    }
    for(int i=0;i<9 && strlen(permissions) > 0;i++){
        if(permissions[i]=='r'||permissions[i]=='w'||permissions[i]=='x'){
            number=number+1;
            number= number*10;
        }else{
            if(permissions[i]=='-'){
                number=number*10;
            }
        }
    }
    number = number/10;
    
    int remainder = 0;
    int decimal =0;
    for (int i=1 ; number!=0 ;i=i*2 , number=number/10){
            remainder = number%10;
            decimal = decimal + remainder * i;
    }
    for(int i=1; decimal!=0; i=i*10){
        numberOctal = numberOctal +( decimal % 8)*i;
        decimal = decimal/8;
    }
    while((entry = readdir(dir))!=NULL){
        if(recursive==1){
            if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0){
                snprintf(fullPath,512,"%s/%s",path, entry->d_name);
                if(lstat(fullPath,&statbuf)==0){
                    int mode = statbuf.st_mode&0X777;
                    int modeOctal =0;
                    for(int i=1; mode!=0; i=i*10){
                        modeOctal = modeOctal +(mode % 8)*i;
                        mode = mode/8;
                    }   
                    if(strncmp(name_start_with,entry->d_name,strlen(name_start_with))==0 && (modeOctal == numberOctal||numberOctal ==0)){
                    (*k)++;
                    printf("%s\n",fullPath);
                    }
                    if(S_ISDIR(statbuf.st_mode)){
                        listDir(fullPath,recursive,name_start_with,permissions,k);
                    }
                }
            }
        }else{
        if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0){
            
            snprintf(fullPath,512,"%s/%s",path, entry->d_name);
            if(lstat(fullPath,&statbuf)==0){
                int mode = statbuf.st_mode & 0777;
                int modeOctal =0;
                for(int i=1; mode!=0; i=i*10){
                    modeOctal = modeOctal +(mode % 8)*i;
                    mode = mode/8;
                }
                if(strncmp(name_start_with,entry->d_name,strlen(name_start_with))==0 && (modeOctal == numberOctal || numberOctal == 0)){
                    (*k)++;
                    printf("%s\n",fullPath);
                }
                
            }
        }
        }
    }
    closedir(dir);
    return 0;
}
struct sectionHeader{
    char name[14];
    unsigned int sect_type;
    int sect_offset;
    int sect_size;
}sectionHeader;
int parse (const char *path){
    int fd = open(path,O_RDONLY);
    if(fd==-1){
        printf("ERROR \n invalid path");
        return -1;
    }
    lseek(fd,-1,SEEK_END);
    char magic;
    if(read(fd,&magic,1)==1){
        if(magic != 'l'){
            printf("ERROR \nwrong magic");
            return -1;
        }
    }
    if(magic =='l'){
        
        lseek(fd,-3,SEEK_END);
        int headerSize = 0;
        if(read(fd,&headerSize,2)!=2){
            perror("could not read size");
        }
        lseek(fd,-headerSize,SEEK_END);
        unsigned char c;
        int version =0;
        int nrSections =0;
        if(read(fd,& c,1)!=1){
            perror("could not read version!");
        }
        version=c;
        if(read(fd,&nrSections,1)!=1){
            perror("could not read no_of_sections!");
        }
        struct sectionHeader **sectionsHeaders =(struct sectionHeader **)malloc(sizeof(struct sectionHeader)*nrSections); 
        for(int i = 1;i<=nrSections;i++){
            sectionsHeaders[i] = (struct sectionHeader*)malloc(sizeof(struct sectionHeader));
        }
        if(version >=38 && version <=126){
            if(nrSections >= 3 && nrSections <= 19){
                for(int i=1;i<=nrSections ;i++){
                    char name[14];
                    if(read(fd,&name,14)!=14){
                        printf("could not read name");
                    }
                    strcpy(sectionsHeaders[i]->name,name);
                    sectionsHeaders[i]->name[14]=0;
                    unsigned int sect_type = 0;
                    if(read(fd,&sect_type,2)!=2){
                        printf("could not read sect_type");
                    }
                    sectionsHeaders[i]->sect_type=0;
                    sectionsHeaders[i]->sect_type=sect_type;
                    if(sect_type == 60 || sect_type == 75 || sect_type == 91){
                        int sect_offset = 0;
                        if(read(fd,&sect_offset,4)!=4){
                            printf("could not read sect_offset");
                        }
                        sectionsHeaders[i]->sect_offset=0;
                        sectionsHeaders[i]->sect_offset=sect_offset;
                        //printf("%d\n",sect_offset);
                        int sect_size = 0;
                        if(read(fd,&sect_size,4)!=4){
                            printf("could not read sect_size");
                        }
                        sectionsHeaders[i]->sect_size = 0;
                        sectionsHeaders[i]->sect_size = sect_size;
                    }else{
                        printf("ERROR \nwrong sect_types");
                        for(int i=1;i<=nrSections;i++){
                            free(sectionsHeaders[i]);
                        }
                        free(sectionsHeaders);
                        return -1;
                    }
                }
            }else{
                printf("ERROR \nwrong sect_nr");
                for(int i=1;i<=nrSections;i++){
                    free(sectionsHeaders[i]);
                }
                free(sectionsHeaders);
                return -1;
            }
        }else{
            printf("ERROR \nwrong version");
            for(int i=1;i<=nrSections;i++){
                free(sectionsHeaders[i]);
            }
            free(sectionsHeaders);
            return -1;
        }
        printf("SUCCESS\n");
        printf("version=%d\n",version);
        printf("nr_sections=%d\n",nrSections);
        for(int i=1;i<=nrSections;i++){
            printf("section%d: ",i);
            printf("%s ",sectionsHeaders[i]->name);
            printf("%d ",sectionsHeaders[i]->sect_type);
            printf("%d ",sectionsHeaders[i]->sect_size);
            printf("\n");
        }
        for(int i=1;i<=nrSections;i++){
            free(sectionsHeaders[i]);
        }
        free(sectionsHeaders);
    }
    close(fd);
    return 0;
}
int findAll(const char *path){
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;
    dir = opendir (path);
    if(dir == NULL){
        closedir(dir);
        perror("Could not open dir");
        return -1;
    }
    int m=0;
    while((entry = readdir(dir))!=NULL){
        if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0){
                snprintf(fullPath,512,"%s/%s",path, entry->d_name);
                if(lstat(fullPath,&statbuf)==0){
                    if(S_ISDIR(statbuf.st_mode)){
                        findAll(fullPath);
                    }
                    if(S_ISREG(statbuf.st_mode)){
                        if(m==0){
                            printf("SUCCESS\n");
                            m=1;
                        }
                        printf("aici");
                        int fd = open(fullPath,O_RDONLY);
                        
                        if(fd==-1){
                            printf("ERROR \n invalid path");
                            return -1;
                        }
                        
                        lseek(fd,-1,SEEK_END);
                        char magic;
                        read(fd,&magic,1);
                        if(magic =='l'){
                            
                            lseek(fd,-3,SEEK_END);
                            int headerSize = 0;
                            if(read(fd,&headerSize,2)!=2){
                                perror("could not read size");
                            }
                            lseek(fd,-headerSize,SEEK_END);
                            unsigned char c;
                            int version =0;
                            int nrSections =0;
                            if(read(fd,& c,1)!=1){
                                perror("could not read version!");
                            }
                            version=c;
                            if(read(fd,&nrSections,1)!=1){
                                perror("could not read no_of_sections!");
                            }
                            struct sectionHeader **sectionsHeaders1 =(struct sectionHeader **)malloc(sizeof(struct sectionHeader)*nrSections); 
                            for(int i = 1;i<=nrSections;i++){
                                sectionsHeaders1[i] = (struct sectionHeader*)malloc(sizeof(struct sectionHeader));
                            }
                            if(version >=38 && version <=126){
                                if(nrSections >= 3 && nrSections <= 19){
                                    for(int i=1;i<=nrSections ;i++){
                                        char name[14];
                                        if(read(fd,&name,14)!=14){
                                            perror("could not read name");
                                        }
                                        strcpy(sectionsHeaders1[i]->name,name);
                                        int sect_type = 0;
                                        if(read(fd,&sect_type,2)!=2){
                                            perror("could not read sect_type");
                                        }
                                        sectionsHeaders1[i]->sect_type=sect_type;
                                        if(sect_type == 60 || sect_type == 75 || sect_type == 91){
                                            int sect_offset = 0;
                                            if(read(fd,&sect_offset,4)!=4){
                                                perror("could not read sect_offset");
                                            }
                                            sectionsHeaders1[i]->sect_offset=sect_offset;
                                            //printf("%d\n",sect_offset);
                                            int sect_size = 0;
                                            if(read(fd,&sect_size,4)!=4){
                                                perror("could not read sect_size");
                                            }
                                            sectionsHeaders1[i]->sect_size = sect_size;
                                        }
                                    }
                                }
                            }
                            printf("Am gasit");
                            int k=0;
                            for(int i=1;i<=nrSections;i++){
                                if(sectionsHeaders1[i]->sect_type == 60)k++;
                            }
                            if(k==nrSections){
                                printf("%s",fullPath);
                            }
                            for(int i=1;i<=nrSections;i++){
                                free(sectionsHeaders1[i]);
                            }
                            free(sectionsHeaders1);

                        }
                        close(fd);
                    }
                }
            }
    }
    closedir(dir);
    return 0;
}
int main(int argc, char **argv){
    char name_start_with[512]="";
    char permissions[10]="";
    char path[512]="";
    int k=0;
    int r = 0, l =0, p =0,f=0;
    if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0){
            printf("21546\n");
        }
        else{
        for(int i = 1;i<argc; i++){
                if(strstr(argv[i],"name_starts_with=")!=NULL){
                    strcpy(name_start_with,argv[i]+17);
                }
                if(strstr(argv[i],"path=")!=NULL){
                    strcpy(path, argv[i]+5);
                }
                if(strstr(argv[i],"permissions=")!=NULL){
                   strcpy(permissions,argv[i]+12);
                }
                if(strcmp(argv[i],"recursive")==0){
                    r=1;
                }
                if(strcmp(argv[i],"list")==0){
                    l=1;
                }
                if(strcmp(argv[i],"parse")==0){
                    p=1;
                }
                if(strcmp(argv[i],"findall")==0){
                    f=1;
                }
        }
        if(l!=0){
            if(strcmp(path,"")!=0){
                DIR *dir = opendir(path);
                if(dir == NULL){
                    closedir(dir);
                    printf("ERROR \ninvalid directory path");
                }else{
                    printf("SUCCESS \n");
                    
                    listDir(path,r,name_start_with,permissions,&k);
                    closedir(dir);
                } 
            }else{
                printf("ERROR \npath is NULL");
            }
        }
        if(p!=0){
            if(strcmp(path,"")!=0){
                parse(path);
            }else{
                printf("ERROR \npath is NULL");
            }
        }
        if(f!=0){
            findAll(path);
        }
        }
    } 
    
    return 0;
    
}