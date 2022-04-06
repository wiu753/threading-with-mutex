#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *deposit(void *arg);
void *withdraw(void *arg);
int initializeMutex(pthread_mutex_t *mutex);
int createThreads(pthread_t *threads, int numThreads, pthread_mutex_t *mutex);
void joinThreads(pthread_t *threads, int numThreads);

int balance;

int main(void)
{
    pthread_mutex_t lock;
    pthread_t threadId[2];

    initializeMutex(&lock);
    createThreads(threadId, 2, &lock);
    joinThreads(threadId, 2);
    pthread_mutex_destroy(&lock);

    printf("Balance: %d", balance);

    return 0;
}

int initializeMutex(pthread_mutex_t *mutex)
{
    if (pthread_mutex_init(mutex, NULL) != 0)
    {
        printf("\nMutex initialization has failed\n");
        return 1;
    }
    return 0;
}

int createThreads(pthread_t *threads, int numThreads, pthread_mutex_t *mutex)
{
    int i = 0;
    int error;

    while (i < numThreads)
    {
        if (i == 0)
            error = pthread_create(&threads[i], NULL, deposit, mutex);
        else
            error = pthread_create(&(threads[i]), NULL, &withdraw, mutex);

        if (error != 0)
            printf("\nThread can't be created :[%s]", strerror(error));
        i++;
    }
    return 0;
}

void joinThreads(pthread_t *threads, int numThreads)
{
    int i = 0;

    while (i < numThreads)
    {
        pthread_join(threads[i], NULL);
        i++;
    }
}

void *deposit(void *arg)
{
    FILE *ptr;
    char str[50];
    ptr = fopen("inn.txt", "r");

    if (NULL == ptr)
        printf("File can't be opened \n");

    while (fgets(str, 50, ptr) != NULL)
    {
        pthread_mutex_lock(arg);
        balance += atoi(str);
        pthread_mutex_unlock(arg);
    }

    fclose(ptr);
    return NULL;
}

void *withdraw(void *arg)
{
    FILE *ptr;
    char str[50];
    ptr = fopen("ut.txt", "r");

    if (NULL == ptr)
        printf("File can't be opened\n");

    while (fgets(str, 50, ptr) != NULL)
    {
        pthread_mutex_lock(arg);
        balance -= atoi(str);
        pthread_mutex_unlock(arg);
    }

    fclose(ptr);
    return NULL;
}