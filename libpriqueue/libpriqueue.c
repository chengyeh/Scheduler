/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.
  
  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
    q->m_front = NULL;
    q->m_size = 0;
    q->m_comparer = comparer;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
    if(q->m_size == 0) {
       Node* temp = malloc(sizeof(Node));
       temp->m_value = ptr;
       temp->m_next = NULL;
       q->m_front = temp;
       q->m_size++;
       return 0;
    }
    else {
        Node* new = malloc(sizeof(Node));
        new->m_value = ptr;
        new->m_next = NULL;
        
        Node* temp = q->m_front;
        q->m_front = new;
        (q->m_front)->m_next = temp;
        temp = q->m_front;
        q->m_size++;

        int index = 0;
        while(temp->m_next != NULL) {
        	if(q->m_comparer(temp->m_value, (temp->m_next)->m_value) > 0) {
        		void* tempValue = (temp->m_next)->m_value;
        		(temp->m_next)->m_value = temp->m_value;
        		temp->m_value = tempValue;
        		index++;
        	}
        	temp = temp->m_next;
        }
        return index;
    }
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
	if(q->m_size != 0){
		return q->m_front;
	}
	else {
		return NULL;
	}
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
	if(q->m_size != 0){
		Node* temp = q->m_front;
		if((q->m_front)->m_next != NULL) {
			q->m_front = (q->m_front)->m_next;
		}
		else {
			q->m_front = NULL;
		}

		void* tempValue = temp->m_value;
		free(temp);
		q->m_size--;
		return tempValue;
	}
	else {
		return NULL;
	}
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
	if((index >= 0) && (index < q->m_size)){
		Node* temp = q->m_front;
		for(int i = 0; i < index; i++) {
			temp = temp->m_next;
		}
		void* element = temp->m_value;
		return element;
	}
	else {
		return NULL;
	}
}


/**
  Removes all instances of ptr from the queue. 
  
  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
	if(q->m_size != 0) {
		int element_removed = 0;
		Node* temp = q->m_front;
		Node* prev = q->m_front;
		while(temp != NULL) {
			if(temp->m_value == ptr) {
				if(temp == q->m_front) {
					q->m_front = temp->m_next;
					prev = q->m_front;
					free(temp);
					temp = prev;
				}
				else {
					prev->m_next = temp->m_next;
					free(temp);
					temp = prev->m_next;
				}
				q->m_size--;
				element_removed++;
			}
			else {
				prev = temp;
				temp = temp->m_next;
			}
		}
		return element_removed;
	}
	else {
		return 0;
	}
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
	if((index >= 0) && (index < q->m_size)) {
		Node* temp = q->m_front;
		Node* prev = q->m_front;
		void* tempValue = temp->m_value;

		for(int i = 0; i < index; i++) {
			prev = temp;
			temp = temp->m_next;
		}

		if(temp == q->m_front) {
			q->m_front = temp->m_next;
			prev = NULL;
			free(temp);
			temp = prev;
		}
		else{
			tempValue = temp->m_value;
			prev->m_next = temp->m_next;
			free(temp);
			temp = NULL;
		}
		q->m_size--;
		return tempValue;
	}
	else {
		return NULL;
	}
}


/**
  Returns the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->m_size;
}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
	while((q->m_front)->m_next != NULL) {
		Node* temp = (q->m_front)->m_next;
		free(q->m_front);
		q->m_size--;
		q->m_front = temp;
	}
	free(q->m_front);
	q->m_size--;
	q->m_front = NULL;
}
