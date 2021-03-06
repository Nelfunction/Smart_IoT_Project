#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <wiringPi.h>

#define TCP_PORT 8088
#define BUFSIZE 1024
#define MAX_CLIENT 5

#define LEDPIN 1

typedef struct client_thread_info {
    pthread_t thread;
    struct sockaddr_in client_addr;
    int sockfd;
    int is_created;
}CTI;

void client_thread_cleanup(void *aux)
{
    CTI *cti = (CTI*) aux;
    cti->is_created = 0;
}

void *client_thread_loop(void *aux)
{
    CTI *cti = (CTI*) aux;
    char fromstr[64];
    char buf[BUFSIZE];

    pthread_cleanup_push(client_thread_cleanup, (void*)aux);

    inet_ntop(AF_INET, &cti->client_addr.sin_addr, fromstr, 64);
    printf("Connected with client %s\n", fromstr);

    do{
        int n;

        memset(buf, 0x00, BUFSIZE);
        if((n = read(cti->sockfd, buf, BUFSIZE)) <=0 ) {
            perror("client thread read()");
            break;
        }

        printf("Data sent from client %s : %s", fromstr, buf);
        if(strncmp("on", buf, 2)==0){
            printf("LED on\n");
            digitalWrite(LEDPIN, HIGH);
        } else if(strncmp("off", buf, 3) == 0) {
            printf("LED off\n");
            digitalWrite(LEDPIN, LOW);
        }

        if (write(cti->sockfd,buf, n) <= 0) {
            perror("client thread write()");
            break;
        }
    } while (strncmp(buf, "q", 1) !=0);

    printf("Halt connection from client %s\n", fromstr);
    close(cti->sockfd);

    pthread_exit((void*) 0);
    pthread_cleanup_pop(0);

}

void start_client_thread(CTI* cti)
{
    memset(&cti->thread, 0x00, sizeof(pthread_t));
    if (!pthread_create(&cti->thread, NULL, client_thread_loop, cti)) {
        cti->is_created = 1;
    } else {
        fprintf(stderr, "error creating client thread\n");
    }
}

int main(int argc, char **argv)
{
    CTI client_threads[MAX_CLIENT];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    int server_sockfd, client_sockfd;
    char buf[BUFSIZE];
    int i;

    wiringPiSetup();
    pinMode(LEDPIN, OUTPUT);

    for (i=0; i<MAX_CLIENT; i++){
        memset(client_threads, 0x00, sizeof(client_threads));
    }

    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    memset(&server_addr, 0x00, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(TCP_PORT);

    if (bind(server_sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind()");
        return -1;
    }

    if (listen(server_sockfd, MAX_CLIENT) < 0) {
        perror("listen()");
        return -1;
    }
    printf("listning on port %d ... \n", TCP_PORT);

    client_addr_len = sizeof(client_addr);

    while (1) {
        client_sockfd = accept(server_sockfd, (struct sockaddr*)& client_addr, &client_addr_len);
        if (client_sockfd < 0){
            perror("accept()");
            return -1;
        }

        for (i = 0; i<MAX_CLIENT; i++){
            if(!client_threads[i].is_created) {
                client_threads[i].client_addr = client_addr;
                client_threads[i].sockfd = client_sockfd;
                start_client_thread(&client_threads[i]);
                break;
            }
        }

        if (i >= MAX_CLIENT) {
            fprintf(stderr, "cannot accept more client\n");
        }

    };

    return 0;
}    
