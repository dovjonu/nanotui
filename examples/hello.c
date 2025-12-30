#include <nanotui/ui.h>

#include <nanotui/layouts/hbox.h>
#include <nanotui/layouts/vbox.h>
#include <nanotui/widgets/label.h>
#include <nanotui/node.h>

int main(void) {
    UI* ui = ui_create();

    Node* root = vbox_create(1);
    node_border_set_mask(root, BORDER_TOP | BORDER_BOTTOM | BORDER_LEFT | BORDER_RIGHT);
    node_border_set_title(root, (BorderTitle){ .text = "Hello Nanotui", .position = BORDER_TITLE_RIGHT });

    node_add_child(root, label_create("Hello"));
    node_add_child(root, label_create("World"));
    node_add_child(root, label_create("!"));

    Node* second = vbox_create(1);
    node_border_set_mask(second, BORDER_TOP | BORDER_BOTTOM | BORDER_LEFT | BORDER_RIGHT);
    node_border_set_title(second, (BorderTitle){ .text = "Title 2", .position = BORDER_TITLE_LEFT });
    node_add_child(second, label_create("Nested"));
    node_add_child(second, label_create("VBox"));
    node_add_child(root, second);

    ui_set_root(ui, root);

    ui_run(ui);
    ui_destroy(ui);
    return 0;
}
