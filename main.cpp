// g++ -std=c++20 -Wall -Wextra -Wpedantic -O2 main.cpp -o server && ./server
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;
typedef long long unsigned u64;
typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

struct path{
  u8 length {};
  u8* pathBegin {};
};

void parseHttpHeader(u8* buffer, u16 bytesRead, path* pathStr)
{
  // we need to parse the header line by lin and determine if is a valid GET request
  // each byte is one char of the header
  if((std::memcmp(buffer, "GET ", 4) != 0)){
    printf("Error, this is not a GET request. \n");
    return;
  }
  //read out the path, which are the bytes until you reach next space
  u8 bufferOffset = 4;
  pathStr->pathBegin = &buffer[bufferOffset];

  while(true)
  { //0x20 is hex code of empty space
    u8 currentByte = *(buffer + bufferOffset);
    if(currentByte == 0x20){
      break;
    }
    bufferOffset++;
  }
  pathStr->length = &buffer[bufferOffset] - pathStr->pathBegin;

  // TODO: validate that the last bytes end with "HTTP/1.1"
  // bufferOffset++; //increase to first char of version
  // u8* versionBegin = &buffer[bufferOffset];
  // u8 versionBytes = 0;
  // while(true)
  // {
  //   u8 currentByte = *(buffer + bufferOffset);
  //   if(currentByte == )
  // }
}

int main(){
    int socketCode  = socket(AF_INET, SOCK_STREAM, 0);
    if(socketCode < 0){
      printf("Error opening Socket! \n");
      return 1;
    }
    sockaddr_in adr{};
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
    sockaddr clientAddr{};
    socklen_t clientAddrLen {};
    //accept connections from clients on 127.0.0.1:8080
    s32 clientSocket = accept(socketCode, &clientAddr, &clientAddrLen); //this returns the descriptor of the new client socket
    if(clientSocket == -1){
      printf("Error binding Socket! \n");
      return 1;
    }
    
    // allocate a buffer to store the incoming bytes
    u8 buffer [4096] = {}; //a http get header should not be bigger than 4kb (I think)
    u8* currentPos = buffer; // we will use this to track where in the buffer we are
    s32 readBytes {};
    u16 remainingBytes = sizeof(buffer);

    while(true)
    {
        readBytes = read(clientSocket, currentPos, remainingBytes);
      
        if(readBytes == -1)
        {
          printf("Error reading Bytes from client socket! \n");
          break;
        }
        if(readBytes == 0)
        {
          printf("We reached EOF !");
          break;
        }
        remainingBytes -= readBytes;
        currentPos += readBytes;
        // the 4 bytes before currentPos are the ones we need to check if they equal to \r\n\r\n which is: 0D 0A 0D 0A
        if((*(currentPos - 4) == 0x0D) && (*(currentPos - 3 ) == 0x0A) && (*(currentPos - 2) == 0x0D) && *(currentPos - 1) == 0x0A)
        {
            break;
        }
    }
    u16 totalBytesRead = 4096 - remainingBytes;
    path p;
    parseHttpHeader(buffer, totalBytesRead, &p);
    
    //TODO: call a function that returns the bytes of the resource that sits at the path
    //TODO: call a function that returns a HttpResponse to the client

  //close the sockets
  close(socketCode);
  close(clientSocket);
  return 0;
}
