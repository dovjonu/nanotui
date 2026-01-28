from . import _nanotui as c

def _mask_from_string(s: str) -> int:
    s = s.lower()
    mask = 0
    if "t" in s: mask |= c.BORDER_TOP
    if "r" in s: mask |= c.BORDER_RIGHT
    if "b" in s: mask |= c.BORDER_BOTTOM
    if "l" in s: mask |= c.BORDER_LEFT
    return mask

def _title_pos(pos: str) -> int:
    pos = pos.lower()
    if pos == "left": return c.BORDER_TITLE_LEFT
    if pos == "center": return c.BORDER_TITLE_CENTER
    if pos == "right": return c.BORDER_TITLE_RIGHT
    raise ValueError("title position must be left/center/right")

class Node:
    def __init__(self, impl):
        self._impl = impl

    def add(self, child: "Node"):
        self._impl.add(child._impl)
        return self

    def border(self, mask="tblr", title=None):
        self._impl.border_mask(_mask_from_string(mask))
        if title is not None:
            text, pos = title
            self._impl.border_title(text, _title_pos(pos))
        return self

class VBox(Node):
    def __init__(self, spacing=0):
        super().__init__(c.VBox(spacing))

class Label(Node):
    def __init__(self, text: str):
        super().__init__(c.Label(text))
