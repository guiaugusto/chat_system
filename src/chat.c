#include "chat.h"
#include "user.h"
#include "file.h"

void set_chat_configuration(){
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(complete_message);
    attr.mq_flags = 0;
    abs_timeout.tv_sec = 1;
}

void open_queues(){
    char queue[16] = "/chat-";
    char mode[] = "0622";
    int permission = strtol(mode, 0, 8);
    strcat(queue, me);

     if((my_queue = mq_open(queue, O_CREAT, permission, &attr)) < 0){
        perror("create queue error");
        exit(1);
    }

    mq_close(my_queue);

    if((my_queue = mq_open(queue, O_RDWR)) < 0){
        perror("read mq_open");
        exit(1);
    }

    char queue_location[50] = "/dev/mqueue";
    strcat(queue_location, queue);
    if(chmod(queue_location, permission) < 0){
        perror("chmod error");
        exit(1);
    }

    memset(queue_location, 0, sizeof(queue_location));
}

int send_message(){
    printf(ANSI_COLOR_GREEN);
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
        printf(ANSI_COLOR_RESET);
        return 0;
    }else if(strcmp(complete_message, "list") == 0){
        show_all_users_online();
        return 1;
    }else if(strcmp(complete_message, "") == 0){
        printf(ANSI_COLOR_YELLOW "Mensagem está vazia" ANSI_COLOR_GREEN "\n");
        return 1;
    }else if(strcmp(complete_message, "stats") == 0){
        char queue_name[16] = "/chat-";
        strcat(queue_name, me);
        show_queue_information(queue_name);
        return 1;
    }else if(strcmp(complete_message, "help") == 0){
        list_all_commands();
        return 1;
    }

    char split[] = ":";
    char *username;
    char *receiver_name;
    char *person_message;
    char message[501];

    strcpy(message, complete_message);

    username = strtok(message, split);
    receiver_name = strtok(NULL, split);
    person_message = strtok(NULL, split);

    // Primeiro tipo de mensagem
    if(strcmp(username, me) == 0){
      if(receiver_name == NULL){
        printf(
          ANSI_COLOR_RED
          "Usuário destinatário inválido! Para saber como enviar uma "
          "mensagem, digite: help."
          ANSI_COLOR_GREEN
          "\n"
        );
        return 1;
      }else if(strcmp(receiver_name, "all") == 0){
        send_message_to_all_users();
        return 1;
      }else if(validate_destiny_user(receiver_name) == 0){
        printf(
          ANSI_COLOR_YELLOW
          "UNKNOWNUSER %s\n"
          ANSI_COLOR_GREEN,
          receiver_name
        );
        return 1;
      }else{
        if(person_message == NULL){
          printf(
            ANSI_COLOR_RED
            "Mensagem inválida! Para saber como enviar uma "
            "mensagem, digite: help."
            ANSI_COLOR_GREEN
            "\n"
          );
          return 1;
        }
      }
    }else{
      // Segundo tipo de mensagem
      if(username != NULL && receiver_name != NULL && person_message == NULL){
        if(validate_destiny_user(username)){
          person_message = receiver_name;
          receiver_name = username;
          username = me;

          memset(complete_message, 0, sizeof(complete_message));
          strcat(complete_message, username);
          strcat(complete_message, ":");
          strcat(complete_message, receiver_name);
          strcat(complete_message, ":");
          strcat(complete_message, person_message);
        }else{
          printf(
            ANSI_COLOR_RED
            "Usuário inválido! Para saber como enviar uma "
            "mensagem, digite: help."
            ANSI_COLOR_GREEN
            "\n"
          );
        }
      }else{
        if(username != NULL && receiver_name == NULL){
          printf(
            ANSI_COLOR_RED
            "Mensagem inválida! Para saber como enviar uma "
            "mensagem, digite: help."
            ANSI_COLOR_GREEN
            "\n"
          );
          return 1;
        }else if(username == NULL){
          printf(
            ANSI_COLOR_RED
            "Usuário destinatário inválido! Para saber como enviar uma "
            "mensagem, digite: help."
            ANSI_COLOR_GREEN
            "\n"
          );
          return 1;
        }
      }
    }

    strcpy(user_to_send, receiver_name);
    strcpy(final_message, complete_message);

    pthread_t thread;
    pthread_create(&thread, NULL, send_message_to_user, NULL);

    return 1;
}

