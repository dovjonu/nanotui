#include <nanotui/ui.h>

#include <nanotui/layouts/hbox.h>
#include <nanotui/layouts/vbox.h>
#include <nanotui/widgets/label.h>
#include <nanotui/widgets/button.h>
#include <nanotui/node.h>

#include <stdlib.h>

void on_ok_pressed(Node* button, void* user_data) {
    (void)button;
    (void)user_data;

    Node* label = (Node*)user_data;
    if (!label)
        return;

    label_set_text(label, "OK button pressed");
}

void on_cancel_pressed(Node* button, void* user_data) {
    (void)button;
    (void)user_data;

    Node* label = (Node*)user_data;
    if (!label)
        return;

    label_set_text(label, "CANCEL button pressed");
}

int main(void) {
    UI* ui = ui_create();

    Node* root = vbox_create(1);
    node_border_set_mask(root, BORDER_TOP | BORDER_BOTTOM | BORDER_LEFT | BORDER_RIGHT);
    node_border_set_title(root, (BorderTitle){ .text = "Button Demo", .position = BORDER_TITLE_CENTER });

    node_add_child(root, label_create("Press Right Arrow key to focus to a next object"));
    node_add_child(root, label_create("Press Enter or Space to activate button"));
    node_add_child(root, label_create("Press q or ESC to quit"));
    node_add_child(root, label_create(""));
    Node* output = label_create("Output will appear here");
    node_add_child(root, output);
    node_add_child(root, label_create(""));

    Node* button_box = hbox_create(2);
    node_border_set_mask(button_box, BORDER_TOP | BORDER_BOTTOM | BORDER_LEFT | BORDER_RIGHT);
    node_add_child(button_box, button_create("OK", on_ok_pressed, output));
    node_add_child(button_box, button_create("Cancel", on_cancel_pressed, output));
    node_add_child(button_box, button_create("Apply", on_ok_pressed, NULL));
    node_add_child(root, button_box);

    node_add_child(root, label_create(""));

    Node* second = vbox_create(1);
    node_border_set_mask(second, BORDER_TOP | BORDER_BOTTOM | BORDER_LEFT | BORDER_RIGHT);
    node_border_set_title(second, (BorderTitle){ .text = "More Buttons", .position = BORDER_TITLE_LEFT });
    node_add_child(second, button_create("Button 1", NULL, NULL));
    node_add_child(second, button_create("Button 2", NULL, NULL));
    node_add_child(second, button_create("Button 3", NULL, NULL));
    node_add_child(root, second);

    ui_set_root(ui, root);

    ui_run(ui);
    ui_destroy(ui);
    return 0;
}
