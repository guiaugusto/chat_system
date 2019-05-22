#include <dirent.h>
#include "file.h"

void show_all_users_online(){
    struct dirent *de;

    DIR *dr = opendir("/dev/mqueue");

    if(dr == NULL){
        printf("Could not open current directory" );
        return;
    }

    char *queue_name;
    char user_name[10];
    char init_queue_name[6];
    int i, j;

    printf("Lista de usuários disponíveis:\n");

    while((de = readdir(dr)) != NULL){
      queue_name = de->d_name;

      if(strlen(queue_name) > 6){
        for(i = 0; i < 6; i++){
            init_queue_name[i] = queue_name[i];
        }

        if(strcmp(init_queue_name, "/chat-")){
          for(i = 5, j = 0; i < strlen(queue_name); i++, j++){
            user_name[j] = queue_name[i];
          }

          printf("-> %s\n", user_name);
        }
      }

      memset(queue_name, 0, sizeof(queue_name));
      memset(user_name, 0, sizeof(user_name));
      memset(init_queue_name, 0, sizeof(init_queue_name));
    }

    closedir(dr);
}

int validate_destiny_user(char *username){
    struct dirent *de;
    int exists = 0;
    char *queue_name;

    DIR *dr = opendir("/dev/mqueue");

    if(dr == NULL){
        printf("Diretório não pôde ser aberto." );
        return -1;
    }

    char queue_destiny_name[15] = "chat-";
    strcat(queue_destiny_name, username);

    while((de = readdir(dr)) != NULL){
        queue_name = de->d_name;
        if(strcmp(queue_destiny_name, queue_name) == 0){
            exists = 1;
        }
    }

    if(!exists){
        return 0;
    }else{
        return 1;
    }
}
