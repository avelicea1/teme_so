#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
typedef struct
{
    int value;
    int pid;
} TH_STRUCT;

typedef struct
{
    int id;
    sem_t *log;
    pthread_mutex_t *mutex;
    pthread_cond_t *cond_13;
    pthread_cond_t *cond;
    pthread_mutex_t *barrier;
    int nr_still_to_come;
} TH_STRUCT_P5;

sem_t *sem1, *sem2, *sem3, *sem4;

void *thread_function(void *args)
{
    TH_STRUCT *s = (TH_STRUCT *)args;
    if (s->value == 2 && s->pid == 7)
    {
        sem_wait(sem1);
    }
    if (s->value == 4 && s->pid == 2)
    {
        sem_wait(sem2);
    }

    if (s->value == 1 && s->pid == 7)
    {
        sem_wait(sem3);
    }
    info(BEGIN, s->pid, s->value);
    if (s->value == 4 && s->pid == 7)
    {
        sem_post(sem3);
        sem_wait(sem4);
    }
    info(END, s->pid, s->value);
    if (s->value == 3 && s->pid == 2)
    {
        sem_post(sem1);
    }
    if (s->value == 2 && s->pid == 7)
    {
        sem_post(sem2);
    }

    if (s->value == 1 && s->pid == 7)
    {
        sem_post(sem4);
    }
    return NULL;
}
int count = 0;
int s_13 = 0;
int count1 = 0;

void *thread_function_p5(void *args)
{
    TH_STRUCT_P5 *s = (TH_STRUCT_P5 *)args;
    
    sem_wait(s->log);
    info(BEGIN,5,s->id);
    pthread_mutex_lock(s->mutex);
    count ++;
    if(s->id == 13){
        while(count>=6){
            pthread_cond_wait(s->cond,s->mutex);
        }
        info(END,5,13);
        pthread_mutex_unlock(s->mutex);
        sem_post(s->log);
        return NULL;
    }
    pthread_mutex_unlock(s->mutex);
    pthread_mutex_lock(s->mutex);
    count --;
    if(s->id!=13)info(END,5,s->id);
    if(count ==5){
        pthread_cond_signal(s->cond);
    }

    pthread_mutex_unlock(s->mutex); 
    sem_post(s->log) ; 
    return NULL;
}
int main()
{
    pid_t pid2 = 0, pid3 = 0, pid4 = 0, pid5 = 0, pid6 = 0, pid7 = 0;
    TH_STRUCT params_p2[4];
    pthread_t tids_p2[4];
    TH_STRUCT params_p7[4];
    pthread_t tids_p7[4];
    init();
    sem_unlink("sem1");
    sem_unlink("sem2");
    sem_unlink("sem3");
    sem_unlink("sem4");

    sem1 = sem_open("sem1", O_CREAT, 0644, 0);
    sem2 = sem_open("sem2", O_CREAT, 0644, 0);
    sem3 = sem_open("sem3", O_CREAT, 0644, 0);
    sem4 = sem_open("sem4", O_CREAT, 0644, 0);
    info(BEGIN, 1, 0);

    pid2 = fork();
    if (pid2 == -1)
    {
        perror("could not create p2");
        return 1;
    }
    else if (pid2 == 0)
    {
        info(BEGIN, 2, 0);
        for (int i = 0; i < 4; i++)
        {
            params_p2[i].value = i + 1;
            params_p2[i].pid = 2;
            pthread_create(&tids_p2[i], NULL, thread_function, &params_p2[i]);
        }
        for (int i = 0; i < 4; i++)
        {
            pthread_join(tids_p2[i], NULL);
        }
        info(END, 2, 0);
    }
    else
    {
        pid3 = fork();
        if (pid3 == -1)
        {
            perror("could not create p3");
            return 1;
        }
        else if (pid3 == 0)
        {
            info(BEGIN, 3, 0);
            pid4 = fork();
            if (pid4 == -1)
            {
                perror("could not create p4");
                return 1;
            }
            else if (pid4 == 0)
            {
                info(BEGIN, 4, 0);
                pid7 = fork();
                if (pid7 == -1)
                {
                    perror("could not create p7");
                    return 1;
                }
                else if (pid7 == 0)
                {
                    info(BEGIN, 7, 0);

                    for (int i = 0; i < 4; i++)
                    {
                        params_p7[i].value = i + 1;
                        params_p7[i].pid = 7;
                        pthread_create(&tids_p7[i], NULL, thread_function, &params_p7[i]);
                    }
                    for (int i = 0; i < 4; i++)
                    {
                        pthread_join(tids_p7[i], NULL);
                    }
                    info(END, 7, 0);
                }
                else
                {
                    wait(NULL);
                    info(END, 4, 0);
                }
            }
            else
            {
                wait(NULL);
                info(END, 3, 0);
            }
        }
        else
        {
            pid5 = fork();
            if (pid5 == -1)
            {
                perror("could not create p5");
                return 1;
            }
            else if (pid5 == 0)
            {
                info(BEGIN, 5, 0);
                int nrThreads = 42;
                pthread_t tids[nrThreads];
                TH_STRUCT_P5 params[nrThreads];
                sem_t log;
                pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
                pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
                pthread_cond_t cond_13 = PTHREAD_COND_INITIALIZER;
                pthread_mutex_t barrier = PTHREAD_MUTEX_INITIALIZER;
                sem_init(&log, 0, 6);
                for (int i = 0; i < nrThreads; i++)
                {
                    params[i].id = i + 1;
                    params[i].log = &log;
                   params[i].mutex = &mutex;
                    params[i].cond = &cond;
                    params[i].cond_13 = &cond_13;
                    params[i].barrier = &barrier;
                    pthread_create(&tids[i], NULL, thread_function_p5, &params[i]);
                }
                for (int i = 0; i < nrThreads; i++)
                {
                    pthread_join(tids[i], NULL);
                }
                sem_destroy(&log);
                pthread_mutex_destroy(&mutex);
                pthread_cond_destroy(&cond);
                pthread_cond_destroy(&cond_13);
                pthread_mutex_destroy(&barrier);
                pid6 = fork();
                if (pid6 == -1)
                {
                    perror("could not create p6");
                    return 1;
                }
                else if (pid6 == 0)
                {
                    info(BEGIN, 6, 0);
                    info(END, 6, 0);
                }
                else
                {
                    wait(NULL);
                    info(END, 5, 0);
                }
            }
            else
            {
                wait(NULL); // astept dupa 2
                wait(NULL); // astept dupa 3
                wait(NULL); // astept dupa 5
                info(END, 1, 0);
            }
        }
    }
    sem_close(sem1);
    sem_close(sem2);
    sem_close(sem3);
    sem_close(sem4);
    return 0;
}