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
        .style = node_border_style_unicode_rounded(),
        //.style = node_border_style_ascii_thick(),
        .title = (BorderTitle){ .text = NULL, .position = BORDER_TITLE_LEFT },
    };
}

static int clamp_int(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static int clamp_to_hint(int v, SizeHint hint) {
    int minv = hint.min;
    int maxv = (hint.max >= 0) ? hint.max : 0x7fffffff;
    return clamp_int(v, minv, maxv);
}

static void hbox_measure(Node* self) {
    if (!self) return;

    HBoxData* d = self->impl;
    if (!d) return;

    int pref_w = 0;
    int pref_h = 0;
    int child_count = 0;

    for (int i = 0; i < self->child_count; i++) {
        Node* c = self->children[i];
        if (!c) continue;

        child_count++;

        int cw = clamp_to_hint(c->width_hint.pref, c->width_hint);
        int ch = clamp_to_hint(c->height_hint.pref, c->height_hint);

        pref_w += cw;
        if (ch > pref_h)
            pref_h = ch;
    }

    if (child_count > 1)
        pref_w += d->spacing * (child_count - 1);

    BorderMask border_mask = node_border_get(self).mask;
    if (border_mask & BORDER_LEFT)   pref_w += 1;
    if (border_mask & BORDER_RIGHT)  pref_w += 1;
    if (border_mask & BORDER_TOP)    pref_h += 1;
    if (border_mask & BORDER_BOTTOM) pref_h += 1;

    if (pref_w < 1) pref_w = 1;
    if (pref_h < 1) pref_h = 1;

    self->width_hint.pref  = clamp_to_hint(pref_w, self->width_hint);
    self->height_hint.pref = clamp_to_hint(pref_h, self->height_hint);
}

/* HBox layout function */
static void hbox_layout(Node* self) {
    if (!self) return;

    HBoxData* d = self->impl;
    if (!d) return;

    /* Inner content box after borders */
    int inner_x = self->x;
    int inner_y = self->y;
    int inner_w = self->width;
    int inner_h = self->height;

    BorderMask border_mask = node_border_get(self).mask;

    if (border_mask & BORDER_LEFT)  { inner_x += 1; inner_w -= 1; }
    if (border_mask & BORDER_RIGHT) { inner_w -= 1; }
    if (border_mask & BORDER_TOP)   { inner_y += 1; inner_h -= 1; }
    if (border_mask & BORDER_BOTTOM){ inner_h -= 1; }

    if (inner_w <= 0 || inner_h <= 0)
        return;

    if (self->child_count == 0)
        return;

    /* Compute totals */
    int child_count = 0;
    int total_pref = 0;
    int total_flex = 0;

    for (int i = 0; i < self->child_count; i++) {
        Node* c = self->children[i];
        if (!c) continue;

        child_count++;

        int cw = clamp_to_hint(c->width_hint.pref, c->width_hint);
        total_pref += cw;

        if (c->flex_x > 0)
            total_flex += c->flex_x;
    }

    if (child_count == 0)
        return;

    int total_spacing = d->spacing * (child_count - 1);
    int available_width = inner_w - total_spacing;
    if (available_width <= 0)
        return;

    int extra = available_width - total_pref;
    if (extra < 0) extra = 0;

    /* Layout children left-to-right */
    int x = inner_x;
    int remaining = inner_w;
    int remaining_extra = extra;
    int flex_left = total_flex;
    int laid_out = 0;

    for (int i = 0; i < self->child_count; i++) {
        Node* c = self->children[i];
        if (!c) continue;

        laid_out++;

        int base_w = clamp_to_hint(c->width_hint.pref, c->width_hint);

        int flex_share = 0;
        if (c->flex_x > 0 && total_flex > 0 && remaining_extra > 0) {
            flex_left -= c->flex_x;

            if (flex_left == 0) {
                flex_share = remaining_extra;
            } else {
                flex_share = (extra * c->flex_x) / total_flex;
                if (flex_share > remaining_extra)
                    flex_share = remaining_extra;
            }

            remaining_extra -= flex_share;
        }

        int desired_w = base_w + flex_share;
        desired_w = clamp_to_hint(desired_w, c->width_hint);

        if (desired_w > remaining)
            desired_w = remaining;
        if (desired_w < 0)
            desired_w = 0;

        int desired_h = c->height_hint.pref;
        if (c->flex_y > 0)
            desired_h = inner_h;

        node_set_rect(c, x, inner_y, desired_w, desired_h);

        x += desired_w;
        remaining -= desired_w;

        if (remaining <= 0)
            break;

        if (laid_out < child_count) {
            if (remaining < d->spacing)
                break;
            x += d->spacing;
            remaining -= d->spacing;
        }
    }
}

/* HBox constructor */
Node* hbox_create(int spacing) {
    Node* n = calloc(1, sizeof(Node));
    if (!n) return NULL;

    node_init(n);

    HBoxData* d = malloc(sizeof(HBoxData));
    if (!d) {
        free(n);
        return NULL;
    }

    d->spacing = spacing;

    n->layout  = hbox_layout;
    n->measure = hbox_measure;
    n->render  = NULL; /* layout nodes do not draw */
    n->impl    = d;

    n->default_border = hbox_default_border;

    return n;
}
