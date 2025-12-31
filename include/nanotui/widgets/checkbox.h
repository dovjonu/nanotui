#ifndef NANOTUI_CHECKBOX_H
#define NANOTUI_CHECKBOX_H

#include <nanotui/node.h>

typedef void (*CheckboxCallback)(Node* checkbox, void* user_data);

Node* checkbox_create(const char* text, CheckboxCallback on_toggle, void* user_data);
void  checkbox_set_text(Node* checkbox, const char* text);
char* checkbox_get_text(Node* checkbox);
int  checkbox_is_checked(Node* checkbox);
void  checkbox_set_checked(Node* checkbox, int checked);

#endif
