#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *transaction(void *arg);
int initializeMutex(pthread_mutex_t *mutex);
int createThreads(pthread_t *threads, int numThreads, pthread_mutex_t *mutex);
void joinThreads(pthread_t *threads, int numThreads);

#define DEPOSIT 0
#define WITHDRAW 1

struct args
{
    pthread_mutex_t *mutex;
    int threadId;
};

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
        struct args *TransactionArgs = (struct args *)malloc(sizeof(struct args));
        TransactionArgs->mutex = mutex;
        TransactionArgs->threadId = i;

        error = pthread_create(&(threads[i]), NULL, transaction, (void *)TransactionArgs); // Have to pass in I and mutex

        if (error != 0)
            printf("\nThread can't be created :[%s]", strerror(error));
        i++;
    }
    return 0;
}

void *transaction(void *arg)
{
    FILE *ptr;
    char str[10];

    if (((struct args *)arg)->threadId == 0)
    {
        ptr = fopen("inn.txt", "r");

        if (NULL == ptr)
            printf("File can't be opened \n");
    }
    else if (((struct args *)arg)->threadId == 1)
        ptr = fopen("ut.txt", "r");
    else
        printf("Error: invalid thread id");

    if (NULL == ptr)
        printf("File can't be opened \n");

    while (fgets(str, sizeof(str), ptr) != NULL)
    {
        pthread_mutex_lock(((struct args *)arg)->mutex);
        (((struct args *)arg)->threadId) ? (balance -= atoi(str)) : (balance += atoi(str));
        pthread_mutex_unlock(((struct args *)arg)->mutex);
    }

    return NULL;
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