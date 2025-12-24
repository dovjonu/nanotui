#ifndef NANOTUI_NODE_H
#define NANOTUI_NODE_H

#include <nanotui/render.h>

typedef struct Node Node;

/* Tree manipulation */
void node_add_child(Node* parent, Node* child);

/* Rendering entry point */
void node_render(Node* node, RenderBuffer* rb);

void node_destroy(Node* node);

#endif
