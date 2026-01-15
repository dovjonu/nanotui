#include <stdlib.h>
#include <string.h>

#include <nanotui/render.h>

RenderBuffer* render_buffer_create(int width, int height) {
    RenderBuffer* rb = malloc(sizeof(RenderBuffer));
    if (!rb) return NULL;

    rb->width = width;
    rb->height = height;
    rb->cells = malloc(sizeof(Cell) * width * height);

    if (!rb->cells) {
        free(rb);
        return NULL;
    }

    render_buffer_clear(rb);
    return rb;
}

void render_buffer_destroy(RenderBuffer* rb) {
    if (!rb) return;
    free(rb->cells);
    free(rb);
}

void render_buffer_clear(RenderBuffer* rb) {
    if (!rb) return;
    for (int i = 0; i < rb->width * rb->height; i++) {
        rb->cells[i].ch = ' ';
        rb->cells[i].attr = 0;
    }
}

void render_buffer_put(RenderBuffer* rb, int x, int y, wchar_t ch) {
    if (!rb) return;
    if (x < 0 || y < 0 || x >= rb->width || y >= rb->height) return;
    rb->cells[y * rb->width + x].ch = ch;
}

void render_buffer_put_attr(RenderBuffer* rb, int x, int y, wchar_t ch, attr_t attr) {
    short color_pair = PAIR_NUMBER(attr);
    attr_t clean_attrs = attr & ~A_COLOR;
    render_buffer_put_style(rb, x, y, ch, clean_attrs, color_pair);
}

void render_buffer_put_style(RenderBuffer* rb, int x, int y, wchar_t ch, attr_t attrs, short color_pair)
{
    if (!rb) return;
    if (x < 0 || y < 0 || x >= rb->width || y >= rb->height) return;

    Cell* c = &rb->cells[y * rb->width + x];
    c->ch = ch;
    c->attr = attrs;
    c->color_pair = color_pair;
}
