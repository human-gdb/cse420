#include <stdio.h>
#include <stdlib.h>
#define nl printf("\n")
 
  struct ListNode{
      int val;
      struct ListNode *next;
  };
 
void AddLink(int val,struct ListNode *head);
void AddVal(int val,struct ListNode *n);


struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2){

struct ListNode *head = malloc (sizeof (struct  ListNode));    
int carry=0, sum=0,i=0;  
    struct ListNode* n1 = l1;
    struct ListNode* n2 = l2;
    struct ListNode *cur =  head;
    int val1,val2;
  

    while(n1 != NULL || n2 != NULL){

        val1 = (n1 == NULL) ? 0 : n1->val;
        val2 = (n2 == NULL) ? 0 : n2->val;

        sum = val1 + val2 + carry;
        carry = (sum>9) ? sum%9:0;
        if(i==0){
            cur->val = (sum>9) ? 9: sum;i++;
        }
        else 
        {
             sum = (sum>9) ? sum-9: sum;
            AddVal(sum,cur);
            i++;
        }

        if(n1->next == NULL){
            n1->next = malloc(sizeof(struct ListNode));
            n1-> val = 0;
            n1->next->next = NULL;
        }
        else 
            n1 = n1->next; 

        if(n2->next == NULL){
            n2->next = malloc(sizeof(struct ListNode));
            n2-> val = 0;
            n2->next->next = NULL;
        }
        else 
            n2 = n2->next;
    }
    return head;
}



void AddLink(int val,struct ListNode *head){

    struct ListNode *n = head;
   
    while(n->next != NULL)
        n=n->next;
    
    struct ListNode *newNode = malloc(sizeof(struct ListNode));
    n->next = newNode;
    newNode->val = val;
    newNode->next =  NULL;
}

void AddVal(int val,struct ListNode *n){

    struct ListNode *newNode = malloc(sizeof(struct ListNode));
    n->next = newNode;
    newNode->val = val;
    newNode->next =  NULL;
}


void ListOut(struct ListNode *head){
    struct ListNode *n = head;
    nl;
    while(n != NULL){
        printf(" %d ",n->val);
        n=n->next;
    }
}



int main(){

        struct ListNode *l1 = malloc(sizeof(struct ListNode));
        struct ListNode *l2 = malloc(sizeof(struct ListNode));
        l1->val = 9;
        l1->next = NULL;
        l2->val = 9;
        l2->next = NULL;
        AddLink(9,l1);
        AddLink(9,l1);
        AddLink(9,l1);
        AddLink(9,l1);
        AddLink(9,l1);

        AddLink(9,l2);
        AddLink(9,l2);
        AddLink(9,l2);
        
       // ListOut(l1);
        //ListOut(l2);
        
      ListOut(addTwoNumbers(l1,l2));

return 0;

}