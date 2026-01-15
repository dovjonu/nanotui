#ifndef NANOTUI_RENDER_H
#define NANOTUI_RENDER_H

#include <stdint.h>
#include <ncursesw/ncurses.h>
#include <wchar.h>
#include <stdint.h>

typedef struct {
    wchar_t ch;
    attr_t  attr;
    short   color_pair;
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
void render_buffer_put(RenderBuffer* rb, int x, int y, wchar_t ch);
void render_buffer_put_attr(RenderBuffer* rb, int x, int y, wchar_t ch, attr_t attr);
void render_buffer_put_style(RenderBuffer* rb, int x, int y, wchar_t ch, attr_t attrs, short color_pair);

#endif /* NANOTUI_RENDER_H */
