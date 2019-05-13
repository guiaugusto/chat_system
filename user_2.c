#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

#define MY_QUEUE "/chat-guila"

char complete_message[522];
char complete_response[522];
char person_name[10];

mqd_t my_queue;
mqd_t person_queue;

char me[] = "guila";
struct mq_attr attr;

void open_queue(){
    if((my_queue = mq_open(MY_QUEUE, O_RDWR|O_CREAT, 0666, &attr)) < 0){
        perror("my_queue mq_open");
        exit(1);
    }

    mq_close(my_queue);

    if((my_queue = mq_open(MY_QUEUE, O_RDWR|O_CREAT)) < 0){
        perror("guila read mq_open");
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

void send_message(){
    memset(complete_message, 0, sizeof(complete_message));
    int i = 0, j = strlen(me) + 1;

    scanf("%[^\n]*c", complete_message);
    getchar();

    // Tratar o nome do usuário, quando inserido incorretamente
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
        perror("guila mq_send");
        exit(1);
    }
}

void *receive_messages(){
    char *sender_name;
    char *user_name;
    char *sender_message;

    while(1){
        int receive = mq_receive(my_queue, (void*) &complete_response, sizeof(complete_response), 0);

        char split[] = ":";
        char *ptr;
        sender_name = strtok(complete_response, split);
        user_name = strtok(NULL, split);
        sender_message = strtok(NULL, split);

        printf("%s: %s\n", sender_name, sender_message);

        memset(user_name, 0, sizeof(user_name));
        memset(sender_name, 0, sizeof(sender_name));
        memset(complete_response, 0, sizeof(complete_response));
        memset(sender_message, 0, sizeof(sender_message));
    }

    pthread_exit(NULL);
}

int main(){

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(complete_message);
    attr.mq_flags = 0;

    struct mq_attr attr;

    open_queue();

    pthread_t thread;

    pthread_create(&thread, NULL, receive_messages, NULL);

    while(1){
        send_message();
    }

    return 0;
}
