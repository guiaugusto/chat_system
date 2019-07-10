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

struct mq_attr attr;
struct timespec abs_timeout;
char complete_message[523];
char complete_response[523];
char final_message[523];
mqd_t group_queue[100];
char own_groups[100][18];
int counter;

void set_chat_configuration();
void open_queues();
void *receive_messages();
int send_message();
void control_handler(int sig);
void send_message_to_all_users();
void list_all_commands();
void *send_message_to_user();

#endif
