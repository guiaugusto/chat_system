#include "user.h"
#include "file.h"
#include "chat.h"

void define_user_name(){
    printf("Username (max 10 characters): ");
    scanf("%[^\n]*c", me);
    getchar();
    printf("Seu username é %s, não se esqueça!\n", me);
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
