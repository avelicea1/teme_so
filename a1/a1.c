#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

int listDir(const char *path, const char *recursive,const char *name_start_with, const char *permissions,int *k ){
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;
    dir = opendir (path);
    long number = 0;
    int numberOctal = 0;
    if(dir == NULL){
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
        
        if(strcmp(recursive,"recursive")==0){
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
                    k++;
                    printf("%s\n",fullPath);
                    }
                    if(S_ISDIR(statbuf.st_mode)){
                        
                        listDir(fullPath,recursive,name_start_with,permissions);
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
                    k++;
                    printf("%s\n",fullPath);
                }
                
            }
        }
        }
    }
    closedir(dir);
    return 0;
}
char *name_start_with = "";
char *permissions ="";
char *recursive = "";
char *path ="";
char* list="";
int k=0;
int main(int argc, char **argv){
    if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0){
            printf("21546\n");
        }
        else{
        for(int i = 1;i<argc; i++){
                if(strstr(argv[i],"name_starts_with=")!=NULL){
                    name_start_with = argv[i]+17;
                }
                if(strstr(argv[i],"path=")!=NULL){
                    path = argv[i]+5;
                }
                if(strstr(argv[i],"permissions=")!=NULL){
                   permissions=argv[i]+12;
                }
                if(strcmp(argv[i],"recursive")==0){
                    recursive="recursive";
                }
                if(strcmp(argv[i],"list")==0){
                    list="list";
                }
        }
        if(list != NULL){
            if(strcmp(path,"")!=0){
                if(opendir(path) == NULL){
                    printf("ERROR \ninvalid directory path");
                }else{
                    printf("SUCCESS \n");
                    listDir(path,recursive,name_start_with,permissions,&k);
                } 
            }else{
                printf("ERROR \npath is NULL");
            }
        }
        }
    }        
    return 0;
}