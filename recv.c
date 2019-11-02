//Dumitru Andrei-Tiberiu 321CB
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

#define INF 9999

typedef struct msg_seq {
  int no_seq;
  char payload[MSGSIZE - sizeof(int)];
} MS;

int main(int argc,char** argv){
  msg m;
  init(HOST,PORT);
  int i;
  //the total number of messages is initialized with a big number (INF)
  int count = INF;
  //create name for output file
  char outputFileName[20];
  strcpy(outputFileName, "recv_");

  //the list which contains the received messages
  msg *msgList = (msg*)calloc(count, sizeof(msg));
  int *isRecvd = (int*)calloc(count, sizeof(int));
  MS *temp;
        printf("[RECV] Inainte de bucla\n");

  for (i = 0; i < count;) {
    //get message from sender
  	int res = recv_message(&m);
  	if (res >= 0) {
  		temp = (MS*)m.payload;
      printf("[RECV] Am isRecvd mesaj %d\n", temp->no_seq);
      //is the current message was not received before
  		if (isRecvd[temp->no_seq] == 0) {
        //copy the message in the list at its sequence number
  			memcpy(&msgList[temp->no_seq], &m, sizeof(msg));
  			isRecvd[temp->no_seq] = 1;
  			i++;

        //send ACK
  		  send_message(&m);
        printf("[RECV] Se trimite ACK pentru %d\n", temp->no_seq);
  		}

  		if (temp->no_seq == 0) {
        //get the real number of messages
        count = msgList[0].len;
  		}
  	}
  }

  temp = (MS*)msgList[0].payload;
  strcat(outputFileName, temp->payload);
  //output file
  int fd = open(outputFileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    perror("Cannot open file error\n");
    return -1;
  }
  printf("[RECV] Se scrie in fisier ......COUNT: %d\n", count);

  for (i = 1; i < count; i++) {
  	temp = (MS *)msgList[i].payload;
    //write the messages in the proper order
    printf("Numar secventa la scriere %d\n", temp->no_seq);
  	write(fd, temp->payload, msgList[i].len);
  }

  close(fd);
  //final message
  temp = (MS*)msgList[0].payload;
  temp->no_seq = INF;
  send_message(&msgList[0]);
  printf("[RECV] Se trimite mesajul de final\n");
  free(msgList);
  free(isRecvd);

  return 0;
}
