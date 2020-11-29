#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include <string.h>
#include <wiringPi.h>

#define TCP_PORT 8088
#define BUFSIZE 1024

int main(int argc, char **argv) {

    wiringPiSetup();
    int sockfd;
    struct sockaddr_in server_addr;
    char buf[BUFSIZE];
    char distanceSTR[BUFSIZE];

    int trig = 8 ;
    int echo = 9 ;

    int ledpin = 1;

    int start_time, end_time ;
    float distance ;

    pinMode(trig, OUTPUT) ;
    pinMode(echo , INPUT) ;

    pinMode(ledpin, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(19);
    pwmSetRange(1000000);

    if(argc < 2) {
        printf("usage : %s IP_ADDRESS\n", argv[0]);
        return -1;
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    inet_pton(AF_INET, argv[1], &(server_addr.sin_addr.s_addr));
    server_addr.sin_port = htons(TCP_PORT);

    if(connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        return -1;
    }

    while(1) {
        memset(buf, 0, BUFSIZE);
        //fgets(buf, BUFSIZE, stdin);

        digitalWrite(trig, LOW) ;
        delay(500) ;
        digitalWrite(trig, HIGH) ;
        delayMicroseconds(10) ;
        digitalWrite(trig, LOW) ;
        while (digitalRead(echo) == 0) ;
        start_time = micros() ;
        while (digitalRead(echo) == 1) ;
        end_time = micros() ;
        distance = (end_time - start_time) / 29. / 2. ;
        sprintf(buf, "distance : %f cm\n", distance);
        pwmWrite(ledpin,1000000 - (100000/(int)distance));

        if(send(sockfd, buf, BUFSIZE, MSG_DONTWAIT) <= 0) {
            perror("send()");
            break;
        }

        memset(buf, 0, BUFSIZE);
        if(recv(sockfd, buf, BUFSIZE, 0) <= 0) {
            perror("recv()");
            return -1;
        }
        printf("received message : %s", buf);

        if(strncmp(buf, "q", 1) == 0) {
            break;
        }
    }

    close(sockfd);
    return 0;
}