#ifndef USER_H
#define USER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

char complete_message[522];
char complete_response[522];
char person_name[10];
char message[500];
char me[10];

mqd_t my_queue;
mqd_t person_queue;

int counter;
struct mq_attr attr;

void set_chat_configuration();
void define_user_name();
void open_queues();
void open_person_queue(char *person_name);
void send_message();
void *receive_messages();

#endif
