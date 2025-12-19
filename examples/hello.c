#include <nanotui/ui.h>

int main(void) {
    UI* ui = ui_create();
    ui_run(ui);
    ui_destroy(ui);
    return 0;
}
