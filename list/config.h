#ifndef LISTCONF_H
#define LISTHONF_H

#ifdef LIST_VALUE_TYPE
typedef LIST_VALUE_TYPE listValue_t;
#else
typedef int listValue_t;
#endif

#ifndef LIST_FORMAT
#define LIST_FORMAT "%d"
#endif

#ifndef LIST_DEFAULT_VALUE
#define LIST_DEFAULT_VALUE -1
#endif

#ifndef LIST_INIT_CAPACITY
#define LIST_INIT_CAPACITY 10
#endif

#ifndef LIST_RESIZE_KOEF
#define LIST_RESIZE_KOEF 2
#endif

typedef size_t listIndex_t;
#define LIST_INVALID_INDEX SIZE_MAX
#define LIST_DUMMY_INDEX 0

#endif

