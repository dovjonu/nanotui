#ifndef NANOTUI_BUTTON_H
#define NANOTUI_BUTTON_H

#include <nanotui/node.h>

typedef void (*ButtonCallback)(Node* button, void* user_data);

Node* button_create(const char* text, ButtonCallback on_press, void* user_data);
void  button_set_text(Node* button, const char* text);
char* button_get_text(Node* button);

#endif
