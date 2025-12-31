#ifndef NANOTUI_NODE_INTERNAL_H
#define NANOTUI_NODE_INTERNAL_H

#include <nanotui/node.h>
#include <nanotui/ui.h>


struct Node {
    /* geometry assigned by parent layout */
    int x, y;
    int width, height;

    SizeHint width_hint;
    SizeHint height_hint;

    int flex;

    /* layout + render behavior */
    void (*measure)(Node* self);
    void (*layout)(Node* self);
    void (*render)(Node* self, RenderBuffer* rb);

    /* tree structure */
    UI* ui;
    Node* parent;
    Node** children;
    int child_count;
    int child_capacity;

    /* border handling */
    BorderConfig (*default_border)(Node* self);
    int has_explicit_border;
    BorderConfig border;

    int focusable;
    int focused;
    int (*on_key)(Node* self, int key);

    /* node-specific data (Label text, VBox spacing, etc.) */
    void* impl;
};

/* Internal initializer used by widgets/layouts after calloc(sizeof(Node)). */
void node_init(Node* node);

void node_render_border(Node* n, RenderBuffer* rb);



#endif
