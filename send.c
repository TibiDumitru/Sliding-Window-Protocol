//Dumitru Andrei-Tiberiu 321CB
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

#define INF 9999

typedef struct msg_seq {
  int no_seq;
  char payload[MSGSIZE - sizeof(int)];
} MS;

int main(int argc,char** argv){
  init(HOST,PORT);
  msg ack;
  int i;
  //the size of the new structure
  int MSGSIZE2 = MSGSIZE - sizeof(int);
   
  char fileName[20];
  strcpy(fileName, argv[1]);
  int speed = atoi(argv[2]);
  int delay = atoi(argv[3]);

  int fd = open(fileName, O_RDONLY);
  if (fd < 0) {
    perror("Cannot open file error\n");
    return -1;
  }
  
  int BDP = speed * delay;
  //window size
  int w = (BDP * 1000) / (MSGSIZE2 * 8);

  int size = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  //get total number of messages
  int count;
  if (size % MSGSIZE2) 
    count = size / MSGSIZE2 + 1;
  else
    count = size / MSGSIZE2;

  //should count be smaller 
  w = (w < count) ? w : count;

  printf("COUNT: %d\tWindow: %d\n", (count + 1), w);

  //the list of messages that will be sent
  msg *msgList = (msg*)calloc(count + 1, sizeof(msg));
  //isSent[i] is 1 if the ith msg was successfully sent
  int *isSent = (int*)calloc(count + 1, sizeof(int));

  //the first message contains fileName and the total number of messages
  MS *temp = (MS*)((msgList[0]).payload);
  strcpy(temp->payload, fileName);
  temp->no_seq = 0;
  msgList[0].len = count + 1;
  send_message(&msgList[0]);
  printf("[SEND] Se trimite mesajul 0\n");
 
  //read from file and fill the window with packets
  for (i = 1; i < w + 1; i++) {
    temp = (MS*)((msgList[i]).payload);
    temp->no_seq = i;
    msgList[i].len = read(fd, temp->payload, MSGSIZE2);
    send_message(&msgList[i]);
  }

  //read from file and complete the list of messages
  for (i = w + 1; i < count + 1; i++) {
    temp = (MS*)((msgList[i]).payload);
    temp->no_seq = i;
    msgList[i].len = read(fd, temp->payload, MSGSIZE2);
  }

  int index = w + 1;

  while (1) {
    int res = recv_message_timeout(&ack, delay * 0.4);
    if (res >= 0) {
      temp = (MS*)(ack.payload);
      printf("[SEND] Am primit ACK pentru %d\n", temp->no_seq);
      //if the current message is the final message
      if (temp->no_seq == INF) {
        close(fd);
        free(msgList);
        free(isSent);
        printf("[SEND] Send se termina\n");
        return 0;
      }
      //if the current message was not sent already
      if (isSent[temp->no_seq] == 0) {
        isSent[temp->no_seq] = 1;
      }
    }
    if (index > count) 
      index = 0;
    //go to the first message that is not sent    
    while (isSent[index] == 1 && index < (count + 1)) 
      index++;
    if (isSent[index] == 0)
      send_message(&msgList[index]);
    index++;
  }
  close(fd);
  free(msgList);
  free(isSent);

  recv_message(&ack);
  printf("[SEND] Send se termina\n");

  return 0;
}
