#ifndef NANOTUI_LINE_EDIT_H
#define NANOTUI_LINE_EDIT_H

#include <nanotui/node.h>

Node* line_edit_create(int length);
void  line_edit_set_text(Node* line_edit, const char* text);
char* line_edit_get_text(Node* line_edit);

#endif
