#pragma once

#include "alloc.h"
#include "base.h"

typedef struct list_node {
  struct list_node *next;
  struct list_node *prev;
  void *data;
} list_node;

typedef struct {
  alloc alloc;
  list_node *head;
  list_node *tail;
} list;

/**
 * @brief Applies the given function to each element in the list.
 *
 * @param l The list to apply the function to.
 * @param f The function to apply to each element in the list.
 */
inline void list_apply(list l ref, void (*f)(void *)) {
  for (list_node *n = l->head; n != nil; n = n->next) {
    f(n->data);
  }
}

/**
 * @brief Adds a new element to the end of the list.
 *
 * @param l The list to add the element to.
 * @param data A pointer to the data to be added to the list.
 */
inline void list_push(list l ref, void *data) {
  list_node *n = alloc_allocz(l->alloc, sizeof(list_node));
  n->data = data;
  n->next = nil;
  n->prev = l->tail;
  if (l->tail != nil) {
    l->tail->next = n;
  }
  l->tail = n;
  if (l->head == nil) {
    l->head = n;
  }
}

/**
 * @brief Removes and returns the last element of the list.
 *
 * @param l The list to remove the element from.
 * @return A pointer to the data of the removed element, or NULL if the list is
 * empty.
 *
 * This function removes and returns the last element of the list. If the list
 * is empty, it returns NULL. The memory of the removed element is freed using
 * the allocator of the list.
 */
inline void *list_pop(list l ref) {
  if (l->tail == nil) {
    return nil;
  }
  list_node *n = l->tail;
  l->tail = n->prev;
  if (l->tail != nil) {
    l->tail->next = nil;
  }
  if (l->head == n) {
    l->head = nil;
  }
  void *data = n->data;
  alloc_free(l->alloc, n);
  return data;
}

/**
 * @brief Inserts a new node with the given data at the beginning of the list.
 *
 * @param l The list to insert the node into.
 * @param data The data to store in the new node.
 */
inline void list_unshift(list l ref, void *data) {
  list_node *n = alloc_allocz(l->alloc, sizeof(list_node));
  n->data = data;
  n->next = l->head;
  n->prev = nil;
  if (l->head != nil) {
    l->head->prev = n;
  }
  l->head = n;
  if (l->tail == nil) {
    l->tail = n;
  }
}

/**
 * @brief Removes and returns the first element of the list.
 *
 * If the list is empty, returns `nil`.
 *
 * @param l The list to remove the element from.
 * @return The data of the removed element, or `nil` if the list is empty.
 */
inline void *list_shift(list l ref) {
  if (l->head == nil) {
    return nil;
  }
  list_node *n = l->head;
  l->head = n->next;
  if (l->head != nil) {
    l->head->prev = nil;
  }
  if (l->tail == n) {
    l->tail = nil;
  }
  void *data = n->data;
  alloc_free(l->alloc, n);
  return data;
}

/**
 * @brief Inserts a new node with the given data at the specified index in the
 * list.
 *
 * @param l The list to insert the node into.
 * @param data The data to store in the new node.
 * @param index The index at which to insert the new node.
 */
inline void list_insert(list l ref, void *data, usize index) {
  list_node *n = alloc_allocz(l->alloc, sizeof(list_node));
  n->data = data;
  if (index == 0) {
    n->next = l->head;
    n->prev = nil;
    if (l->head != nil) {
      l->head->prev = n;
    }
    l->head = n;
    if (l->tail == nil) {
      l->tail = n;
    }
    return;
  }
  list_node *p = l->head;
  while (index > 1 && p != nil) {
    p = p->next;
    index--;
  }
  if (p == nil) {
    return;
  }
  n->next = p->next;
  n->prev = p;
  if (p->next != nil) {
    p->next->prev = n;
  }
  p->next = n;
  if (l->tail == p) {
    l->tail = n;
  }
}

