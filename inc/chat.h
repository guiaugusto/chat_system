#ifndef CHAT_H

#define CHAT_H
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\e[38;5;27m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_MAGENTA "\e[38;5;200m"

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
char complete_message[522];
char complete_response[522];

void set_chat_configuration();
void open_queues();
void *receive_messages();
int send_message();
void control_handler(int sig);
void send_message_to_all_users();
void list_all_commands();

#endif
