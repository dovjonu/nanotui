#ifndef NANOTUI_NODE_INTERNAL_H
#define NANOTUI_NODE_INTERNAL_H

#include <nanotui/render.h>

typedef struct Node Node;

typedef enum {
    BORDER_NONE   = 0,
    BORDER_TOP    = 1 << 0,
    BORDER_RIGHT  = 1 << 1,
    BORDER_BOTTOM = 1 << 2,
    BORDER_LEFT   = 1 << 3,
} BorderMask;

typedef struct {
    char top;
    char bottom;
    char left;
    char right;
    char top_left;
    char top_right;
    char bottom_left;
    char bottom_right;
} BorderStyle;

typedef struct {
    BorderMask mask;
    BorderStyle style;
} BorderConfig;


struct Node {
    /* geometry assigned by parent layout */
    int x, y;
    int width, height;

    /* layout + render behavior */
    void (*layout)(Node* self);
    void (*render)(Node* self, RenderBuffer* rb);

    /* tree structure */
    Node** children;
    int child_count;
    int child_capacity;

    /* border handling */
    BorderConfig (*default_border)(Node* self);
    int has_explicit_border;
    BorderConfig border;

    /* node-specific data (Label text, VBox spacing, etc.) */
    void* impl;
};

void node_render_border(Node* n, RenderBuffer* rb);



#endif
