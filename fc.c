/*
 * A try to make a type-safe list in C using macros.
 *
 * Andreas Textor <textor.andreas@googlemail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#define LAMBDA(type, args, body) ({ type __lambda__ args body &__lambda__; })
#define foreach(list, type, body) _foreach(l, LAMBDA(void, (type _), body))
#define foldLeft(list, type, seed, body) _foldLeft(l, seed, LAMBDA(type, (type _1, type _2), body))
#define forall(list, type, body) _forall(l, LAMBDA(bool, (type _), body))
#define map(list, type, body) _map(l, LAMBDA(type, (type _), body))

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
	char* typeName;\
	TYPE (*_foldLeft)(List_##TYPE l, TYPE seed, TYPE(f)(TYPE, TYPE));\
	void (*_foreach)(List_##TYPE l, void(f)(TYPE));\
	bool (*_forall)(List_##TYPE l, bool (f)(TYPE));\
	List_##TYPE (*_map)(List_##TYPE l, TYPE (f)(TYPE));\
};\
TYPE identity(TYPE v) { return v; }\
List_##TYPE list_##TYPE##_nil() {\
	List_##TYPE l = malloc(sizeof(struct Li_##TYPE));\
	l->tl = NULL;\
	l->isEmpty = true;\
	l->_foldLeft = list_##TYPE##_foldLeft;\
	l->_foreach = list_##TYPE##_foreach;\
	l->_forall = list_##TYPE##_forall;\
	l->_map = list_##TYPE##_map;\
	l->typeName = malloc(strlen(#TYPE) + 1);\
	memset(l->typeName, 0, strlen(#TYPE) + 1);\
	snprintf(l->typeName, strlen(#TYPE) + 1, #TYPE);\
	return l;\
}\
List_##TYPE list_##TYPE##_cons(TYPE v, List_##TYPE tl) {\
	List_##TYPE l = list_##TYPE##_nil();\
	l->hd = v;\
	l->tl = tl;\
	l->isEmpty = false;\
	l->typeName = tl->typeName;\
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

int main(void) {
	list_t(int) l = new_list(int)(1, 2, 3, 4, 5, 0);
	l->foreach(l, int, { printf("%d ", _); });
	putchar('\n');
	printf("Sum %d\n", l->foldLeft(l, int, 0, { return _1 + _2; }));

	printf("All even? %d\n", l->forall(l, int, { return _ % 2 == 0; }));
	printf("All <20? %d\n", l->forall(l, int, { return _ < 20; }));

	list_t(int) l2 = l->map(l, int, { return _ * 2; });
	list_t(int) l3 = l->_map(l2, identity);
	l3->foreach(l3, int, { printf("%d ", _); });
	putchar('\n');

	printf("Base type of l: %s\n", l->typeName);

	return 0;
}

