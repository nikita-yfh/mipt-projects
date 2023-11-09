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

#ifndef LIST_INIT_CAPACITY
#define LIST_INIT_CAPACITY 10
#endif

#ifndef LIST_RESIZE_KOEF
#define LIST_RESIZE_KOEF 2
#endif

typedef int listIndex_t;

#endif

