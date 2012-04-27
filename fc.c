/*
 * A try to make a type-safe list in C using macros.
 *
 * Andreas Textor <textor.andreas@googlemail.com>
 */

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdarg.h>

#define list_t(TYPE) struct Li_##TYPE*
#define new_list(TYPE) new_list_##TYPE

#define USE_LIST(TYPE) \
struct Li_##TYPE;\
typedef struct Li_##TYPE* List_##TYPE;\
List_##TYPE list_##TYPE##_cons(TYPE v, List_##TYPE tl);\
void list_##TYPE##_foreach(List_##TYPE l, void (f)(TYPE));\
bool list_##TYPE##_forall(List_##TYPE l, bool (f)(TYPE));\
List_##TYPE list_##TYPE##_map(List_##TYPE l, TYPE (f)(TYPE));\
TYPE list_##TYPE##_foldLeft(List_##TYPE l, TYPE seed, TYPE(f)(TYPE, TYPE));\
\
struct Li_##TYPE {\
	struct Li_##TYPE *tl;\
	TYPE hd;\
	bool isEmpty;\
	TYPE (*foldLeft)(List_##TYPE l, TYPE seed, TYPE(f)(TYPE, TYPE));\
	void (*foreach)(List_##TYPE l, void(f)(TYPE));\
	bool (*forall)(List_##TYPE l, bool (f)(TYPE));\
	List_##TYPE (*map)(List_##TYPE l, TYPE (f)(TYPE));\
};\
TYPE identity(TYPE v) { return v; }\
List_##TYPE list_##TYPE##_nil() {\
	List_##TYPE l = malloc(sizeof(struct Li_##TYPE));\
	l->tl = NULL;\
	l->isEmpty = true;\
	l->foldLeft = list_##TYPE##_foldLeft;\
	l->foreach = list_##TYPE##_foreach;\
	l->forall = list_##TYPE##_forall;\
	l->map = list_##TYPE##_map;\
	return l;\
}\
List_##TYPE list_##TYPE##_cons(TYPE v, List_##TYPE tl) {\
	List_##TYPE l = list_##TYPE##_nil();\
	l->hd = v;\
	l->tl = tl;\
	l->isEmpty = false;\
	return l;\
}\
void list_##TYPE##_foreach(List_##TYPE l, void (f)(TYPE)) {\
	while(!l->isEmpty) {\
		f(l->hd);\
		l = l->tl;\
	}\
}\
bool list_##TYPE##_forall(List_##TYPE l, bool (f)(TYPE)) {\
	bool result = true;\
	while(!l->isEmpty) {\
		result = result && f(l->hd);\
		l = l->tl;\
	}\
	return result;\
}\
List_##TYPE list_##TYPE##_map(List_##TYPE l, TYPE (f)(TYPE)) {\
	List_##TYPE result = list_##TYPE##_nil();\
	while(!l->isEmpty) {\
		result = list_##TYPE##_cons(f(l->hd), result);\
		l = l->tl;\
	}\
	return result;\
}\
TYPE list_##TYPE##_foldLeft(List_##TYPE l, TYPE seed, TYPE(f)(TYPE, TYPE)) {\
	return l->isEmpty ? seed : list_##TYPE##_foldLeft(l->tl, f(seed, l->hd), f);\
}\
List_##TYPE new_list_##TYPE(TYPE n, ...) {\
	va_list ptr;\
	List_##TYPE result = list_##TYPE##_nil();\
	result->isEmpty = false;\
	result->hd = n;\
	va_start(ptr, n);\
	TYPE x;\
	List_##TYPE y;\
	List_##TYPE first = result;\
	do {\
		x = va_arg(ptr, TYPE);\
		y = list_##TYPE##_nil();\
		y->isEmpty = false;\
		y->hd = x;\
		result->tl = y;\
		result = y;\
	} while (x != 0);\
	va_end(ptr);\
	y = list_##TYPE##_nil();\
	result->tl = y;\
	return first;\
}

USE_LIST(int)

void printint(int i) { printf("%d ", i); }
int timestwo(int i) { return i * 2; }
bool isEven(int i) { return i % 2 == 0; }
bool lessThan20(int i) { return i < 20; }
int add(int a, int b) { return a + b; }

int main(void) {
	list_t(int) l = new_list(int)(1, 2, 3, 4, 5, 0);
	l->foreach(l, printint);
	putchar('\n');
	printf("Sum %d\n", l->foldLeft(l, 0, add));
	printf("All even? %d\n", l->forall(l, isEven));
	printf("All <20? %d\n", l->forall(l, lessThan20));

	list_t(int) l2 = l->map(l, timestwo);
	list_t(int) l3 = l2->map(l2, identity);
	l3->foreach(l3, printint);
	putchar('\n');

	return 0;
}

