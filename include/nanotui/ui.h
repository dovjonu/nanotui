#ifndef NANOTUI_UI_H
#define NANOTUI_UI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque UI type */
typedef struct UI UI;

/* Lifecycle */
UI* ui_create(void);
void ui_run(UI* ui);
void ui_destroy(UI* ui);

#ifdef __cplusplus
}
#endif

#endif /* NANOTUI_UI_H */