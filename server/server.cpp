// name: Phạm Hoàng Nam
// mssv: 17021164
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bits/stdc++.h> 
#include <unistd.h>
#include <iostream>

using namespace std;

#define SERVER_PORT 9000
#define BUFFER_SIZE 1024

const string HELLO_200 = "200 Hello Client";
const string DOWNLOAD_FILE_OK = "210 Download file OK";
const string FILE_NOT_FOUND = "211 File not found";
const string WRONG_SYNTAX = "WRONG SYNTAX";
const string NOT_READY = "ENTER HELLO SERVER TO START";

int main(int argc, char **argv) {
    int listenPort = SERVER_PORT;
    int servSock, cliSock;
    struct sockaddr_in servSockAddr, cliAddr;
    int servSockLen, cliAddrLen;
    char recvBuf[BUFFER_SIZE];
    int n;

    if (argc >= 2){
        listenPort = atoi(argv[1]);
    }

    // create socket

    servSock = socket(AF_INET, SOCK_STREAM,0);

    if (servSock == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }  

    bzero(&servSockAddr, sizeof(servSockAddr));
    servSockAddr.sin_family = AF_INET;
    servSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servSockAddr.sin_port = htons(listenPort);

    servSockLen = sizeof(servSockAddr);
    if(bind(servSock,(struct sockaddr*)&servSockAddr, servSockLen) < 0) {
        perror("bind"); exit(1);
    }
    if(listen(servSock, 10) < 0) {
        perror("listen");
        exit(1);
    }

    bool typingFileName = false, ready = false, downloadReady = false;

     while(1){
        cout << "Waiting for a client ... \n";
        cliSock = accept(servSock, (struct sockaddr *) &cliAddr,
                        (socklen_t *)&cliAddrLen);
        cout << "Received a connection from a client: " << inet_ntoa(cliAddr.sin_addr) << endl;
        while (1){
            n = read(cliSock, recvBuf, BUFFER_SIZE-1);
            recvBuf[n] = '\0';

            // uper 
            if(n > 0) {
                cout << "Received a message from the client: "  << recvBuf << endl;

                for(int i=0; i < n; i++){
                    if (recvBuf[i] >= 97  && recvBuf[i] <= 122)
                        recvBuf[i] -= 32;
                }

                // client close connection
                if (n == 0 || strcmp(recvBuf,"QUIT") == 0){
                    close(cliSock);
                    cout << "Close the connection\n";
                    break;
                }
                
                if(strcmp(recvBuf, "HELLO SERVER") == 0) {
                    write(cliSock, HELLO_200.c_str(), HELLO_200.length());
                    ready = true;
                }
                else if(strcmp(recvBuf, "DOWNLOAD FILE") == 0) {
                    if(ready) {
                        write(cliSock, DOWNLOAD_FILE_OK.c_str(), DOWNLOAD_FILE_OK.length());
                        typingFileName = true;
                        downloadReady = true;
                    }
                    else {
                        write(cliSock, NOT_READY.c_str(), NOT_READY.length());
                    }
    
                } 
                else if(typingFileName) {
        
                    FILE *fp = fopen(recvBuf,"r");
                    if(fp == NULL)
                    {
                        write(cliSock, FILE_NOT_FOUND.c_str(), FILE_NOT_FOUND.length());
                    }
                    else 
                    {
                        if(downloadReady) {
                       
                            fseek(fp, 0L, SEEK_END);
                            int fileSize = ftell(fp);
                            fseek(fp, 0, SEEK_SET); // seek back to beginning of file
                            cout << fileSize << endl;
                            /* First read file in chunks of 256 bytes */
                            unsigned char buff[fileSize]={0};
                            int nread = fread(buff,1,fileSize,fp);

                            /* If read was success, send data. */
                            if(nread > 0)
                            {
                                write(cliSock, buff, nread);
                            } 

                        }
                        
                    }
                    typingFileName = false;
                } 
                else {
                    typingFileName = false;
                    write(cliSock, WRONG_SYNTAX.c_str(), WRONG_SYNTAX.length());
                }
            }

        }
    }
}