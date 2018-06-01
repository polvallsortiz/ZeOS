#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

#define MAXSIZE 200
#define NUMTHREADS 50

typedef struct {
    int buff[200];
    int r;
    int w;
    int size;
}buffer_circular;

buffer_circular circular;
sem_t work;
sem_t buf;

void addConnection(int FD) {
    while (circular.size > MAXSIZE);
    circular.buff[circular.w] = FD;
    circular.w = (circular.w + 1)%MAXSIZE;
    ++circular.size;
    sem_post(&work);
}

void makeConnection() {
    int FD = circular.buff[circular.r];
    circular.r = (circular.r + 1)%MAXSIZE;
    --circular.size;
    sem_post(&buf);
    doService(FD);
}

void *gestor() {
    while(1) {
        sem_wait(&work);
        makeConnection();
    }
}

void initializeThreads() {
    for(int i = 0; i < NUMTHREADS; ++i) {
        pthread_t p;
        pthread_create(&p,NULL,&gestor,NULL);
    }
}


doService(int fd) {
    int i = 0;
    char buff[80];
    char buff2[80];
    int ret;
    int socket_fd = (int) fd;

    ret = read(socket_fd, buff, sizeof(buff));
    while(ret > 0) {
        buff[ret]='\0';
        sprintf(buff2, "Server [%d] received: %s\n", getpid(), buff);
        write(1, buff2, strlen(buff2));
        ret = write(fd, "caracola ", 8);
        if (ret < 0) {
            perror ("Error writing to socket");
            exit(1);
        }
        ret = read(socket_fd, buff, sizeof(buff));
    }
    if (ret < 0) {
        perror ("Error reading from socket");

    }
    sprintf(buff2, "Server [%d] ends service\n", getpid());
    write(1, buff2, strlen(buff2));

}


int main (int argc, char *argv[])
{
    int socketFD;
    int connectionFD;
    char buffer[80];
    int ret;
    int port;
    circular.r = 0;
    circular.w = 0;
    circular.size = 0;

    //SEMAPHORES
    sem_init(&work,0,0);
    sem_init(&buf,0,1);

    initializeThreads();


    if (argc != 2)
    {
        strcpy (buffer, "Usage: ServerSocket PortNumber\n");
        write (2, buffer, strlen (buffer));
        exit (1);
    }

    port = atoi(argv[1]);
    socketFD = createServerSocket (port);
    if (socketFD < 0)
    {
        perror ("Error creating socket\n");
        exit (1);
    }
    char buff2[200];
    sprintf(buff2, "Ready to accept");
    write(1, buff2, strlen(buff2));

    while (1) {
        connectionFD = acceptNewConnections (socketFD);
        if (connectionFD < 0)
        {
            perror ("Error establishing connection \n");
            deleteSocket(socketFD);
            exit (1);
        }

        //doServiceFork(connectionFD);
        addConnection(connectionFD);
    }

}





