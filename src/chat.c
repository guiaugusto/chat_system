#include "chat.h"
#include "user.h"
#include "file.h"

void set_chat_configuration(){
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = sizeof(complete_message);
  attr.mq_flags = 0;
  abs_timeout.tv_sec = 1;
  counter = 0;
}

void set_group_configuration(struct group_attrib *group){
  group->group_attr.mq_maxmsg = 10;
  group->group_attr.mq_msgsize = sizeof(complete_message);
  group->group_attr.mq_flags = 0;
  group->users_list = create_empty_list();
  insert_element(&group->users_list, me);
}

void create_channel(char *channel){
  char queue_channel[18] = "/canal-";
  char queue_location[50] = "/dev/mqueue";
  char mode[] = "0622";
  int permission = strtol(mode, 0, 8);

  struct group_attrib group;
  strcpy(group.owner_nickname, me);
  strcat(queue_channel, channel);
  strcpy(group.group_name, queue_channel);
  set_group_configuration(&group);
  own_groups[counter] = group;
  if((own_groups[counter].queue = mq_open(queue_channel, O_CREAT, permission, &group)) < 0){
    perror("create channel error");
    exit(1);
  }

  mq_close(own_groups[counter].queue);

  if((own_groups[counter].queue = mq_open(queue_channel, O_RDWR)) < 0){
    perror("read channel error");
    exit(1);
  }

  strcat(queue_location, queue_channel);

  if(chmod(queue_location, permission) < 0){
    perror("chmod error");
    exit(1);
  }

  memset(queue_location, 0, sizeof(queue_location));
  pthread_create(&own_groups[counter].thread, NULL, receive_messages, (void*)own_groups[counter].queue);
  counter++;
}

void channel_callback(char *channel) {
  char queue_channel[18] = "/canal-";
  char queue_location[50] = "/dev/mqueue";
  char mode[] = "0622";
  int permission = strtol(mode, 0, 8);

  struct group_attrib group;
  strcpy(group.owner_nickname, me);
  strcat(queue_channel, channel);
  printf("queue_channel: %s\n", queue_channel);
  strcpy(group.group_name, queue_channel);
  set_group_configuration(&group);
  own_groups[counter] = group;

  if((own_groups[counter].queue = mq_open(queue_channel, O_RDWR, permission, &group)) < 0){
    perror("channel callback error");
    exit(1);
  }

  counter++;
}

