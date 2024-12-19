#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUF_SIZE 1024

void *recv_message(void *arg);
void error_handling(char *message);

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUF_SIZE];
    pthread_t recv_thread;

    if (argc != 3) {
        printf("Usage: %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");
    else
        puts("Connected to server...");

    // 메시지 수신을 위한 스레드 생성
    pthread_create(&recv_thread, NULL, recv_message, (void*)&sock);
    pthread_detach(recv_thread);

    while (1) {
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);

        if (!strcmp(message, "Q\n") || !strcmp(message, "q\n")) {
            close(sock);
            exit(0);
        }

        write(sock, message, strlen(message));
    }

    return 0;
}

void *recv_message(void *arg) {
    int sock = *((int*)arg);
    char message[BUF_SIZE];
    int str_len;

    while (1) {
        str_len = read(sock, message, BUF_SIZE - 1);
        if (str_len == -1)
            return (void*)-1;

        message[str_len] = '\0';
        printf("Message from server: %s", message);
    }

    return NULL;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
