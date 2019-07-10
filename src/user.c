#include "user.h"
#include "file.h"
#include "chat.h"

int define_user_name(){
    printf("Username (max 10 characters): ");

    char *auxiliar_name;
    scanf("%m[^\n]", &auxiliar_name);
    getchar();

    if(validate_username_size(auxiliar_name)){
      printf("O username informado é muito grande!\n");
      return 1;
    }else if(validate_destiny_user(auxiliar_name, "chat-")){
      printf("Este username já está sendo usado!\n");
      return 1;
    }else{
      strcpy(me, auxiliar_name);
      printf("Seu username é %s, não se esqueça!\n", me);
      return 0;
    }
}

void open_person_queue(char *person_name, char *type){
    char queue_name[16];
    int i, j;

    strcat(queue_name, type);
    strcat(queue_name, person_name);

    if((person_queue = mq_open(queue_name, O_WRONLY)) < 0){
        perror("queue name mq_open");
        exit(1);
    }
}

void close_person_queue(char *person_name){
    mq_close(person_queue);
}

int validate_username_size(char *username){
    int size = strlen(username);

    if(size <= 10){
      return 0;
    }else{
      return 1;
    }
}
