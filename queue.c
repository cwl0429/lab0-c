#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head) {
        return NULL;
    }
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l) {
        return;
    }
    element_t *e, *safe;
    list_for_each_entry_safe (e, safe, l, list) {
        q_release_element(e);
    }
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *e = malloc(sizeof(element_t));
    if (!e) {
        return false;
    }
    e->value = strdup(s);
    if (!e->value) {
        free(e);
        return false;
    }
    list_add(&e->list, head);

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *e = malloc(sizeof(element_t));
    if (!e) {
        return false;
    }
    e->value = strdup(s);
    if (!e->value) {
        free(e);
        return false;
    }
    list_add_tail(&e->list, head);

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *e = list_first_entry(head, element_t, list);
    list_del(head->next);
    if (sp) {
        sp = strncpy(sp, e->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return e;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *e = list_last_entry(head, element_t, list);
    list_del(head->prev);
    if (sp) {
        sp = strncpy(sp, e->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return e;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head) {
        return 0;
    }

    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        len++;
    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return NULL if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return NULL;
    }

    struct list_head *fast;
    struct list_head **idir = &head->next;
    for (fast = head->next; fast != head && fast->next != head;
         fast = fast->next->next) {
        idir = &(*idir)->next;
    }

    element_t *e = list_entry(*idir, element_t, list);
    list_del(*idir);
    q_release_element(e);
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head) {
        return false;
    }

    element_t *e, *safe;
    list_for_each_entry_safe (e, safe, head, list) {
        if (e->list.next == head) {
            break;
        }
        if (strcmp(e->value, safe->value) == 0) {
            list_del(&e->list);
            q_release_element(e);
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head) {
        return;
    }
    struct list_head *l;
    for (l = head->next; (l != head) && (l->next != head); l = l->next) {
        list_move(l, l->next);
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *l, *safe;
    list_for_each_safe (l, safe, head) {
        list_move(l, head);
    }
}

struct list_head *mergeTwoLists(struct list_head *l1, struct list_head *l2)
{
    struct list_head *head = NULL, **ptr = &head, **node = NULL;
    for (node = NULL; l1 && l2; *node = (*node)->next) {
        element_t *e1 = list_entry(l1, element_t, list);
        element_t *e2 = list_entry(l2, element_t, list);

        node = strcmp(e1->value, e2->value) < 0 ? &l1 : &l2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) l1 | (uintptr_t) l2);
    return head;
}

struct list_head *mergesort(struct list_head *head)
{
    if (!head || !head->next) {
        return head;
    }
    struct list_head *fast, *slow;
    slow = head;
    for (fast = head->next; fast && fast->next; fast = fast->next->next) {
        slow = slow->next;
    }
    struct list_head *left, *right;
    right = slow->next;
    slow->next = NULL;

    left = mergesort(head);
    right = mergesort(right);

    return mergeTwoLists(left, right);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if ((!head || list_empty(head)) || list_is_singular(head)) {
        return;
    }

    head->prev->next = NULL;
    head->next = mergesort(head->next);
    struct list_head *l, *prev = head;
    for (l = head->next; l->next != NULL; l = l->next) {
        l->prev = prev;
        prev = l;
    }
    l->next = head;
    head->prev = l;
}