void open_queues(){
  char queue[17] = "/chat-";
  char queue_location[50] = "/dev/mqueue";
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
  int is_channel = 0;
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
  }else if(strcmp(complete_message, "new_channel") == 0){
    char *group_name;
    printf("Nome do grupo: ");
    scanf("%m[^\n]", &group_name);
    getchar();
    create_channel(group_name);
    printf("Canal criado com sucesso!\n");
    return 1;
  }else if(strcmp(complete_message, "list_my_groups") == 0){
    printf("Lista de grupos criados:\n");
    for(int i = 0; i < counter; i++){
      printf("-> %s\n", own_groups[i].group_name);
    }
    return 1;
  }else if(strcmp(complete_message, "get_info") == 0){
    struct group_attrib info;
    char queue_location[50] = "/dev/mqueue";
    char mode[] = "0622";
    int permission = strtol(mode, 0, 8);

    printf("Informações:");
    printf("Owner: %s\n", info.owner_nickname);

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

  if(username == NULL){
    printf(
      ANSI_COLOR_RED
      "Mensagem inválida! Para saber como enviar uma "
      "mensagem, digite: help."
      ANSI_COLOR_GREEN
      "\n"
    );

    return 1;
  }else if(receiver_name == NULL){
    printf(
      ANSI_COLOR_RED
      "Usuário destinatário inválido! Para saber como enviar uma "
      "mensagem, digite: help."
      ANSI_COLOR_GREEN
      "\n"
    );

    return 1;
  }else if(person_message == NULL){
    printf(
      ANSI_COLOR_RED
      "Mensagem inválida! Para saber como enviar uma "
      "mensagem, digite: help."
      ANSI_COLOR_GREEN
      "\n"
    );

    return 1;
  }else{
    if(strcmp(receiver_name, "all") == 0){
      send_message_to_all_users();

      return 1;
    }else if(receiver_name[0] != '#' && !validate_destiny_user(receiver_name, "chat-")){
      printf(
        ANSI_COLOR_YELLOW
        "UNKNOWNUSER %s\n"
        ANSI_COLOR_GREEN,
        receiver_name
      );

      return 1;
    }else if(receiver_name[0] == '#'){
      char *channel_name;
      channel_name = strtok(receiver_name, "#");
      strcpy(receiver_name, channel_name);
      int i;
      is_channel = 1;
      if(!validate_destiny_user(receiver_name, "canal-")){
        printf(ANSI_COLOR_RED "Este canal não existe!" ANSI_COLOR_GREEN "\n");
      }else{
        //entra no canal
        // channel_callback(receiver_name);
      }
    }
  }

  strcpy(user_to_send, receiver_name);
  // strcpy(final_message, complete_message);

  if(is_channel) strcpy(channel_type, "/canal-");
  else strcpy(channel_type, "/chat-");

  char current_message[523];
  strcpy(current_message, complete_message);

  message_object *m_object = (message_object *) malloc (sizeof(message_object));
  strcpy(m_object->current_message, current_message);
  strcpy(m_object->user_to_send, user_to_send);
  strcpy(m_object->channel_type, channel_type);

  pthread_t thread;
  pthread_create(&thread, NULL, send_message_to_user, (void *)m_object);
  is_channel = 0;
  return 1;
}

void *send_message_to_user(message_object *m_object){
  open_person_queue(m_object->user_to_send, m_object->channel_type);

  int send, tries = 0;

  do{
    send = mq_timedsend (
      person_queue,
      (void *)m_object->current_message,
      strlen(m_object->current_message),
      0,
      &abs_timeout
    );

    if(send == 0)
      break;

    sleep(1);

    printf(
      ANSI_COLOR_YELLOW
      "Tentativa %d de enviar mensagem à %s\n"
      ANSI_COLOR_GREEN,
      tries+1,
      m_object->user_to_send
    );

    tries++;
  }while(send < 0 && tries != 3);

  if (send < 0){
    printf(
      ANSI_COLOR_RED
      "Não foi possível enviar a mensagem à %s\n",
      m_object->user_to_send
    );
  }

  close_person_queue(m_object->user_to_send);
}

