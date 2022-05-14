#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <mutex>
#include "stack.hpp"
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

//this lock may be modified or released using any of these file descriptors//
struct flock lock_;
block_header *head=NULL;
int fd;

//
void stack::file_de(){
    fd = open("example.txt", O_WRONLY | O_CREAT);
}

void * my_malloc(size_t size){
    void* new_block;
    intptr_t* add=(intptr_t*) sbrk(0);
    block_header *iter=head;
    while (iter)
    {
        if (iter->is_free==1&&iter->size>=size)
        {
           new_block=(void*)(ulong)iter + sizeof(block_header);
           iter->is_free=0;
           return new_block+1;
        }
        else
            iter=iter->next_block;
        
    }
    if (sbrk(sizeof(block_header) + size) == (void *) -1 )
    {
        return NULL;
    }
    iter=(block_header*)add;
    iter->is_free=0;
    iter->size=size;
    iter->next_block=head;
    head=iter;
    new_block=(void*)iter + sizeof(block_header);
    return new_block+1;
}

void my_free(void *item_to_free){
    block_header* del_block=(block_header*)(item_to_free-sizeof(block_header));
    del_block->is_free=1;
}

stack::stack(){
    node *stack_point = NULL;
    this->head = stack_point;
    this->size = 0;
}
stack::~stack(){}

void stack::PUSH(string &data){
    lock_.l_type = F_WRLCK;
    fcntl(fd, F_SETLKW, &lock_);
    node *n = (struct node*)my_malloc(sizeof(struct node));
    n->set_data(data);
    this->set_new_head(*n);
    lock_.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock_);

}

string stack::POP(){
    lock_.l_type = F_WRLCK;
    fcntl(fd, F_SETLKW, &lock_);
    node *temp = this->head;
    this->head = temp->next_node();
    string data = temp->get_data();
    my_free(temp);
    lock_.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock_);
    return data;
}

string stack::TOP(){
    lock_.l_type = F_WRLCK;
    fcntl(fd, F_SETLKW, &lock_);
    string top="OUTPUT: "+ this->head->get_data();
    lock_.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock_);
    return top;
}
// node::node(){

//  }
// int main(){
//     stack st;
//     string s = "ilan";
//     st.PUSH(s);
//     string ilan = st.POP();
//      std::cout<<ilan<<std::endl;
//     string s1 = "ilan2";
//     st.PUSH(s1);
//     string ilan2 = st.POP();
//     std::cout<<ilan2<<std::endl;
//     string s2 = "ilan3";
//      st.PUSH(s2);
//     string ilan3 = st.POP();
//      std::cout<<ilan3<<std::endl;
//     st.PUSH(s);
//     st.PUSH(s1);
//     st.PUSH(s2);
//     string test_top = st.TOP();
//      string ilan4 = st.POP();
//      std::cout<<test_top<<std::endl;
// }