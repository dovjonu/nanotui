#include "node_internal.h"
#include <nanotui/node.h>
#include <stdlib.h>

void node_add_child(Node* parent, Node* child) {
    if (!parent || !child) return;

    if (parent->child_count == parent->child_capacity) {
        parent->child_capacity = parent->child_capacity ? parent->child_capacity * 2 : 4;
        parent->children = realloc(
            parent->children,
            parent->child_capacity * sizeof(Node*)
        );
    }

    parent->children[parent->child_count++] = child;
}

void node_render(Node* node, RenderBuffer* rb) {
    if (!node) return;

    /* layout first */
    if (node->layout)
        node->layout(node);

    node_render_border(node, rb);

    /* render self */
    if (node->render)
        node->render(node, rb);

    /* render children */
    for (int i = 0; i < node->child_count; i++) {
        node_render(node->children[i], rb);
    }
}

void node_destroy(Node* node) {
    if (!node) return;

    /* destroy children */
    for (int i = 0; i < node->child_count; i++) {
        node_destroy(node->children[i]);
    }
    free(node->children);

    /* destroy impl if needed */
    if (node->impl) {
        free(node->impl);
    }

    free(node);
}

void node_render_border(Node* n, RenderBuffer* rb) {
    if (n->border.mask == BORDER_NONE)
        return;

    int x0 = n->x;
    int y0 = n->y;
    int x1 = n->x + n->width  - 1;
    int y1 = n->y + n->height - 1;

    BorderStyle* s = &n->border.style;

    if (n->border.mask & BORDER_TOP) {
        for (int x = x0 + 1; x < x1; x++)
            render_buffer_put(rb, x, y0, s->top);
    }

    if (n->border.mask & BORDER_BOTTOM) {
        for (int x = x0 + 1; x < x1; x++)
            render_buffer_put(rb, x, y1, s->bottom);
    }

    if (n->border.mask & BORDER_LEFT) {
        for (int y = y0 + 1; y < y1; y++)
            render_buffer_put(rb, x0, y, s->left);
    }

    if (n->border.mask & BORDER_RIGHT) {
        for (int y = y0 + 1; y < y1; y++)
            render_buffer_put(rb, x1, y, s->right);
    }

    /* corners */
    if ((n->border.mask & BORDER_TOP) && (n->border.mask & BORDER_LEFT))
        render_buffer_put(rb, x0, y0, s->top_left);

    if ((n->border.mask & BORDER_TOP) && (n->border.mask & BORDER_RIGHT))
        render_buffer_put(rb, x1, y0, s->top_right);

    if ((n->border.mask & BORDER_BOTTOM) && (n->border.mask & BORDER_LEFT))
        render_buffer_put(rb, x0, y1, s->bottom_left);

    if ((n->border.mask & BORDER_BOTTOM) && (n->border.mask & BORDER_RIGHT))
        render_buffer_put(rb, x1, y1, s->bottom_right);
}