void *receive_messages(mqd_t myqueue){
  char *sender_name;
  char *user_name;
  char *sender_message;
  char response[527];

  while(1){
    mq_receive(myqueue, (void*) &response, sizeof(response), 0);
    char split[] = ":";

    sender_name = strtok(response, split);
    user_name = strtok(NULL, split);
    sender_message = strtok(NULL, split);

    if(strcmp(user_name, "all") == 0){
      // Broadcast message
      printf(ANSI_COLOR_BLUE "Broadcast de %s: %s" ANSI_COLOR_GREEN "\n", sender_name, sender_message);
    }else if(user_name[0] == '#'){
      char group_name[17] = "/canal-";
      user_name = strtok(user_name, "#");
      strcat(group_name, user_name);

      if(validate_destiny_user(user_name, "canal-")){
        int is_member = 0, i, j;

        for(i = 0; i < counter; i++){
          if(strcmp(own_groups[i].group_name, group_name) == 0){
            break;
          }
        }

        is_member = check_user_in_list(own_groups[i].users_list, sender_name);

        if(is_member){
          if(strcmp(sender_message, "join") == 0){
            // printf(ANSI_COLOR_YELLOW "Você já pertence a este grupo!" ANSI_COLOR_GREEN "\n");
          }else if(strcmp(sender_message, "leave") == 0){
            remove_element(&own_groups[i].users_list, sender_name);
          }else{
            char current_message[523];
            char final_response[523];
            char channel[20] = "#";
            List *list = own_groups[i].users_list;

            while(list != NULL){
              strcat(channel, user_name);
              strcat(current_message, channel);
              strcat(current_message, ":");
              strcat(current_message, list->name);
              strcat(current_message, ":");
              strcat(current_message, "<");
              strcat(current_message, sender_name);
              strcat(current_message, ">");
              strcat(current_message, sender_message);

              strcpy(user_to_send, list->name);
              strcpy(channel_type, "/chat-");
              pthread_t thread;

              message_object *m_object = (message_object *) malloc (sizeof(message_object));
              strcpy(m_object->current_message, current_message);
              strcpy(m_object->user_to_send, list->name);
              strcpy(m_object->channel_type, "/chat-");

              pthread_create(&thread, NULL, send_message_to_user, (void*)m_object);
              memset(current_message, 0, sizeof(current_message));
              memset(channel, 0, sizeof(channel));
              channel[0] = '#';
              list = list->next;
            }
          }
        }else{
          if(strcmp(sender_message, "join") == 0){
            insert_element(&own_groups[i].users_list, sender_name);
          }else{
            char current_message[523];
            char channel[20] = "#";
            strcat(channel, user_name);

            strcat(current_message, sender_name);
            strcat(current_message, ":");
            strcat(current_message, channel);
            strcat(current_message, ":");
            strcat(current_message, "NOT A MEMBER");

            printf("%s\n", current_message);

            strcpy(user_to_send, sender_name);
            strcpy(channel_type, "/chat-");
            pthread_t thread;

            struct message_object *m_object;
            strcpy(m_object->current_message, current_message);
            strcpy(m_object->user_to_send, sender_name);
            strcpy(m_object->channel_type, "/chat-");

            pthread_create(&thread, NULL, send_message_to_user, (void*)m_object);
          }
        }
      }
    }else{
      if(sender_name[0] == '#'){
        char *sender;
        char f_message[523];
        strcpy(f_message, sender_message);

        sender = strtok(sender_message, "<");
        sender = strtok(sender, ">");

        sender_message = strtok(f_message, ">");
        sender_message = strtok(NULL, ">");

        sender_name = strtok(sender_name, "#");
        printf(ANSI_COLOR_ORANGE "%s(%s): %s" ANSI_COLOR_GREEN "\n", sender_name, sender, sender_message);
      }else{
        // Private message
        printf(ANSI_COLOR_MAGENTA "%s: %s" ANSI_COLOR_GREEN "\n", sender_name, sender_message);
      }
    }

    memset(user_name, 0, sizeof(user_name));
    memset(sender_name, 0, sizeof(sender_name));
    memset(response, 0, sizeof(response));
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

    if(queue_name[0] == '.')
      continue;

    memset(queue_aux, 0, sizeof(queue_aux));
    strncpy(queue_aux, queue_name, 5);

    memset(header, 0, sizeof(header));
    strcpy(header, queue_aux);

    if(strcmp(header, queue_destiny_name) == 0){
      auxiliar_header = strtok(queue_name, "-");
      auxiliar_name = strtok(NULL, "-");

      memset(user_name, 0, sizeof(user_name));
      strcpy(user_name, auxiliar_name);

      if(strcmp(user_name, me) != 0 && validate_destiny_user(user_name, "chat-")){
        strcpy(user_to_send, user_name);
        char current_message[523];
        strcpy(current_message, complete_message);

        message_object *m_object = (message_object *) malloc (sizeof(message_object));
        strcpy(m_object->current_message, current_message);
        strcpy(m_object->user_to_send, user_to_send);
        strcpy(m_object->channel_type, "/chat-");

        pthread_t thread;
        pthread_create(&thread, NULL, send_message_to_user, (void *)m_object);
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
  printf("  list_my_groups\t List all groups created\n");
  printf(
    "  help\t Shows how to send messages and use commands"
    ANSI_COLOR_GREEN
    "\n"
  );
}
