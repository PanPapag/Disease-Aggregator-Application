#ifndef __WORKER_AVL__
  #define __WORKER_AVL__

  #include <stdint.h>

  typedef uint8_t byte_t;

  typedef struct avl_node_t* avl_node_ptr;
  typedef struct avl_t* avl_ptr;

  struct avl_node_t {
    avl_node_ptr left_;
    avl_node_ptr right_;
    avl_node_ptr parent_;
    int balance_factor_;
    void* data_;
  } avl_node_t;

  struct avl_t {
    avl_node_ptr root_;
    size_t size_;
    int64_t (*avl_cmp_func_)(void*, void*);
    void (*avl_print_func_)(void*, FILE*);
  } avl_t;

  /* Creates a new avl tree given compare, print and delete functions */
  avl_ptr avl_create(int64_t (*)(void*, void*), void (*)(void*, FILE*));

  /* Generic Purpose Delete Function of AVL Tree */
  void avl_clear(void*);

  /* Adds an element to the avl tree */
  void avl_insert(avl_ptr*, void*);

  /* Returns the number of elements inserted into the avl tree */
  size_t avl_size(avl_ptr);

  /* Find the element equals a given one */
  avl_node_ptr avl_find(avl_ptr, void*);

  /* Generic Purpose Print Function of AVL Tree */
  void avl_print_inorder(void*, FILE*);

#endif
