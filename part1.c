#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#define N 3           //Each can be connected to max 3
#define SIZE 8

struct node
{
    int data;
    bool mark;
    int refCount;
    struct node *lptr[N];
};

struct heap
{
    struct node memory;
    struct heap *next;
};

struct heap *head = NULL;
struct node *ptr[SIZE];

struct node *insert(int size, struct node *lptr1, struct node *lptr2, struct node *lptr3)
{
    struct heap *ptr = malloc(sizeof(struct heap));
    ptr->memory.data = size;
    ptr->memory.mark = false;
    ptr->memory.lptr[0] = lptr1;
    ptr->memory.lptr[1] = lptr2;
    ptr->memory.lptr[2] = lptr3;
    ptr->memory.refCount = 0;
    ptr->next = head;
    head = ptr;

    struct node *mptr = &(ptr->memory);

    if (lptr1 != NULL)
    {
        lptr1->refCount += 1;
    }
    if (lptr2 != NULL)
    {
        lptr2->refCount += 1;
    }
    if (lptr3 != NULL)
    {
        lptr3->refCount += 1;
    }

    return mptr;
}

void printList()
{
    struct heap *mptr = head;

    while (mptr != NULL)
    {
        printf("(%d , refc=%d , mark=%d) ->", mptr->memory.data, mptr->memory.refCount, mptr->memory.mark);
        mptr = mptr->next;
    }
    printf("NULL");
}

void allocate(struct node *root)
{
    (root)->refCount += 1;
}

void print_Reference(struct node *root)
{
    if (root != NULL)
    {
        printf("%d(rfc=%d) ", root->data, root->refCount);
        print_Reference(root->lptr[0]);
        print_Reference(root->lptr[1]);
        print_Reference(root->lptr[2]);
    }
}

void garbage_Collect(struct node *ptr[], int n)
{
    int j = n - 1;
    struct heap *mptr = head;
    struct heap *prev = NULL;

    while (mptr != NULL)
    {
        if (mptr->memory.refCount == 0)
        {
            for (int i = 0; i < N; i++)
            {
                if (mptr->memory.lptr[i] != NULL)
                {
                    mptr->memory.lptr[i]->refCount -= 1;
                }
            }
            prev->next = mptr->next;
            free(mptr);
            mptr = prev;
            ptr[j] = NULL;
        }
        prev = mptr;
        mptr = mptr->next;
        j--;
    }
}

void mark_Nodes(struct node *root)
{
    if (root != NULL)
    {
        mark_Nodes(root->lptr[0]);
        mark_Nodes(root->lptr[1]);
        mark_Nodes(root->lptr[2]);
        root->mark = true;
    }
}

void sweep_Nodes()
{
    int j = SIZE - 1;
    struct heap *mptr = head;
    struct heap *prev = NULL;

    while (mptr != NULL)
    {
        if (mptr->memory.mark == false)
        {
            for (int i = 0; i < N; i++)
            {
                if (mptr->memory.lptr[i] != NULL)
                {
                    mptr->memory.lptr[i]->refCount -= 1;
                }
            }
            prev->next = mptr->next;
            free(mptr);
            mptr = prev;
            ptr[j] = NULL;
        }
        prev = mptr;
        mptr = mptr->next;
        j--;
    }
}

int main()
{
    ptr[0] = insert(10, NULL, NULL, NULL);      // Not pointing to any node
    ptr[1] = insert(9, NULL, NULL, NULL);       // Not pointing to any node
    ptr[2] = insert(2, NULL, NULL, NULL);       // Not pointing to any node
    ptr[3] = insert(8, ptr[1], NULL, NULL);     // connection with 9
    ptr[4] = insert(1, ptr[2], ptr[1], ptr[0]); // connection with 2,9,10
    ptr[5] = insert(3, ptr[3], ptr[0], NULL);   // connection with 8,10
    ptr[6] = insert(7, ptr[4], ptr[3], NULL);   // connection with 1,8
    ptr[7] = insert(5, ptr[4], NULL, NULL);     // connection with 1

    struct node *root1, *root2;
    root1 = ptr[7];                             //Pointing to 5
    allocate(root1);
    root2 = ptr[4];                             //Pointing to 1
    allocate(root2);

    for (int i = SIZE - 1; i >= 0; i--)
    {
        print_Reference(ptr[i]);
        printf("\n");
    }
    printf("\n------------------------------------------\n");
    printf("Heap connected to root node is : \n");
    print_Reference(root1);
    printf("\n");
    print_Reference(root2);

    printf("\nAfter garbage collection : \n");

    garbage_Collect(ptr, SIZE);

    for (int i = SIZE - 1; i >= 0; i--)
    {
        print_Reference(ptr[i]);
        if (ptr[i] != NULL)
            printf("\n");
    }

    printf("Complete list is : \n");
    printList();

    mark_Nodes(root1);
    mark_Nodes(root2);

    sweep_Nodes();

    printf("\nComplete list is : \n");
    printList();

    return 0;
}