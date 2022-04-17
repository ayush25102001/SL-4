#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int SIZE = 2000;
int check_tag_uniqueness(char s[]);

//Struct Node of free list
struct free_list{
    struct free_list* next;
    struct free_list* prev;
    int size;
    int start;
};

//Head of Freelist 
struct free_list* FreeHead;

//Struct Node of allocated list
struct alloc{
    struct alloc* next;
    struct alloc* prev;
    int size;
    int start;
    char tag[30];
};

//Head of Alloclist
struct alloc* allocHead;

int check_tag_uniqueness(char s[])
{
    int flag = 1;
    struct alloc* ptr = allocHead;
    while (ptr && flag)
    {
        if (strcmp(ptr->tag , s)==0)
        {
              return 0;
        }
        ptr = ptr->next;
    }
    return 1;
}
void allocate()
{
    int curr;
    char tag[100];
    printf("Enter size you want to allocate(integer format)\n");
    scanf("%d",&curr);
    printf("Enter tag for allocated block\n");
    scanf("%s",tag);
    
    //Checking uniqueness of entered tag
    if(!check_tag_uniqueness(tag))
    {
    	printf("Tag already used select another tag. \n");
    	return;
	}
	
    struct free_list* ptr = FreeHead;
    
    //Finding the appropriate free block
    while (ptr && ptr->size < curr)
    {
        ptr = ptr->next;
    }
    
    //If we reach end of list but can't find suitable free block
    if (ptr == NULL)
    {
        printf("\nMEMORY CAN'T BE ALLOCATED!!\n\n");
    }
    else
    {
        //Making changes to ptr through which memory is to be allocated
        //And accordingly creating new temp node to insert in list

        //If size of free block is greater than required size
        if (ptr->size>curr)
        {
            struct free_list* temp = (struct free_list*)malloc(sizeof(struct free_list)); 
            
            //Starting point of the next free location
            temp->start = ptr->start + curr;
            temp->next = temp->prev = NULL;
            
            //Condition when memory is allocated from head node itself
            if (ptr == FreeHead)
            {
                temp->next = FreeHead->next;
                FreeHead = temp;
            }
            else
            {
                (ptr->prev)->next = temp;
                temp->next = ptr->next;
                temp->prev = ptr->prev;
                if(ptr->next)
                {
                	ptr->next->prev = temp;
				}
            }
            temp->size = ptr->size - curr;
        }
        //If size of free block equals required size
        else
        {
            if (ptr->prev)
            {
                ptr->prev->next = ptr->next;
                if(ptr->next)
                {
                	ptr->next->prev = ptr->prev;
				}
            }
            else
            {
                FreeHead = ptr->next;
                if(FreeHead)
                {
                	FreeHead->prev = NULL;
				}
            }
        }

        //---------------FREE LIST OPERATION DONE----------------

        //Declaring and Initializing Node that we want to add in list
        struct alloc* alloc_curr = (struct alloc*)malloc(sizeof(struct alloc));
        alloc_curr->next = NULL;
        alloc_curr->prev = NULL;
        alloc_curr->size = curr;
        strcpy(alloc_curr->tag , tag);
        alloc_curr->start = ptr->start;
        free(ptr);
        
        //Case when allocated node is first one
        //Current allocated block will be the head of the alloc list
        if (allocHead == NULL)
        {
        	allocHead = alloc_curr;
		}
        else
        {
            struct alloc* tempp=allocHead;
            
            //Finding the position where to allocate
            while (tempp->next && tempp->start < alloc_curr->start)
            {
                tempp=tempp->next;
            }
            //When node is to be allocated at first position itself
            if (tempp==allocHead && tempp->start>alloc_curr->start)
            {
                alloc_curr->next=tempp;
                tempp->prev=alloc_curr;
                allocHead=alloc_curr;
            }
            //Condition when node to allocate is at last position
            else if (tempp->next == NULL && tempp->start < alloc_curr->start)
            {
                tempp->next = alloc_curr;
                alloc_curr->prev = tempp;
                alloc_curr->next = NULL;
            }
            //When Node is to be inserted in between
            else
            {
                (tempp->prev)->next=alloc_curr;
                alloc_curr->prev=tempp->prev;
                alloc_curr->next=tempp;
                tempp->prev=alloc_curr;
            }
        }
        //Allocation details
        printf("Allocated from index: %d to %d \n" ,alloc_curr->start ,alloc_curr->start + alloc_curr->size);
        printf("\n");
    }
}
void merge(struct free_list* fptr)
{
    //If the previous adjacent block is free
    if (fptr->prev)
    {
        if (fptr->prev->start + fptr->prev->size == fptr->start)
        {
            //Changing size of previous node as we are merging
            fptr->prev->size = fptr->prev->size + fptr->size;
            fptr->prev->next = fptr->next;
            if(fptr->next)
            {
            	fptr->next->prev = fptr->prev;
			}
            struct free_list* temp = fptr;
            fptr = fptr->prev;
            free(temp);
        }
    }
    
    //If the next adjacent block is free
    if(fptr->next)
    {
        if(fptr->start + fptr->size == fptr->next->start)
        {
            //Changing size of current node as we are merging
            fptr->size += fptr->next->size;
            fptr->next = fptr->next->next;
            if(fptr->next)
            {
            	fptr->next->prev = fptr;
			}
        }
    }
}
void deallocate(char tag[])
{
	//ptr initially pointing to the head of alloc list
    struct alloc* ptr = allocHead;
    
    //Finding the block with the specified tag
    while (ptr && strcmp(ptr->tag,tag))
    {
        ptr = ptr->next;
    }
    
    //Condition if given tag doesn't exist
    if (ptr == 0)
    {
        printf("Tag entered is incorrect\n");
    }
    else
    {
    	//Freelist node that is to be inserted in freelist
        struct free_list* fptr = (struct free_list*)malloc(sizeof(struct free_list));
        fptr->next = fptr->prev = 0;
        fptr->size = ptr->size;     //200
        fptr->start = ptr->start;  //300
        
        //If deallocated memory is first node itself
        //Same as deleting the headNode of the linklist
        if (ptr == allocHead)
        {
        	allocHead = ptr->next;
		}
        else
        {
        	//Deleting the node from the alloc list
        	//Same as deletion from a doubly linked list
            ptr->prev->next = ptr->next;
            
            //If the deleted node is not the last node
            if (ptr->next)
            {
            	ptr->next->prev = ptr->prev;
			}
                
        }
        //--------------Alteration of the alloc list is done-------------
        
        //Freed block must be appropriately inserted in freelist
        struct free_list* temp = FreeHead;
        
        //If list is NULL, initialize it
        // i.e Entire memory of 2000 units is allocated
        if (FreeHead==0)
        {
            FreeHead=fptr;
        }
        else
        {
            while (temp && temp->next && temp->start < fptr->start)
            {
                temp = temp->next;
            }
            
            //Case when freed block is to be inserted at end
            if (temp && temp->next==0 && temp->start < fptr->start)
            {
                temp->next = fptr;
                fptr->prev = temp;
            }
            else
            {
                //When freelist node is to be inserted at start
                //Freed block would become the new head of the freelist
                if (temp && (temp == FreeHead))
                {
                    fptr->next = temp;
                    temp->prev = fptr;
                    FreeHead= fptr;
                }
                //When freelist node is to be inserted in between
                else if (temp)
                {
                    fptr->prev = temp->prev;
                    temp->prev->next = fptr;
                    temp->prev = fptr;
                    fptr->next = temp;
                }

            }
        }
        //Merging continguous free blocks
        merge(fptr);
    }
}

