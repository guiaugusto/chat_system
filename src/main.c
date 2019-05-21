#include "user.h"
#include "file.h"
#include "chat.h"

int main(){

    set_chat_configuration();
    define_user_name();
    open_queues();

    pthread_t thread;

    pthread_create(&thread, NULL, receive_messages, NULL);

    while(send_message());

    return 0;
}
