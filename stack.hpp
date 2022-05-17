#pragma once

#include <stdio.h>
#include <string>
#include <fcntl.h>


using namespace std;
struct node
{
   char data[2048];
   node *next;
};

struct stack
{
    node *head;
    int size = 0 ;
    char* top_data;
    char *addres_;
};
	//PUSH
    void PUSH(char * ch,stack *stack);
    //POP
    string POP(struct stack*);
    //TOP
    string TOP(struct stack );

void file_de();

void memory_to_shared(struct stack* stack);

void * my_malloc(stack * stack);

void my_free(void *item_to_free, stack *stack);