void *send_message_to_user(){

  open_person_queue(user_to_send);

  int send, tries = 0;

  do{
    send = mq_timedsend (
      person_queue,
      (void *) &final_message,
      strlen(final_message),
      0,
      &abs_timeout
    );
    if(send == 0) break;
    sleep(1);
    printf(
      ANSI_COLOR_YELLOW
      "Tentativa %d de enviar mensagem à %s\n"
      ANSI_COLOR_GREEN,
      tries+1,
      user_to_send
    );
    tries++;
  }while(send < 0 && tries != 3);

  if (send < 0){
    printf(
      ANSI_COLOR_RED
      "Não foi possível enviar a mensagem à %s\n",
      user_to_send
    );
  }

  // printf("message: %s\n", complete_message);

  close_person_queue(user_to_send);
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

      if(strcmp(user_name, "all") == 0){
        // Broadcast message
        printf(ANSI_COLOR_BLUE "Broadcast de %s: %s" ANSI_COLOR_GREEN "\n", sender_name, sender_message);
      }else{
        // Private message
        printf(ANSI_COLOR_MAGENTA "%s: %s" ANSI_COLOR_GREEN "\n", sender_name, sender_message);
      }

      memset(user_name, 0, sizeof(user_name));
      memset(sender_name, 0, sizeof(sender_name));
      memset(complete_response, 0, sizeof(complete_response));
      memset(sender_message, 0, sizeof(sender_message));
  }

  pthread_exit(NULL);
}

void control_handler(int sig){
    signal(sig, SIG_IGN);
    printf(
      ANSI_COLOR_RED
      "\nPara parar a execução, você deverá digitar: sair"
      ANSI_COLOR_GREEN
      "\n"
    );
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

  char queue_destiny_name[] = "chat-";
  char header[6];
  char queue_aux[17];
  char *auxiliar_header = NULL;
  char *auxiliar_name = NULL;
  char user_name[11];
  int i, j;

  while((de = readdir(dr)) != NULL){
      queue_name = de->d_name;

      if(queue_name[0] == '.'){
        continue;
      }

      memset(queue_aux, 0, sizeof(queue_aux));
      strncpy(queue_aux, queue_name, 5);
      memset(header, 0, sizeof(header));
      strcpy(header, queue_aux);

      if(strcmp(header, queue_destiny_name) == 0){
        auxiliar_header = strtok(queue_name, "-");
        auxiliar_name = strtok(NULL, "-");

        memset(user_name, 0, sizeof(user_name));
        strcpy(user_name, auxiliar_name);

        if(strcmp(user_name, me) != 0 && validate_destiny_user(user_name)){
          strcpy(user_to_send, user_name);
          strcpy(final_message, complete_message);

          pthread_t thread;
          pthread_create(&thread, NULL, send_message_to_user, NULL);
        }
      }
      memset(auxiliar_name, 0, sizeof(auxiliar_name));
      memset(auxiliar_header, 0, sizeof(auxiliar_header));
  }

  closedir(dr);
}

void list_all_commands(){
  printf(ANSI_COLOR_RESET "Usage:\n");
  printf("  To send messages: USERNAME:DESTINY:MESSAGE or DESTINY:MESSAGE\n");
  printf("  To use commands: COMMAND\n\n");
  printf("A simple irc system to chat via ssh.\n\n");
  printf("Commands:\n");
  printf("  stats\t Shows user queue stats\n");
  printf("  list\t Lists all users connected\n");
  printf("  sair\t Ends application execution\n");
  printf(
    "  help\t Shows how to send messages and use commands"
    ANSI_COLOR_GREEN
    "\n"
  );
}
