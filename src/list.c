#include <list.h>

List *create_empty_list(){
  return NULL;
}

void insert_element(List **head, char *username){
  List *user = (List *) malloc(sizeof(List));
  strcpy(user->name, username);
  user->next = NULL;

  List *last = (*head);

  if((*head) == NULL){
    user->prev = NULL;
    (*head) = user;
    return;
  }

  while(last->next != NULL){
    last = last->next;
  }

  last->next = user;
  user->prev = last;
}

void remove_element(List **head, char *username){
  List *user_node = (*head);

  while(strcmp(user_node->name, username) != 0){
    user_node = user_node->next;
  }

  List *actual = user_node;

  if(user_node == (*head)){
    (user_node)->next->prev = NULL;
  }else if((user_node)->next == NULL){
    (user_node)->prev->next = NULL;
  }else{
    user_node->next->prev = user_node->prev->next;
    user_node->prev->next = user_node->next;
  }

  free(actual);

}

void imprime_lista(List *lista){
  printf("Elemento: %s\n", lista->name);

  if(lista->next != NULL){
    imprime_lista(lista->next);
  }
}

int check_user_in_list(List *list, char *username){
  if(list != NULL){
    if(strcmp(list->name, username) == 0){
      return 1;
    }else{
      check_user_in_list(list->next, username);
    }
  }else{
    return 0;
  }
}
