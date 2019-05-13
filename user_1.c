#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

#define MY_QUEUE "/chat-guiaugusto"

char complete_message[522];
char complete_response[522];
char person_name[10];
char message[500];

mqd_t my_queue;
mqd_t person_queue;

char me[] = "guiaugusto";
int counter;
struct mq_attr attr;

void open_queues(){
    if((my_queue = mq_open(MY_QUEUE, O_RDWR|O_CREAT, 0666, &attr)) < 0){
        perror("mq_open");
        exit(1);
    }

    mq_close(my_queue);

    if((my_queue = mq_open(MY_QUEUE, O_RDWR|O_CREAT)) < 0){
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

void send_message(){
    scanf("%s", person_name);
    getchar();

    memset(message, 0, sizeof(message));
    memset(complete_message, 0, sizeof(complete_message));

    int counter_message = 0;
    char caracter;

    while(1){
      scanf("%c", &caracter);

      if(caracter == '\n' || counter_message > 500){
        break;
      }

      message[counter_message] = caracter;
      counter_message++;
    }

    counter_message = 0;

    open_person_queue(person_name);

    for(counter = 0; counter < strlen(me); counter++){
        complete_message[counter] = me[counter];
    }

    complete_message[strlen(me)] = ':';

    int i = 0;
    int current_size = strlen(me) + 1;

    for(counter = current_size, i = 0; i < strlen(person_name); counter++, i++){
        complete_message[counter] = person_name[i];
    }

    complete_message[counter] = ':';

    for(counter = strlen(complete_message), i = 0; i < strlen(message); counter++, i++){
        complete_message[counter] = message[i];
    }

    int send = mq_send (person_queue, (void *) &complete_message, strlen(complete_message), 0);

    if (send < 0){
        perror("guiaugusto mq_send");
        exit(1);
    }

    counter = 0;
    complete_message[0] = '\0';
}

void *receive_messages(){
    char sender_name[10];
    char user_name[10];
    char sender_message[500];
    int i, j;

    while(1){
        int receive = mq_receive(my_queue, (void*) &complete_response, sizeof(complete_response), 0);

        if(receive < 0){
            perror("mq_receive:");
            exit(1);
        }

        for(i = 0; complete_response[i] != ':'; i++){
            sender_name[i] = complete_response[i];
        }

        i++;

        for(j = 0; complete_response[i] != ':'; i++, j++){
            user_name[j] = complete_response[i];
        }

        i++;

        for(j = 0; i < strlen(complete_response); i++, j++){
            sender_message[j] = complete_response[i];
        }

        if(complete_response != ""){
            printf("(%s): %s\n", sender_name, sender_message);
            complete_response[0] = '\0';
        }

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

    open_queues();

    pthread_t thread;

    pthread_create(&thread, NULL, receive_messages, NULL);

    while(1){
        send_message();
    }


    return 0;
}
