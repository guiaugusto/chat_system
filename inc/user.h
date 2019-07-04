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
#include <dirent.h>

char person_name[12];
char message[501];
char me[12];
char user_to_send[12];

mqd_t my_queue;
mqd_t person_queue;


void chat_name(char *queue_name, char *user_name);
int define_user_name();
void open_person_queue(char *person_name);
void close_person_queue(char *person_name);
int validate_username_size(char *username);

#endif
