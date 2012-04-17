#include "stdlib.h"
#include "assert.h"
#include "bool.h"
#include "malloc.h"
#include "list.h"
#include "nat.h"

typedef int ElementType;

/*@

fixpoint bool forall_nth_core<t>(list<t> xs, fixpoint(list<t>, int, bool) p, nat n) {
  switch(n) {
    case zero: return p(xs, int_of_nat(zero));
    case succ(n0): return p(xs, int_of_nat(n)) && forall_nth_core(xs, p, n0);
  }
}

fixpoint bool forall_nth<t>(list<t> xs, fixpoint(list<t>, int, bool) p) {
  switch(xs) {
    case nil: return true;
    case cons(h, t): return forall_nth_core(xs, p, nat_of_int(length(xs) - 1));
  }
}

fixpoint bool heap_index(list<int> xs, int i) {
  return 
    i < 1 || 
    i >= length(xs) || 
    (
      (2*i >= length(xs) || nth(i,xs) >= nth(2*i,xs)) && 
      (2*i + 1 >= length(xs) || nth(i,xs) >= nth(2*i + 1,xs))
    );
}

fixpoint bool heap_index_e(int except, list<int> xs, int i) {
  return 
    i < 1 || 
    i >= length(xs) || 
    (
      (2*i >= length(xs) || 2*i == except || nth(i,xs) >= nth(2*i,xs)) && 
      (2*i + 1 >= length(xs) || 2*i + 1 == except || nth(i,xs) >= nth(2*i + 1,xs))
    );
}

fixpoint bool ge(int v, list<int> vs, int index) { return v >= nth(index, vs); }

predicate heap(struct heap *heap, list<int> values) =
  malloc_block_heap(heap)
  &*& heap->capacity |-> ?capacity
  &*& 0 <= capacity
  &*& heap->size |-> ?size
  &*& length(values) == size
  &*& size + 1 <= capacity
  &*& heap->elems |-> ?elems
  &*& array<int>(elems, size + 1, sizeof(int), integer, ?vs)
  &*& tail(vs) == values
  &*& array<int>(elems + (size + 1), capacity - (size + 1), sizeof(int), integer, ?rest)
  &*& malloc_block(elems,4 * capacity)
  &*& forall_nth(vs, heap_index) == true;
  //&*& switch(values) { case nil: return true; case cons(h, t): return forall_nth(values, (ge)(h)) == true; }; // todo
@*/		

struct heap
{
    int capacity;
    int size;
    ElementType *elems; 
};

struct heap* heap_create(int capacity)
  //@ requires 0 <= capacity;
  //@ ensures heap(result, nil) &*& result != 0;
{
  struct heap* q;
  q = malloc(sizeof ( struct heap));
  if (q == 0) abort();
  if (sizeof(ElementType) == 0) abort();
  int acapacity = capacity + 1;
  int *array = malloc(4 * acapacity);
  if (array == 0) abort();
  //@ chars_to_intarray(array,acapacity);
  q->elems = array;
  if (q->elems == 0) abort();
  q->capacity = capacity + 1;
  q->size = 0;
  //@ open array<int>(array, capacity + 1, sizeof(int), integer, _);
  //@ close array<int>(array, 1, sizeof(int), integer, _);
  //@ close heap(q,nil);
  return q;
}

bool heap_is_empty(struct heap* heap)
	//@ requires heap(heap,?values);
	//@ ensures heap(heap,values) &*& result == (length(values) == 0);
{
	//@ open heap(heap,values);
        return heap->size == 0;
        //@ close heap(heap,values);
}

/*@
lemma void move_array_elem(int* arr, int N)
  requires array<int>(arr, N, sizeof(int), integer, ?vs1) &*& integer(arr + N, ?v);
  ensures array<int>(arr, N + 1, sizeof(int), integer, append(vs1, cons(v, nil)));
{
  switch(vs1) {
    case nil:
    case cons(h, t):
      open array<int>(arr, N, 4, integer, vs1);
      move_array_elem(arr + 1, N - 1);
  }
}

lemma int not_forall_nth_nat<t>(list<t> vs, fixpoint (list<t>, int, bool) p, nat n)
  requires ! forall_nth_core(vs, p, n);
  ensures 0 <= result &*& result <= int_of_nat(n) &*& ! p(vs,result);
{
  switch(n) {
    case zero: return 0;
    case succ(n0):
      if( ! p(vs, int_of_nat(n))) {
        return int_of_nat(n);
      } else {
        int i = not_forall_nth_nat(vs, p, n0);
        return i;
      }
  }
}

lemma int not_forall_nth<t>(list<t> vs, fixpoint (list<t>, int, bool) p)
  requires ! forall_nth(vs, p);
  ensures 0 <= result &*& result < length(vs) &*& ! p(vs, result);
{
  switch(vs) {
    case nil: return 0;
    case cons(h, t):
      int i = not_forall_nth_nat(vs, p, nat_of_int(length(vs) - 1));
      assert i <= int_of_nat(nat_of_int(length(vs) - 1));
      int_of_nat_of_int(length(vs) - 1);
      assert i <= length(vs) - 1;
      return i;
  }
  
}
  
lemma void forall_nth_elim_nat<t>(list<t> vs, fixpoint (list<t>, int, bool) p, nat n, int i)
  requires forall_nth_core(vs, p, n) == true &*& 0 <= i && i <= int_of_nat(n);
  ensures p(vs, i) == true;
{
  switch(n) {
    case zero:
    case succ(n0):
      if(i == int_of_nat(n)) {
      } else {
          forall_nth_elim_nat(vs, p, n0, i);
      } 
  }
}


lemma void forall_nth_elim<t>(list<t> vs, fixpoint (list<t>, int, bool) p, int i)
  requires forall_nth(vs, p) == true &*& 0 <= i && i < length(vs);
  ensures p(vs, i) == true;
{
  switch(vs) {
    case nil:
    case cons(h, t): forall_nth_elim_nat(vs, p, nat_of_int(length(vs) - 1), i);
  }
}

@*/


