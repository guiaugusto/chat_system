#include "user.h"

int main(){

    set_chat_configuration();
    define_user_name();
    open_queues();

    pthread_t thread;

    pthread_create(&thread, NULL, receive_messages, NULL);

    while(1){
        send_message();
    }


    return 0;
}
