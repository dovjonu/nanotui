#ifndef NANOTUI_UI_H
#define NANOTUI_UI_H

/* forward declarations */
typedef struct UI UI;
typedef struct Node Node;

UI* ui_create(void);
void ui_set_root(UI* ui, Node* root);
Node* ui_get_root(const UI* ui);
void ui_run(UI* ui);
void ui_destroy(UI* ui);
void ui_lock(UI* ui);
void ui_unlock(UI* ui);
int ui_get_locked(const UI* ui);

#endif
