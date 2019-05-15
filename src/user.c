#include "user.h"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void set_chat_configuration(){
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(complete_message);
    attr.mq_flags = 0;
}

void define_user_name(){
    printf("Username (max 10 characters): ");
    scanf("%s", me);
    getchar();
    printf("Seu username é %s, não se esqueça!\n", me);
}

void open_queues(){
    char queue[16] = "/chat-";
    strcat(queue, me);

     if((my_queue = mq_open(queue, O_RDWR|O_CREAT, 0666, &attr)) < 0){
        perror("mq_open");
        exit(1);
    }

    mq_close(my_queue);

    if((my_queue = mq_open(queue, O_RDWR|O_CREAT)) < 0){
        perror("guiaugusto read mq_open");
        exit(1);
    }
}

void open_person_queue(char *person_name){
    char queue_name[16] = "/chat-";
    int i, j;

    for(i = 6, j = 0; j < strlen(person_name); i++, j++){
        queue_name[i] = person_name[j];
    }

    if((person_queue = mq_open(queue_name, O_RDWR|O_CREAT, 0666, &attr)) < 0){
        perror("person name mq_open");
        exit(1);
    }
}

void close_person_queue(char *person_name){
    mq_close(person_queue);
}

int send_message(){
    memset(complete_message, 0, sizeof(complete_message));
    int i = 0, j = strlen(me) + 1;

    printf(ANSI_COLOR_GREEN);

    scanf("%[^\n]*c", complete_message);
    getchar();

    if(strcmp(complete_message, "sair") == 0){
        mq_close(my_queue);
        char queue_name[16] = "/chat-";
        strcat(queue_name, me);
        mq_unlink(queue_name);
        return 0;
    }else if(strcmp(complete_message, "list") == 0){
        show_all_users_online();
        return 1;
    }

    while(1){
        if(complete_message[j] == ':'){
          break;
        }

        person_name[i] = complete_message[j];

        i++;
        j++;
    }

    open_person_queue(person_name);

    int send = mq_send (person_queue, (void *) &complete_message, strlen(complete_message), 0);

    if (send < 0){
        perror("guiaugusto mq_send");
        exit(1);
    }

    close_person_queue(person_name);

    return 1;
}

void *receive_messages(){
  char *sender_name;
  char *user_name;
  char *sender_message;

  while(1){
      int receive = mq_receive(my_queue, (void*) &complete_response, sizeof(complete_response), 0);

      char split[] = ":";
      sender_name = strtok(complete_response, split);
      user_name = strtok(NULL, split);
      sender_message = strtok(NULL, split);

      printf(ANSI_COLOR_BLUE "%s: %s", sender_name, sender_message);
      printf(ANSI_COLOR_GREEN "\n");

      memset(user_name, 0, sizeof(user_name));
      memset(sender_name, 0, sizeof(sender_name));
      memset(complete_response, 0, sizeof(complete_response));
      memset(sender_message, 0, sizeof(sender_message));
  }

  pthread_exit(NULL);
}
