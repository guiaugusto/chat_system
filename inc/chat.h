#ifndef CHAT_H

#define CHAT_H
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\e[38;5;27m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_MAGENTA "\e[38;5;200m"
#define ANSI_COLOR_ORANGE "\e[38;5;202m"

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
#include <list.h>

struct group_attrib{
  struct mq_attr group_attr;
  char owner_nickname[20];
  char group_name[20];
  List *users_list;
  pthread_t thread;
  mqd_t queue;
};

typedef struct message_object{
  char current_message[523];
  char user_to_send[20];
  char channel_type[10];
}message_object;

struct mq_attr attr;

struct timespec abs_timeout;
char complete_message[523];
char complete_response[523];
char final_message[523];
// mqd_t group_queue[100];
struct group_attrib own_groups[100];
int counter;
char channel_type[8];

void set_chat_configuration();
void open_queues();
void *receive_messages(mqd_t queue);
int send_message();
void control_handler(int sig);
void send_message_to_all_users();
void list_all_commands();
void *send_message_to_user();

#endif
