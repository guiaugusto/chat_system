#include "user.h"
#include "file.h"
#include "chat.h"

int define_user_name(){
    printf("Username (max 10 characters): ");
    scanf("%[^\n]*c", me);
    getchar();
    if(validate_destiny_user(me)){
     printf("Este username já está sendo usado!\n");
      return 1;
    }else{
      printf("Seu username é %s, não se esqueça!\n", me);
      return 0;
    }
}

void open_person_queue(char *person_name){
    char queue_name[16] = "/chat-";
    int i, j;

    strcat(queue_name, person_name);

    if((person_queue = mq_open(queue_name, O_WRONLY)) < 0){
        perror("person name mq_open");
        exit(1);
    }
}

void close_person_queue(char *person_name){
    mq_close(person_queue);
}
