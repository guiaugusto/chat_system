#include "chat.h"
#include "user.h"
#include "file.h"

void set_chat_configuration(){
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(complete_message);
    attr.mq_flags = 0;
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

int send_message(){
    printf(ANSI_COLOR_GREEN "\n");
    memset(complete_message, 0, sizeof(complete_message));
    int i = 0, j = strlen(me) + 1;

    signal(SIGINT, control_handler);

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
    }else if(strcmp(complete_message, "") == 0){
        printf("\nMensagem está vazia\n");
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

    if(strcmp(person_name, "all") == 0){
        send_message_to_all_users();
    }else if(validate_destiny_user(person_name) == 0){
        printf("UNKNOWNUSER %s\n", person_name);
        return 1;
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

void control_handler(int sig){
    signal(sig, SIG_IGN);
    printf("\nPara parar a execução, você deverá digitar: sair\n");
    signal(SIGINT, control_handler);
}

void send_message_to_all_users(){
  struct dirent *de;
  char *queue_name;

  DIR *dr = opendir("/dev/mqueue");

  if(dr == NULL){
      printf("Diretório não pôde ser aberto." );
      return;
  }

  char queue_destiny_name[15] = "chat-";
  char user_name[10];
  char header[6];
  int i, j;

  while((de = readdir(dr)) != NULL){
      queue_name = de->d_name;

      if(strlen(queue_name) >= 6){
        for(i = 0; i < 5; i++){
          header[i] = queue_name[i];
        }
        if(strcmp(header, queue_destiny_name) == 0){
          for(j = 0; i < strlen(queue_name); i++, j++){
            user_name[j] = queue_name[i];
          }

          if(strcmp(user_name, me) == 0){
            continue;
          }

          open_person_queue(user_name);

          int send = mq_send (person_queue, (void *) &complete_message, strlen(complete_message), 0);

          if (send < 0){
            perror("guiaugusto mq_send");
            exit(1);
          }

          close_person_queue(user_name);
          memset(user_name, 0, sizeof(user_name));
          memset(header, 0, sizeof(header));
        }
      }

  }
}