//Printing allocated list
void print_allocated()
{
    struct alloc* ptr = allocHead;
    while(ptr)
    {
        printf("%d to %d \n",ptr->start,ptr->start + ptr->size);
        ptr = ptr->next;
    }
}

//Printing free list
void print_free()
{
    struct free_list* ptr = FreeHead;
    while (ptr)
    {
        printf("%d to %d \n",ptr->start,ptr->start + ptr->size);
        ptr = ptr->next;
    }
}

int main()
{
    int choice=1;

    //Initializing head node of Free list
    //Initially pointing to the free memory bolck of 2000 units
    FreeHead = (struct free_list*)malloc(sizeof(struct free_list));
    FreeHead->next = FreeHead->prev = NULL;
    FreeHead->size = SIZE;
    FreeHead->start = 0;
    
    //Initializing allocated list
    allocHead = NULL;
    
    while (choice)
    {
    	printf("0:EXIT\n");
        printf("1.Allocate Memory\n");
        printf("2:Deallocate Memory\n");
        printf("Enter your choice:");
        scanf("%d",&choice);
        printf("-------------------------------------------------------------------------------------\n\n");
        switch (choice)
        {
        case 0:break;
        case 1:allocate();
            break;
        case 2:
        {
		   char tag[100];
           printf("Enter tag of block you want to delete\n");
           scanf("%s", tag);
           deallocate(tag);
           break;
        }
           default:printf("Invalid choice\n");
        }
        printf("Memory state\n");
        printf("\n");
        printf("Allocated list\n");
        print_allocated();
        printf("\n");
        printf("Free list\n");
        print_free();
        printf("\n"); 
    }
}
