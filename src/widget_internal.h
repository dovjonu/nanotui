#ifndef NANOTUI_WIDGET_INTERNAL_H
#define NANOTUI_WIDGET_INTERNAL_H

#include <nanotui/render.h>
#include <nanotui/widget.h>

struct Widget {
    int x, y;
    void (*render)(Widget*, RenderBuffer*);
    void* impl;  /* widget-specific data */
};

#endif
