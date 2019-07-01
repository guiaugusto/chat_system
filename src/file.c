#include "file.h"
#include "chat.h"
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
      for(i = 0; i < 6; i++)
        init_queue_name[i] = queue_name[i];

      if(strcmp(init_queue_name, "/chat-")){
        for(i = 5, j = 0; i < strlen(queue_name); i++, j++)
          user_name[j] = queue_name[i];

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
    printf("Diretório não pôde ser aberto.\n" );

    return -1;
  }

  char queue_destiny_name[15] = "chat-";
  strcat(queue_destiny_name, username);

  while((de = readdir(dr)) != NULL){
    queue_name = de->d_name;

    if(strcmp(queue_destiny_name, queue_name) == 0)
      exists = 1;
  }

  closedir(dr);

  if(!exists)
    return 0;
  else
    return 1;
}

void show_queue_information(char *filename){
  struct stat fileStat;
  char pathname[30];
  
  memset(pathname, 0, sizeof(pathname));
  strcat(pathname, "/dev/mqueue");
  strcat(pathname, filename);

  if(stat(pathname, &fileStat) < 0){
    perror("FileStat error\n");
    exit(1);
  }

  printf(
    ANSI_COLOR_YELLOW
    "--------------------------------------------------\n"
  );

  printf("\t\t  Informations\n");
  printf("--------------------------------------------------\n");
  printf("  File Size: \t\t%d bytes\n", (int) fileStat.st_size);
  printf("  Number of Links: \t%d\n", (int) fileStat.st_nlink);
  printf("  File inode: \t\t%d\n", (int) fileStat.st_ino);
  printf("  User Owner id: \t%d\n", (int) fileStat.st_uid);
  printf("  Created: \t\t%s\n", ctime(&fileStat.st_ctime));

  printf("  File Permissions: \t");
  printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
  printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
  printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
  printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
  printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
  printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
  printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
  printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
  printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
  printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
  printf("\n");
  printf(
    "--------------------------------------------------"
    ANSI_COLOR_GREEN
    "\n"
  );
}
