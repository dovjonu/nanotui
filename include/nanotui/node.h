#ifndef NANOTUI_NODE_H
#define NANOTUI_NODE_H

#include <nanotui/render.h>
#include <nanotui/ui.h>

typedef enum {
	BORDER_NONE = 0,
	BORDER_TOP    = 1 << 0,
	BORDER_RIGHT  = 1 << 1,
	BORDER_BOTTOM = 1 << 2,
	BORDER_LEFT   = 1 << 3,
} BorderMask;

typedef struct {
	char top;
	char bottom;
	char left;
	char right;
	char top_left;
	char top_right;
	char bottom_left;
	char bottom_right;
} BorderStyle;

typedef enum {
    BORDER_TITLE_LEFT,
    BORDER_TITLE_CENTER,
    BORDER_TITLE_RIGHT
} BorderTitlePosition;

typedef struct {
    const char* text;
    BorderTitlePosition position;
} BorderTitle;


typedef struct {
	BorderMask mask;
	BorderStyle style;
    BorderTitle title;
} BorderConfig;

typedef enum {
	BORDER_STYLE_TOP          = 1 << 0,
	BORDER_STYLE_RIGHT        = 1 << 1,
	BORDER_STYLE_BOTTOM       = 1 << 2,
	BORDER_STYLE_LEFT         = 1 << 3,
	BORDER_STYLE_TOP_LEFT     = 1 << 4,
	BORDER_STYLE_TOP_RIGHT    = 1 << 5,
	BORDER_STYLE_BOTTOM_LEFT  = 1 << 6,
	BORDER_STYLE_BOTTOM_RIGHT = 1 << 7,
} BorderStyleMask;

typedef struct {
    int min;
    int pref;
    int max;   // -1 = inf
} SizeHint;

typedef struct Node Node;

/* Geometry */
void node_set_rect(Node* node, int x, int y, int width, int height);

/* Tree manipulation */
void node_add_child(Node* parent, Node* child);

/* Rendering entry point */
void node_render(Node* node, RenderBuffer* rb);

/* Measurement + sizing */
void node_measure(Node* node);

void node_set_width_hint(Node* node, SizeHint hint);
void node_set_height_hint(Node* node, SizeHint hint);
SizeHint node_get_width_hint(const Node* node);
SizeHint node_get_height_hint(const Node* node);

void node_set_flex(Node* node, int flex);
int node_get_flex(const Node* node);

/* Border helpers */
BorderStyle node_border_style_ascii(void);
BorderStyle node_border_style_ascii_thick(void);
BorderConfig node_border_get(const Node* node);

void node_border_clear_override(Node* node);
void node_border_set_config(Node* node, BorderConfig config);
void node_border_set_mask(Node* node, BorderMask mask);
void node_border_set_style(Node* node, BorderStyle style);
void node_border_set_title(Node* node, BorderTitle title);
void node_border_patch_style(Node* node, BorderStyleMask fields, BorderStyle patch);


// Focus / input
void node_set_focusable(Node* node, int focusable);
int  node_is_focusable(const Node* node);

void node_set_focus(Node* node, int focused);
int  node_has_focus(const Node* node);

// Dispatch one key to a node (calls node->on_key if present)
int node_handle_key(Node* node, int key);

// Find next focusable in tree
Node* node_focus_next(Node* root, Node* current);
Node* node_focus_prev(Node* root, Node* current);

void node_set_ui(Node* node, UI* ui);
UI* node_get_ui(const Node* node);

void node_destroy(Node* node);

#endif
