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

void render_buffer_put(RenderBuffer* rb, int x, int y, uint32_t ch) {
    if (!rb) return;
    if (x < 0 || y < 0 || x >= rb->width || y >= rb->height) return;
    rb->cells[y * rb->width + x].ch = ch;
}

void render_buffer_put_attr(RenderBuffer* rb, int x, int y, uint32_t ch, uint32_t attr) {
    if (!rb) return;
    if (x < 0 || y < 0 || x >= rb->width || y >= rb->height) return;
    rb->cells[y * rb->width + x].ch = ch;
    rb->cells[y * rb->width + x].attr = attr;
}
