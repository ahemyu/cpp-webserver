// g++ -std=c++20 -Wall -Wextra -Wpedantic -O2 main.cpp -o server && ./server
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>

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
    }

  return 0;
}
