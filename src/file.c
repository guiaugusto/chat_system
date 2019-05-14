#include <dirent.h>
#include "file.h"

void list_all_files_in_mqueue_directory(){
    struct dirent *de;

    DIR *dr = opendir("/dev/mqueue");

    if(dr == NULL){
        printf("Could not open current directory" );
        return;
    }

    while((de = readdir(dr)) != NULL){
      printf("%s\n", de->d_name);
    }

    closedir(dr);
}
