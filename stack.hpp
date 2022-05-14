#include <stdio.h>
#include <string>
#include <fcntl.h>


using namespace std;

typedef struct block_header
{
    int is_free; 
    size_t size; // 4 bytes
    block_header *next_block;
    block_header *prev_block; 
}block_header;
// malloc
void * my_malloc(size_t size);
// free
void my_free(void *item_to_free);

class node
{
private:
   string data;
   node *next;
public:
    node(){}
    node(string data){
        this->data = data;
    }
    string &get_data(){
        return this->data;
    }
    //void set_next_data(string &data);
    void set_next(node *n){
        this->next =n;
    }
    node *next_node(){
        return this->next;
    }
    void set_data(string &data){
        this->data = data;
    }
    ~node(){}
};

class stack
{
private:
    node *head;
    int size;
public:
    stack();
    ~stack();
    
    //PUSH
    void PUSH(string &data);
    //POP
    string POP();
    //TOP
    string TOP();
    //get_size
    int get_size(){
        return this->size;
    }
    void set_new_head(node &n){
        n.set_next(head);
        this->head = &n;
    }
    void file_de();
    //output of stack
    friend ostream & operator<<(ostream& out, const stack &st);
    
};