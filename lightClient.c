#ifdef RaspberryPi 

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <stdint.h> //uint8_t definitions
#include <errno.h> //error output
#include <stdlib.h> //for exit(int);

#include <wiringPi.h>
#include <wiringSerial.h>

void loop(void);
void setup(void);
void append(char *dst, char c);

char device[] = "/dev/ttyACM0";
// filedescriptor
int fd;
unsigned long baud = 9600;
unsigned long time = 0;

#define TCP_PORT 8088
#define BUFSIZE 1024

int main(int argc, char **argv) {

	setup();

    int sockfd;
    struct sockaddr_in server_addr;
    char buf[BUFSIZE];

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

		char emptySTR[BUFSIZE];
		strcpy(buf, emptySTR);
        
		if (millis() - time >= 1000){
			serialPuts(fd, "Pong!\n");
			serialPutchar(fd, 65);
			time = millis();
		}

		// read signal
		if (serialDataAvail(fd)){

			while(1) {
				char newChar = serialGetchar(fd);
				append(buf, newChar);

				if(newChar == '\n') {
					break;
				}
			}
			
			if(send(sockfd, buf, BUFSIZE, MSG_DONTWAIT) <= 0) {
				perror("send()");
				break;
			}

			memset(buf, 0, BUFSIZE);
			if(recv(sockfd, buf, BUFSIZE, 0) <= 0) {
				perror("recv()");
				return -1;
			}
			printf("Sending message... %s", buf);

			if(strncmp(buf, "q", 1) == 0) {
				break;
			}

			fflush(stdout);
		}


    }

    close(sockfd);
    return 0;
}

void setup(){

	printf("%s \n", "Raspberry Startup!");
	fflush(stdout);

	//get filedescriptor
	if ((fd = serialOpen(device, baud)) < 0){
		fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
		exit(1); //error
	}

	//setup GPIO in wiringPi mode
	if (wiringPiSetup() == -1){
		fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));
		exit(1); //error
	}

}

void append(char *dst, char c) {
    char *p = dst;
    while (*p != '\0') p++; // 문자열 끝 탐색
    *p = c;
    *(p+1) = '\0'; 
}

#endif //#ifdef RaspberryPi