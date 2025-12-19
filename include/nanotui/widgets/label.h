#ifndef NANOTUI_LABEL_H
#define NANOTUI_LABEL_H

#include <nanotui/widget.h>

Widget* label_create(const char* text, int x, int y);
void label_destroy(Widget* w);

#endif
