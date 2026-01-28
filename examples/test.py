import nanotui as nt

root = nt.VBox(1)
root.border(mask="tblr", title=("Button Demo", "center"))

root.add(nt.Label("Press Right Arrow key to focus to a next object"))
root.add(nt.Label("Press Enter or Space to activate button"))
root.add(nt.Label("Press q or ESC to quit"))

print("Tree constructed successfully")
