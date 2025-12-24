#ifndef NANOTUI_LABEL_H
#define NANOTUI_LABEL_H

#include <nanotui/node.h>

Node* label_create(const char* text);
void label_destroy(Node* n);

#endif
