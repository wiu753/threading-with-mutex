#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *transaction(void *arg);
int initializeMutex(pthread_mutex_t *mutex);
int runTransactions(pthread_t *threads, int numThreads, pthread_mutex_t *mutex);
void closeThreads(pthread_t *threads, int numThreads, pthread_mutex_t *mutex);

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

    if (initializeMutex(&lock))
        printf("Error initializing mutex\n");

    if (runTransactions(threadId, 2, &lock) != 0) // The threads also run the transaction function
        printf("Error running transactions\n");

    closeThreads(threadId, 2, &lock);

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

int runTransactions(pthread_t *threads, int numThreads, pthread_mutex_t *mutex)
{
    int i = 0;

    while (i < numThreads)
    {
        // To pass multiple arguments to pthread_create you need to use a struct
        struct args *TransactionArgs = (struct args *)malloc(sizeof(struct args));
        TransactionArgs->mutex = mutex;
        TransactionArgs->threadId = i;

        if (pthread_create(&(threads[i]), NULL, transaction, (void *)TransactionArgs) != 0)
        {
            printf("\nThread creation failed\n");
            return 1;
        }

        i++;
    }
    return 0;
}

void *transaction(void *arg)
{
    FILE *ptr;
    char str[10];

    if (((struct args *)arg)->threadId == 0)
        ptr = fopen("inn.txt", "r");
    else if (((struct args *)arg)->threadId == 1)
        ptr = fopen("ut.txt", "r");
    else
        printf("Error: invalid thread id");

    if (NULL == ptr)
        printf("File can't be opened \n");

    // If the thread id is 1 (true) then it is a withdraw, otherwise it is a deposit
    while (fgets(str, sizeof(str), ptr) != NULL)
    {
        pthread_mutex_lock(((struct args *)arg)->mutex);
        (((struct args *)arg)->threadId) ? (balance -= atoi(str)) : (balance += atoi(str));
        pthread_mutex_unlock(((struct args *)arg)->mutex);
    }

    return NULL;
}

void closeThreads(pthread_t *threads, int numThreads, pthread_mutex_t *mutex)
{
    int i = 0;

    while (i < numThreads)
    {
        pthread_join(threads[i], NULL);
        i++;
    }
    pthread_mutex_destroy(mutex);
}