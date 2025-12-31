#include "node_internal.h"
#include <nanotui/node.h>
#include <stdlib.h>
#include <string.h>

static SizeHint size_hint_normalize(SizeHint hint) {
    if (hint.min < 0)
        hint.min = 0;
    if (hint.pref < hint.min)
        hint.pref = hint.min;
    if (hint.max != -1) {
        if (hint.max < hint.min)
            hint.max = hint.min;
        if (hint.pref > hint.max)
            hint.pref = hint.max;
    }
    return hint;
}

void node_init(Node* node) {
    if (!node)
        return;

    node->width_hint = (SizeHint){ .min = 1, .pref = 1, .max = -1 };
    node->height_hint = (SizeHint){ .min = 1, .pref = 1, .max = -1 };
    node->flex = 0;
}

BorderStyle node_border_style_ascii(void) {
    return (BorderStyle){
        .top = '-',
        .bottom = '-',
        .left = '|',
        .right = '|',
        .top_left = '+',
        .top_right = '+',
        .bottom_left = '+',
        .bottom_right = '+',
    };
}

BorderStyle node_border_style_ascii_thick(void) {
    return (BorderStyle){
        .top = '=',
        .bottom = '=',
        .left = '#',
        .right = '#',
        .top_left = '+',
        .top_right = '+',
        .bottom_left = '+',
        .bottom_right = '+',
    };
}

static BorderConfig node_border_default(const Node* node) {
    if (node && node->default_border)
        return node->default_border((Node*)node);

    return (BorderConfig){
        .mask = BORDER_NONE,
        .style = node_border_style_ascii(),
        .title = (BorderTitle){ .text = NULL, .position = BORDER_TITLE_LEFT }
    };
}

BorderConfig node_border_get(const Node* node) {
    if (!node) {
        return (BorderConfig){
            .mask = BORDER_NONE,
            .style = node_border_style_ascii(),
            .title = (BorderTitle){ .text = NULL, .position = BORDER_TITLE_LEFT }
        };
    }

    if (node->has_explicit_border)
        return node->border;

    return node_border_default(node);
}

static void node_border_ensure_override(Node* node) {
    if (!node)
        return;

    if (node->has_explicit_border)
        return;

    node->border = node_border_default(node);
    node->has_explicit_border = 1;
}

void node_border_clear_override(Node* node) {
    if (!node)
        return;
    node->has_explicit_border = 0;
}

void node_border_set_config(Node* node, BorderConfig config) {
    if (!node)
        return;
    node->border = config;
    node->has_explicit_border = 1;
}

void node_border_set_mask(Node* node, BorderMask mask) {
    node_border_ensure_override(node);
    if (!node)
        return;
    node->border.mask = mask;
}

void node_border_set_style(Node* node, BorderStyle style) {
    node_border_ensure_override(node);
    if (!node)
        return;
    node->border.style = style;
}

void node_border_set_title(Node* node, BorderTitle title) {
    node_border_ensure_override(node);
    if (!node)
        return;
    node->border.title = title;
}

void node_border_patch_style(Node* node, BorderStyleMask fields, BorderStyle patch) {
    node_border_ensure_override(node);
    if (!node)
        return;

    if (fields & BORDER_STYLE_TOP)
        node->border.style.top = patch.top;
    if (fields & BORDER_STYLE_RIGHT)
        node->border.style.right = patch.right;
    if (fields & BORDER_STYLE_BOTTOM)
        node->border.style.bottom = patch.bottom;
    if (fields & BORDER_STYLE_LEFT)
        node->border.style.left = patch.left;
    if (fields & BORDER_STYLE_TOP_LEFT)
        node->border.style.top_left = patch.top_left;
    if (fields & BORDER_STYLE_TOP_RIGHT)
        node->border.style.top_right = patch.top_right;
    if (fields & BORDER_STYLE_BOTTOM_LEFT)
        node->border.style.bottom_left = patch.bottom_left;
    if (fields & BORDER_STYLE_BOTTOM_RIGHT)
        node->border.style.bottom_right = patch.bottom_right;
}

void node_set_rect(Node* node, int x, int y, int width, int height) {
    if (!node)
        return;
    node->x = x;
    node->y = y;
    node->width = width;
    node->height = height;
}

void node_set_width_hint(Node* node, SizeHint hint) {
    if (!node)
        return;
    node->width_hint = size_hint_normalize(hint);
}

void node_set_height_hint(Node* node, SizeHint hint) {
    if (!node)
        return;
    node->height_hint = size_hint_normalize(hint);
}

SizeHint node_get_width_hint(const Node* node) {
    if (!node)
        return (SizeHint){ .min = 0, .pref = 0, .max = -1 };
    return node->width_hint;
}

SizeHint node_get_height_hint(const Node* node) {
    if (!node)
        return (SizeHint){ .min = 0, .pref = 0, .max = -1 };
    return node->height_hint;
}

void node_set_flex(Node* node, int flex) {
    if (!node)
        return;
    node->flex = flex < 0 ? 0 : flex;
}

int node_get_flex(const Node* node) {
    if (!node)
        return 0;
    return node->flex;
}

void node_measure(Node* node) {
    if (!node)
        return;

    for (int i = 0; i < node->child_count; i++) {
        node_measure(node->children[i]);
    }

    if (node->measure)
        node->measure(node);

    node->width_hint = size_hint_normalize(node->width_hint);
    node->height_hint = size_hint_normalize(node->height_hint);
}

