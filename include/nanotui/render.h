#ifndef NANOTUI_RENDER_H
#define NANOTUI_RENDER_H

#include <stdint.h>

typedef struct {
    uint32_t ch;   /* Unicode codepoint */
    uint32_t attr; /* ncurses attributes */
} Cell;

typedef struct {
    int width;
    int height;
    Cell* cells;
} RenderBuffer;

/* Render buffer lifecycle */
RenderBuffer* render_buffer_create(int width, int height);
void render_buffer_destroy(RenderBuffer* rb);

/* Utilities */
void render_buffer_clear(RenderBuffer* rb);
void render_buffer_put(RenderBuffer* rb, int x, int y, uint32_t ch);
void render_buffer_put_attr(RenderBuffer* rb, int x, int y, uint32_t ch, uint32_t attr);

#endif /* NANOTUI_RENDER_H */
