#include "widget_internal.h"
#include <nanotui/widget.h>


void widget_render(Widget* w, RenderBuffer* rb) {
    if (w && w->render)
        w->render(w, rb);
}