static void node_render_impl(Node* node, RenderBuffer* rb) {
    if (!node)
        return;

    /* layout first */
    if (node->layout)
        node->layout(node);

    node_render_border(node, rb);

    /* render self */
    if (node->render)
        node->render(node, rb);

    /* render children */
    for (int i = 0; i < node->child_count; i++) {
        node_render_impl(node->children[i], rb);
    }
}

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

    child->parent = parent;
}

void node_render(Node* node, RenderBuffer* rb) {
    if (!node)
        return;

    node_measure(node);
    node_render_impl(node, rb);
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
    BorderConfig bc = node_border_get(n);
    if (bc.mask == BORDER_NONE)
        return;

    int x0 = n->x;
    int y0 = n->y;
    int x1 = n->x + n->width  - 1;
    int y1 = n->y + n->height - 1;

    BorderStyle* s = &bc.style;

    if (bc.mask & BORDER_TOP) {
        for (int x = x0 + 1; x < x1; x++)
            render_buffer_put(rb, x, y0, s->top);
    }

    if (bc.mask & BORDER_BOTTOM) {
        for (int x = x0 + 1; x < x1; x++)
            render_buffer_put(rb, x, y1, s->bottom);
    }

    if (bc.mask & BORDER_LEFT) {
        for (int y = y0 + 1; y < y1; y++)
            render_buffer_put(rb, x0, y, s->left);
    }

    if (bc.mask & BORDER_RIGHT) {
        for (int y = y0 + 1; y < y1; y++)
            render_buffer_put(rb, x1, y, s->right);
    }

    /* corners */
    if ((bc.mask & BORDER_TOP) && (bc.mask & BORDER_LEFT))
        render_buffer_put(rb, x0, y0, s->top_left);

    if ((bc.mask & BORDER_TOP) && (bc.mask & BORDER_RIGHT))
        render_buffer_put(rb, x1, y0, s->top_right);

    if ((bc.mask & BORDER_BOTTOM) && (bc.mask & BORDER_LEFT))
        render_buffer_put(rb, x0, y1, s->bottom_left);

    if ((bc.mask & BORDER_BOTTOM) && (bc.mask & BORDER_RIGHT))
        render_buffer_put(rb, x1, y1, s->bottom_right);


    /* title */
    if (bc.title.text) {
        int title_len = (int)strlen(bc.title.text);
        int title_x = x0 + 3;

        if (bc.title.position == BORDER_TITLE_CENTER) {
            title_x = x0 + (n->width - title_len) / 2;
        } else if (bc.title.position == BORDER_TITLE_RIGHT) {
            title_x = x1 - title_len - 2;
        }

        for (int i = 0; i < title_len; i++) {
            int px = title_x + i;
            if (px > x0 && px < x1) {
                render_buffer_put(rb, px, y0, bc.title.text[i]);
            }
        }
        render_buffer_put(rb, title_x - 1, y0, ' ');
        render_buffer_put(rb, title_x + title_len, y0, ' ');
    }
}

void node_set_focusable(Node* node, int focusable) {
    if (!node) return;
    node->focusable = focusable ? 1 : 0;
}

int node_is_focusable(const Node* node) {
    return node ? node->focusable : 0;
}

void node_set_focus(Node* node, int focused) {
    if (!node) return;
    node->focused = focused ? 1 : 0;
}

int node_has_focus(const Node* node) {
    return node ? node->focused : 0;
}

int node_handle_key(Node* node, int key) {
    if (!node) return 0;
    if (node->on_key) return node->on_key(node, key);
    return 0;
}

/* preorder traversal: find next focusable after 'current' */
static Node* next_node(Node* current) {
    if (!current) return NULL;
    if (current->child_count > 0)
        return current->children[0];
    
    // search next sibling
    Node* parent = current->parent;
    if (!parent) return NULL;
    for (int i = 0; i < parent->child_count; i++) {
        if (parent->children[i] == current) {
            if (i + 1 < parent->child_count)
                return parent->children[i + 1];
            else
                return next_node(parent);
        }
    }
    return NULL;
}

Node* node_focus_next(Node* root, Node* current) {
    Node* n = next_node(current);
    while (n) {
        if (node_is_focusable(n))
            return n;
        else
            n = next_node(n);
    }
}

static Node* last_node(Node* n) {
    if (!n) return NULL;

    while (n->child_count > 0) {
        n = n->children[n->child_count - 1];
    }
    return n;
}


static Node* prev_node(Node* root, Node* current, int check_children) {
    if (!current)
        return last_node(root);

    /* wrap-around */
    if (current == root)
        return last_node(root);

    Node* parent = current->parent;
    if (!parent)
        return last_node(root);

    /* find current index in parent */
    for (int i = 0; i < parent->child_count; i++) {
        if (parent->children[i] == current) {

            /* has previous sibling */
            if (i > 0) {
                Node* prev = parent->children[i - 1];

                if (check_children && prev->child_count > 0)
                    return last_node(prev);

                return prev;
            }

            /* no previous sibling */
            return parent;
        }
    }

    return root;
}

Node* node_focus_prev(Node* root, Node* current) {
    Node* n = prev_node(root, current, 1);
    while (n) {
        if (node_is_focusable(n))
            return n;
        n = prev_node(root, n, 1);
    }
    return NULL;
}

void node_set_ui(Node* node, UI* ui) {
    if (!node)
        return;
    node->ui = ui;
}

UI* node_get_ui(const Node* node) {
    if (!node)
        return NULL;

    if (!node->ui)
        return node_get_ui(node->parent);

    return node->ui;
}