#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */
/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        q_release_element(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }
    element_t *element = malloc(sizeof(element_t));
    if (!element) {
        return false;
    }
    element->value = strdup(s);
    if (!element->value) {
        q_release_element(element);
        return false;
    }
    list_add(&element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }
    element_t *element = malloc(sizeof(element_t));
    if (!element) {
        return false;
    }
    element->value = strdup(s);
    if (!element->value) {
        q_release_element(element);
        return false;
    }
    list_add_tail(&element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head)) {
        return NULL;
    }
    struct list_head *node = head->next;
    element_t *element = list_first_entry(head, element_t, list);
    size_t len = strlen(element->value);
    // 確保不會超過緩衝區大小
    if (len >= bufsize) {
        len = bufsize - 1;
    }
    strncpy(sp, element->value, len);
    sp[len] = '\0';  // 確保字符串以空字符結尾
    list_del(node);
    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head)) {
        return NULL;
    }
    struct list_head *node = head->prev;
    element_t *element = list_last_entry(head, element_t, list);
    strncpy(sp, element->value, bufsize);
    list_del(node);
    return element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (list_empty(head)) {
        return 0;
    }
    int count = 0;
    struct list_head *node, *next;
    list_for_each_safe (node, next, head) {
        count++;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (list_empty(head)) {
        return false;
    }
    struct list_head *slow = head->next;
    struct list_head *fast = slow->next;
    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }
    list_del(slow);
    element_t *element = list_entry(slow, element_t, list);
    free(element->value);
    free(element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (list_empty(head)) {
        return false;
    }
    struct list_head *left = head->next;
    struct list_head *right = left;
    const char *left_value = list_entry(left, element_t, list)->value;
    while (right->next != head && right != head) {
        right = right->next;
        const char *right_value = list_entry(right, element_t, list)->value;
        if (strcmp(left_value, right_value) == 0) {
            while (strcmp(left_value, right_value) == 0) {
                struct list_head *temp = right;
                right = right->next;
                list_del(temp);
                q_release_element(list_entry(temp, element_t, list));
                if (right != head) {
                    right_value = list_entry(right, element_t, list)->value;
                } else {
                    break;
                }
            }
            struct list_head *temp = left;
            list_del(temp);
            q_release_element(list_entry(temp, element_t, list));
        }
        left = right;
        left_value = right_value;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *left = head->next;
    struct list_head *right = left->next;
    while (left != head && right != head) {
        right->prev = left->prev;
        right->prev->next = right;
        left->next = right->next;
        left->next->prev = left;
        right->next = left;
        left->prev = right;
        left = left->next;
        right = left->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *it, *safe;
    list_for_each_safe (it, safe, head)
        list_move(it, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (q_size(head) < k) {
        return;
    }
    struct list_head *left = head->next;
    struct list_head *right = left;
    while (right->next != head) {
        for (int i = 0; i < k - 1; i++) {
            right = right->next;
            if (right == head) {
                return;
            }
        }
        struct list_head *next = right->next;
        struct list_head *prev = left->prev;
        right->next = left;
        left->prev = right;
        q_reverse(right);
        prev->next = right;
        right->prev = prev;
        left->next = next;
        next->prev = left;
        left = next;
        right = left;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (list_empty(head) || list_is_singular(head))
        return;

    struct list_head *pivot = head->next;
    struct list_head left, right;
    INIT_LIST_HEAD(&left);
    INIT_LIST_HEAD(&right);

    struct list_head *node, *tmp;
    list_for_each_safe (node, tmp, head) {
        if (node == pivot)
            continue;
        const element_t *element = list_entry(node, element_t, list);
        const element_t *pivot_element = list_entry(pivot, element_t, list);
        if ((descend && strcmp(element->value, pivot_element->value) > 0) ||
            (!descend && strcmp(element->value, pivot_element->value) < 0)) {
            list_move_tail(node, &left);
        } else {
            list_move_tail(node, &right);
        }
    }

    q_sort(&left, descend);
    q_sort(&right, descend);

    list_splice_tail(&left, head);
    list_move_tail(pivot, head);
    list_splice_tail(&right, head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (list_empty(head)) {
        return 0;
    }
    if (list_is_singular(head)) {
        return 1;
    }
    int cnt = 0;
    struct list_head *left = head->next;
    struct list_head *right = left->next;
    while (right != head) {
        while (strcmp(list_entry(left, element_t, list)->value,
                      list_entry(right, element_t, list)->value) > 0) {
            struct list_head *temp = left;
            left = left->prev;
            list_del(temp);
            q_release_element(list_entry(temp, element_t, list));
            if (left == head) {
                break;
            }
            cnt--;
        }
        left = right;
        right = right->next;
        cnt++;
    }
    return cnt;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (list_empty(head)) {
        return 0;
    }
    if (list_is_singular(head)) {
        return 1;
    }
    int cnt = 0;
    struct list_head *left = head->next;
    struct list_head *right = left->next;
    while (right != head) {
        while (strcmp(list_entry(left, element_t, list)->value,
                      list_entry(right, element_t, list)->value) < 0) {
            struct list_head *temp = left;
            left = left->prev;
            list_del(temp);
            q_release_element(list_entry(temp, element_t, list));
            if (left == head) {
                break;
            }
            cnt--;
        }
        left = right;
        right = right->next;
        cnt++;
    }
    return cnt;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (list_empty(head)) {
        return 0;
    }
    int queue_count = 0;
    queue_contex_t *first = list_first_entry(head, queue_contex_t, chain);
    queue_contex_t *second =
        list_entry(first->chain.next, queue_contex_t, chain);
    while (second->size != 0) {
        list_splice_init(second->q, first->q);
        second->size = 0;
        list_move_tail(&second->chain, head);
        second = list_entry(first->chain.next, queue_contex_t, chain);
    }
    q_sort(first->q, descend);
    queue_count = q_size(first->q);
    first->size = queue_count;
    return queue_count;
}