/**
 * @brief Removes the element at the specified index from the list.
 *
 * If the index is out of bounds, or if the list is empty, this function returns
 * `nil`.
 *
 * @param l The list to remove an element from.
 * @param index The index of the element to remove.
 *
 * @return A pointer to the data of the removed element, or `nil` if the index
 * is out of bounds or the list is empty.
 */
inline void *list_remove(list l ref, usize index) {
  if (index == 0) {
    return list_shift(l);
  }
  list_node *p = l->head;
  while (index > 1 && p != nil) {
    p = p->next;
    index--;
  }
  if (p == nil) {
    return nil;
  }
  list_node *n = p->next;
  if (n == nil) {
    return nil;
  }
  p->next = n->next;
  if (n->next != nil) {
    n->next->prev = p;
  }
  if (l->tail == n) {
    l->tail = p;
  }
  void *data = n->data;
  alloc_free(l->alloc, n);
  return data;
}

/**
 * @brief Get the data at the specified index in the list.
 *
 * @param l The list to get the data from.
 * @param index The index of the data to get.
 * @return A pointer to the data at the specified index, or `nil` if the index
 * is out of bounds.
 */
inline void *list_get(list l ref, usize index) {
  list_node *n = l->head;
  while (index > 0 && n != nil) {
    n = n->next;
    index--;
  }
  if (n == nil) {
    return nil;
  }
  return n->data;
}

/**
 * @brief Sets the data at the specified index in the given list.
 *
 * @param l The list to modify.
 * @param index The index of the element to modify.
 * @param data The new data to set at the specified index.
 * @return A pointer to the old data at the specified index, or `nil` if the
 * index is out of bounds.
 */
inline void *list_set(list l ref, usize index, void *data) {
  list_node *n = l->head;
  while (index > 0 && n != nil) {
    n = n->next;
    index--;
  }
  if (n == nil) {
    return nil;
  }
  void *old = n->data;
  n->data = data;
  return old;
}

/**
 * @brief Returns the length of the given list.
 *
 * This function iterates over the list and counts the number of nodes to determine the length of the list.
 *
 * @param l A reference to the list to get the length of.
 * @return The length of the list.
 */
inline usize list_len(list l ref) {
  usize len = 0;
  for (list_node *n = l->head; n != nil; n = n->next) {
    len++;
  }
  return len;
}

/**
 * @brief Clears all elements from the given list.
 *
 * This function removes all elements from the given list by repeatedly calling
 * `list_shift` until the list is empty.
 *
 * @param l A reference to the list to clear.
 */
inline void list_clear(list l ref) {
  while (l->head != nil) {
    list_shift(l);
  }
}

/**
 * @brief Enqueues the given data to the end of the list.
 *
 * This function is a convenience wrapper around `list_push`, which adds the given data to the end of the list.
 *
 * @param l A reference to the list to enqueue the data to.
 * @param data A pointer to the data to enqueue.
 */
inline void list_enqueue(list l ref, void *data) { list_push(l, data); }

/**
 * @brief Removes and returns the first element of the list.
 *
 * @param l The list to dequeue from.
 * @return void* A pointer to the first element of the list.
 */
inline void *list_dequeue(list l ref) { return list_shift(l); }

/**
 * @brief Requeues the first element of the list to the end of the list.
 *
 * @param l The list to requeue the element in.
 * @return void* A pointer to the data of the requeued element, or NULL if the list is empty.
 */
inline void *list_requeue(list l ref) {
  if (l->head == nil) {
    return nil;
  }
  list_node *n = l->head;
  l->head = n->next;
  if (l->head != nil) {
    l->head->prev = nil;
  }
  if (l->tail == n) {
    l->tail = nil;
  }
  n->next = nil;
  n->prev = l->tail;
  if (l->tail != nil) {
    l->tail->next = n;
  }
  l->tail = n;
  if (l->head == nil) {
    l->head = n;
  }
  return n->data;
}