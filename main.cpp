// g++ -std=c++20 -Wall -Wextra -Wpedantic -O2 main.cpp -o server && ./server
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;
typedef long long unsigned u64;
typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;


int main(){
    int socketCode  = socket(AF_INET, SOCK_STREAM, 0);
    if(socketCode < 0){
      printf("Error opening Socket! \n");
      return 1;
    }
    sockaddr_in adr{}; //TODO: set this so that it is some local ip
    adr.sin_family = AF_INET;
    adr.sin_port = 0x901F; //reverse notation for 8080 in hex bc x68 machines they store it Little Endian but we need network byte order (Big Endian)
    in_addr localhost;
    localhost.s_addr = 0x0100007F; //reverse notation for 127.0.0.1 in hex bc x68 machines they store it Little Endian but we need network byte order (Big Endian)
    adr.sin_addr = localhost;
    socklen_t len = sizeof(adr);

    s32 bindReturn = bind(socketCode, reinterpret_cast<const sockaddr*>(&adr), len);
    if(bindReturn != 0){
      printf("Error binding Socket! \n");
      return 1;
    }
    
    s32 listenReturn = listen(socketCode, 1 ); //socketCode is the socket file descriptor
    if(listenReturn != 0){
      printf("Error binding Socket! \n");
      return 1;
    }
    sockaddr clientAddr;
    socklen_t clientAddrLen;
    //accept connections from clients on 127.0.0.1:8080
    s32 acceptReturn = accept(socketCode, &clientAddr, &clientAddrLen);
    if(acceptReturn == -1){
      printf("Error binding Socket! \n");
      return 1;
    }

  return 0;
}
