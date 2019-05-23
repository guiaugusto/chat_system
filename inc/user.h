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

char person_name[10];
char message[500];
char me[10];

mqd_t my_queue;
mqd_t person_queue;


void chat_name(char *queue_name, char *user_name);
void define_user_name();
void open_person_queue(char *person_name);
void close_person_queue(char *person_name);

#endif