void heap_insert(struct heap* heap, ElementType x)
  //@ requires heap(heap, ?values);
  //@ ensures heap(heap, ?values2) &*& length(values2) == length(values) + 1;
{
  //@ open heap(heap, values);
  //@ int* arr = heap->elems;
  if(heap->size + 1 == heap->capacity) {
    abort();
  }
  //@ assert array<int>(arr, length(values) + 1, sizeof(int), integer, ?vs);
  //@ open array<int>(heap->elems + (heap->size + 1), heap->capacity - (heap->size + 1), sizeof(int), integer, ?rest);
  //@ move_array_elem(heap->elems, heap->size + 1);
  /*@if(! forall_nth(append(vs, cons(head(rest), nil)), (heap_index_e)(length(vs)))) {
    int i = not_forall_nth(append(vs, cons(head(rest), nil)), (heap_index_e)(length(vs)));
    nth_append(vs, cons(head(rest), nil), i);
    forall_nth_elim(vs, heap_index, i);
    nth_append(vs, cons(head(rest), nil), 2*i);
    nth_append(vs, cons(head(rest), nil), 2*i + 1);
  } @*/
  int in = ++heap->size;
  //@ assert array<int>(arr, length(values) + 2, sizeof(int), integer, ?es);
  heap->elems[in] = x;
  //@ assert 0 <= in && in < length(es);
  /*@
  if(! forall_nth(update(length(es) - 1, x, es), (heap_index_e)(length(es) - 1))) {
    int i = not_forall_nth(update(length(es) - 1, x, es), (heap_index_e)(length(es) - 1));
    forall_nth_elim(es, (heap_index_e)(length(es) - 1), i);
  }
  @*/
  swim(heap->elems, heap->size + 1, in);
  //@ assert array<int>(arr, in + 1, sizeof(int), integer, ?values2);
  //@ switch(values2) { case nil: case cons(h, t): switch(t) { case nil: case cons(h0, t0): } }
  //@ close heap(heap, tail(values2));
}

/*@
lemma_auto(i/2) void div_mul(int i);
  requires true;
  ensures 2*(i/2) == i || 2*(i/2) + 1 == i || i < 1;
@*/
void swim(int* arr, int N, int k)
  /*@ requires array<int>(arr, N, sizeof(int), integer, ?vs) &*& 0 < k &*& k < N &*& 
               forall_nth(vs, (heap_index_e)(k)) == true &*&
               (k == 1 || 2*k >= length(vs) || nth(k/2, vs) >= nth(2*k, vs)) &*&
               (k == 1 || 2*k + 1 >= length(vs) || nth(k/2, vs) >= nth(2*k + 1, vs)); @*/
  /*@ ensures array<int>(arr, N, sizeof(int), integer, ?vs2) &*& 
              forall_nth(vs2, heap_index) == true; @*/
{
  if(k == 1) {
    /*@
    if(! forall_nth(vs, heap_index)) {
      int i = not_forall_nth(vs, heap_index);
      forall_nth_elim(vs, (heap_index_e)(k), i);
    }
    @*/
    return;
  }
  if(arr[k/2] >= arr[k]) {
    /*@
    if(! forall_nth(vs, heap_index)) {
      int i = not_forall_nth(vs, heap_index);
      forall_nth_elim(vs, (heap_index_e)(k), i);
    }
    @*/
    return;
  }
  int tmp = arr[k];
  arr[k] = arr[k/2];
  arr[k/2] = tmp;
  //@ list<int> nvs = update(k/2, nth(k, vs), update(k, nth(k/2, vs), vs));
  //@ int nk = k / 2;
  /*@
  if(! forall_nth(nvs, (heap_index_e)(k/2))) {
    int i = not_forall_nth(nvs, (heap_index_e)(k/2));
    forall_nth_elim(vs, (heap_index_e)(k), i);
  }
  @*/
  //@ forall_nth_elim(vs, (heap_index_e)(k), nk/2);
  //@ forall_nth_elim(vs, (heap_index_e)(k), k/2);
  swim(arr, N, k /2);
}

ElementType heap_max(struct heap* heap)
  //@ requires heap(heap, ?values) &*& 0 < length(values);
  //@ ensures heap(heap, values);// &*& forall_nth(values, (ge)(result)) == true; // todo
{
  //@ open heap(heap, values);
  return heap->elems[1];
  //@ close heap(heap, values);
}


int main()
  //@ requires true;
  //@ ensures true;
{
    ElementType max;
    struct heap* q = heap_create(6);
    bool empty = heap_is_empty(q);
    assert(empty);
    heap_insert(q,2);
    empty = heap_is_empty(q);
    assert(!empty);
    heap_insert(q,5);
    heap_insert(q,3);
    heap_insert(q,6);
    heap_insert(q,9);
    heap_insert(q,10);
    return 0;
    //@ leak heap(q,_); //TO BE REMOVED
}



