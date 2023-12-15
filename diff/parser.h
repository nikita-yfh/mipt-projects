#ifndef PARSER_H
#define PARSER_H

#include "btree.h"
#include "syntaxError.h"

struct BinaryTreeNode *parseString(const char *str, struct SyntaxError *error); 

#endif // PARSER_H

