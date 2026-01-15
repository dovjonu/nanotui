#include "../node_internal.h"
#include <nanotui/layouts/vbox.h>
#include <stdlib.h>

/* VBox private data */
typedef struct {
    int spacing;
} VBoxData;

static BorderConfig vbox_default_border(Node* self) {
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
    int maxv = (hint.max >= 0) ? hint.max : 0x7fffffff; /* "infinite" */
    return clamp_int(v, minv, maxv);
}

static void vbox_measure(Node* self) {
    if (!self) return;

    VBoxData* d = self->impl;
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

        if (cw > pref_w) pref_w = cw;
        pref_h += ch;
    }

    if (child_count > 1)
        pref_h += d->spacing * (child_count - 1);

    BorderMask border_mask = node_border_get(self).mask;
    if (border_mask & BORDER_LEFT)   pref_w += 1;
    if (border_mask & BORDER_RIGHT)  pref_w += 1;
    if (border_mask & BORDER_TOP)    pref_h += 1;
    if (border_mask & BORDER_BOTTOM) pref_h += 1;

    if (pref_w < 1) pref_w = 1;
    if (pref_h < 1) pref_h = 1;

    /* Respect this node's own hints too (optional but safer) */
    self->width_hint.pref  = clamp_to_hint(pref_w, self->width_hint);
    self->height_hint.pref = clamp_to_hint(pref_h, self->height_hint);
}

/* VBox layout function */
static void vbox_layout(Node* self) {
    if (!self) return;

    VBoxData* d = self->impl;
    if (!d) return;

    /* Compute inner (content) box after borders */
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

    /* Count non-null children and compute totals */
    int child_count = 0;
    int total_pref = 0;      /* sum of preferred heights (clamped to child hints) */
    int total_flex = 0;      /* sum of flex_y */
    for (int i = 0; i < self->child_count; i++) {
        Node* c = self->children[i];
        if (!c) continue;

        child_count++;

        int ch_pref = clamp_to_hint(c->height_hint.pref, c->height_hint);
        total_pref += ch_pref;

        if (c->flex_y > 0)
            total_flex += c->flex_y;
    }

    if (child_count == 0)
        return;

    int total_spacing = d->spacing * (child_count - 1);
    int available_height = inner_h - total_spacing;
    if (available_height <= 0)
        return;

    int extra = available_height - total_pref;
    if (extra < 0) extra = 0;

    /* Layout children top-to-bottom:
       - base height = child preferred (clamped)
       - + flex share of extra space (using flex_y)
       - spacing applied AFTER each child except the last
       - last flex child gets any remaining rows from integer division rounding
    */
    int y = inner_y;
    int remaining = inner_h;
    int remaining_extra = extra;
    int flex_left = total_flex;

    int laid_out_index = 0; /* number of visible children laid out so far */

    for (int i = 0; i < self->child_count; i++) {
        Node* c = self->children[i];
        if (!c) continue;

        laid_out_index++;

        /* base preferred height (clamped) */
        int base_h = clamp_to_hint(c->height_hint.pref, c->height_hint);

        /* distribute extra height by flex_y */
        int flex_share = 0;
        if (c->flex_y > 0 && total_flex > 0 && remaining_extra > 0) {
            flex_left -= c->flex_y;

            if (flex_left == 0) {
                /* last flex child gets all leftover to avoid losing rows */
                flex_share = remaining_extra;
            } else {
                flex_share = (extra * c->flex_y) / total_flex;
                if (flex_share > remaining_extra)
                    flex_share = remaining_extra;
            }

            remaining_extra -= flex_share;
        }

        int desired_h = base_h + flex_share;

        /* enforce child's min/max on final height */
        desired_h = clamp_to_hint(desired_h, c->height_hint);

        /* clip to remaining space */
        if (desired_h > remaining)
            desired_h = remaining;
        if (desired_h < 0)
            desired_h = 0;

        int desired_w = c->width_hint.pref;
        if (c->flex_x > 0)
            desired_w = inner_w;
        node_set_rect(c, inner_x, y, desired_w, desired_h);

        y += desired_h;
        remaining -= desired_h;

        if (remaining <= 0)
            break;

        /* spacing after child except the last visible child */
        if (laid_out_index < child_count) {
            if (remaining < d->spacing)
                break;
            y += d->spacing;
            remaining -= d->spacing;
        }
    }
}

/* VBox constructor */
Node* vbox_create(int spacing) {
    Node* n = calloc(1, sizeof(Node));
    if (!n) return NULL;

    node_init(n);

    VBoxData* d = malloc(sizeof(VBoxData));
    if (!d) {
        free(n);
        return NULL;
    }

    d->spacing = spacing;

    n->layout = vbox_layout;
    n->measure = vbox_measure;
    n->render = NULL;   /* layout nodes do not draw */
    n->impl = d;

    /* Defaults */
    n->default_border = vbox_default_border;

    return n;
}
