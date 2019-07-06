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
  char *user_name;
  char *init_queue_name;
  int i, j;

  printf("Lista de usuários disponíveis:\n");

  while((de = readdir(dr)) != NULL){
    queue_name = de->d_name;
    if(strcmp(queue_name, ".") != 0 && strcmp(queue_name, "..") != 0){
      init_queue_name = strtok(queue_name, "-");
      user_name = strtok(NULL, "-");
      if(strcmp(init_queue_name, "chat") == 0){
        printf("User -> %s\n", user_name);
      }else if(strcmp(init_queue_name, "canal") == 0){
        printf("Channel -> %s\n", user_name);
      }
    }
  }

  closedir(dr);
}

int validate_destiny_user(char *username, char *type){
  struct dirent *de;
  int exists = 0;
  char *queue_name;

  DIR *dr = opendir("/dev/mqueue");

  if(dr == NULL){
    printf("Diretório não pôde ser aberto.\n" );

    return -1;
  }

  char queue_destiny_name[18];
  strcpy(queue_destiny_name, type);
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
