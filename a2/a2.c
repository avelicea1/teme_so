#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
pid_t pid1=0,pid2=0,pid3=0,pid4=0,pid5=0,pid6=0,pid7=0;
int main(){
    init();
    info(BEGIN,1,0);
    pid2=fork();
    if(pid2==-1){
        perror("could not create p2");
        return 1;
    }else if(pid2==0){
        info(BEGIN,2,0);
        info(END,2,0);
    }else{
        wait(NULL);
        pid3 = fork();
        if(pid3 == -1){
            perror("could not create p3");
            return 1;
        }else if(pid3 == 0){
            info(BEGIN,3,0);
            pid4=fork();
            if(pid4 == -1){
                perror("could not create p4");
                return 1;
            }else if(pid4 == 0){
                info(BEGIN,4,0);
                pid7 = fork();
                if(pid7 == -1){
                    perror("could not create p7");
                    return 1;
                }else if(pid7 ==0){
                    info(BEGIN,7,0);
                    info(END,7,0);
                }else{
                    wait(NULL);
                    info(END,4,0);
                }
                
            }else{
                wait(NULL);
                info(END,3,0);
            } 
        }else{
            wait(NULL);
            pid5 = fork();
            if(pid5 == -1){
                perror("could not create p5");
                return 1;
            }else if(pid5 == 0){
                info(BEGIN,5,0);
                pid6= fork();
                if(pid6 == -1){
                    perror("could not create p6");
                    return 1;
                }else if(pid6 == 0){
                    info(BEGIN,6,0);
                    info(END,6,0);
                }else{
                    wait(NULL);
                    info(END,5,0);
                }
            }else{
                wait(NULL);
                wait(NULL);
                info(END,1,0);
            }
            
        }
        
    }
    return 0;
}