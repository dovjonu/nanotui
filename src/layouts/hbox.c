#include "../node_internal.h"
#include <nanotui/layouts/hbox.h>
#include <stdlib.h>

/* HBox private data */
typedef struct {
    int spacing;
} HBoxData;

static BorderConfig hbox_default_border(Node* self) {
    (void)self;
    return (BorderConfig){
        .mask = BORDER_NONE,
        .style = node_border_style_ascii(),
        //.style = node_border_style_ascii_thick(),
        .title = (BorderTitle){ .text = NULL, .position = BORDER_TITLE_LEFT },
    };
}

static void hbox_measure(Node* self) {
    if (!self)
        return;

    HBoxData* d = self->impl;
    if (!d)
        return;

    int pref_w = 0;
    int pref_h = 0;

    for (int i = 0; i < self->child_count; i++) {
        Node* c = self->children[i];
        if (!c)
            continue;

        if (c->height_hint.pref > pref_h)
            pref_h = c->height_hint.pref;
        pref_w += c->width_hint.pref;
    }

    BorderMask border_mask = node_border_get(self).mask;
    if (border_mask & BORDER_LEFT)
        pref_w += 1;
    if (border_mask & BORDER_RIGHT)
        pref_w += 1;
    if (border_mask & BORDER_TOP)
        pref_h += 1;
    if (border_mask & BORDER_BOTTOM)
        pref_h += 1;

    if (pref_w < 1)
        pref_w = 1;
    if (pref_h < 1)
        pref_h = 1;

    self->width_hint.pref = pref_w;
    self->height_hint.pref = pref_h;
}

/* HBox layout function */
static void hbox_layout(Node* self) {
    if (!self)
        return;

    HBoxData* d = self->impl;
    if (!d)
        return;

    /* Compute inner (content) box after borders */
    int inner_x = self->x;
    int inner_y = self->y;
    int inner_w = self->width;
    int inner_h = self->height;

    BorderMask border_mask = node_border_get(self).mask;

    if (border_mask & BORDER_LEFT) {
        inner_x += 1;
        inner_w -= 1;
    }
    if (border_mask & BORDER_RIGHT) {
        inner_w -= 1;
    }
    if (border_mask & BORDER_TOP) {
        inner_y += 1;
        inner_h -= 1;
    }
    if (border_mask & BORDER_BOTTOM) {
        inner_h -= 1;
    }

    /* Guard against negative or zero space */
    if (inner_w <= 0 || inner_h <= 0)
        return;

    /* No children → nothing to layout */
    if (self->child_count == 0)
        return;




    /* Compute total preferred width (main axis) */
    int total_pref = 0;
    int total_flex = 0;
    int child_count = 0;

    for (int i = 0; i < self->child_count; i++) {
        Node* c = self->children[i];
        if (!c)
            continue;

        child_count++;

        total_pref += c->width_hint.pref;

        if (c->flex > 0)
            total_flex += c->flex;
        
    }

    if (child_count == 0)
        return;

    int total_spacing = d->spacing * (child_count - 1);
    int available_width = inner_w - total_spacing;
    if (available_width <= 0)
        return;

    int extra = available_width - total_pref;
    if (extra < 0)
        extra = 0;  
        
    /* Assign geometry to children (sequential, pref-sized; flex consumes extra) */
    int x = inner_x;
    int remaining = inner_w;
    int remaining_extra = extra;

    for (int i = 0; i < self->child_count; i++) {
        Node* c = self->children[i];
        if (!c)
            continue;

        /* spacing before child (except first visible child) */
        if (x != inner_x) {
            if (remaining < d->spacing)
                break;
            x += d->spacing;
            remaining -= d->spacing;
        }

        int flex_share = 0;
        if (c->flex > 0 && total_flex > 0 && remaining_extra > 0) {
            flex_share = (extra * c->flex) / total_flex;
            if (flex_share > remaining_extra)
                flex_share = remaining_extra;
            remaining_extra -= flex_share;
        }

        int desired_w = c->width_hint.pref + flex_share;
        if (desired_w < 0)
            desired_w = 0;
        if (desired_w > remaining)
            desired_w = remaining;

        node_set_rect(c, x, inner_y, desired_w, inner_h);

        x += desired_w;
        remaining -= desired_w;
        if (remaining <= 0)
            break;

        // if (node_has_focus(self)){
        //     BorderConfig border = node_border_get(self);
        //     BorderStyle style = border.style;
        //     style.top_right = 'O';
        //     style.bottom_right = 'O';
        //     style.top_left = 'O';
        //     style.bottom_left = 'O';
        //     node_border_set_style(self, style);
        // }
        // else {
        //     BorderConfig border = node_border_get(self);
        //     BorderStyle style = border.style;
        //     style.top_right = '+';
        //     style.bottom_right = '+';
        //     style.top_left = '+';
        //     style.bottom_left = '+';
        //     node_border_set_style(self, style);
        // }
    }
}

/* HBox constructor */
Node* hbox_create(int spacing) {
    Node* n = calloc(1, sizeof(Node));
    if (!n)
        return NULL;

    node_init(n);

    HBoxData* d = malloc(sizeof(HBoxData));
    if (!d) {
        free(n);
        return NULL;
    }

    d->spacing = spacing;

    n->layout = hbox_layout;
    n->measure = hbox_measure;
    n->render = NULL;   /* layout nodes do not draw */
    n->impl = d;

    /* Defaults */
    n->default_border = hbox_default_border;

    // node_set_focusable(n, 1);

    return n;
}