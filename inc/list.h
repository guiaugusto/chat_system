#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct List {
  char name[20];
  struct List *next;
  struct List *prev;
}List;

List *create_empty_list();
int check_user_in_list(List *list, char *username);
void insert_element(List **head, char *username);
void remove_element(List **head, char *username);
void imprime_lista(List *head);

#endif
