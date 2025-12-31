#include <nanotui/ui.h>

#include <nanotui/layouts/hbox.h>
#include <nanotui/layouts/vbox.h>
#include <nanotui/widgets/label.h>
#include <nanotui/widgets/button.h>
#include <nanotui/widgets/checkbox.h>
#include <nanotui/widgets/line_edit.h>
#include <nanotui/node.h>

#include <stdlib.h>
#include <stdio.h>

#include <pthread.h>
#include <unistd.h>
typedef struct {
    Node* label;
} ClearLabelData;

void* clear_label_thread(void* arg) {
    ClearLabelData* data = (ClearLabelData*)arg;
    sleep(2);
    label_set_text(data->label, "");
    free(data);
    return NULL;
}

void on_button_pressed(Node* button, void* user_data) {
    (void)button;
    (void)user_data;

    char* button_text = button_get_text(button);
    if (!button_text)
        return;

    Node* label = (Node*)user_data;
    if (!label)
        return;

    char* output = malloc(256);
    if (!output)
        return;

    snprintf(output, 256, "Button '%s' pressed", button_text);
    label_set_text(label, output);
    free(output);

    // after 2 seconds, clear the label by spawning a thread
    ClearLabelData* data = malloc(sizeof(ClearLabelData));
    data->label = label;
    pthread_t thread;
    pthread_create(&thread, NULL, clear_label_thread, data);
    pthread_detach(thread);
}

void on_checkbox_toggled(Node* checkbox, void* user_data) {
    (void)checkbox;
    (void)user_data;

    char* checkbox_text = checkbox_get_text(checkbox);
    if (!checkbox_text)
        return;

    Node* label = (Node*)user_data;
    if (!label)
        return;

    char* output = malloc(256);
    if (!output)
        return;

    int checked = checkbox_is_checked(checkbox);
    snprintf(output, 256, "Checkbox '%s' toggled to %s", checkbox_text, checked ? "checked" : "unchecked");
    label_set_text(label, output);
    free(output);
}

int main(void) {
    UI* ui = ui_create();

    Node* root = hbox_create(0);
    node_border_set_mask(root, BORDER_TOP | BORDER_BOTTOM | BORDER_LEFT | BORDER_RIGHT);
    //node_set_flex(root, 5);

    Node* left = vbox_create(1);
    node_border_set_mask(left, BORDER_TOP | BORDER_BOTTOM | BORDER_LEFT | BORDER_RIGHT);
    node_border_set_title(left, (BorderTitle){ .text = "Button Demo", .position = BORDER_TITLE_CENTER });
    node_set_flex(left, 3);
    node_add_child(left, label_create("Press Right Arrow key to focus to a next object"));
    node_add_child(left, label_create("Press Enter or Space to activate button"));
    node_add_child(left, label_create("Press q or ESC to quit"));
    node_add_child(left, label_create(""));
    Node* output = label_create("Output will appear here");
    node_add_child(left, output);

    node_add_child(root, left);

    Node* right = vbox_create(1);
    node_border_set_mask(right, BORDER_TOP | BORDER_BOTTOM | BORDER_LEFT | BORDER_RIGHT);
    node_border_set_title(right, (BorderTitle){ .text = "More Buttons", .position = BORDER_TITLE_LEFT });
    node_set_flex(right, 2);
    node_add_child(right, button_create("Button 1", on_button_pressed, output));
    node_add_child(right, button_create("Button 2", on_button_pressed, output));
    node_add_child(right, button_create("Button 3", on_button_pressed, output));
    node_add_child(right, checkbox_create("Checkbox 1", on_checkbox_toggled, output));
    node_add_child(right, checkbox_create("Checkbox 2", on_checkbox_toggled, output));
    node_add_child(right, line_edit_create(10));


    node_add_child(root, right);

    ui_set_root(ui, root);

    ui_run(ui);
    ui_destroy(ui);
    return 0;
}
