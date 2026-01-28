from setuptools import setup, Extension

ext = Extension(
    "nanotui._nanotui",
    sources=[
        "nanotui/_nanotui.c",
        "../src/node.c",
        "../src/layouts/vbox.c",
        "../src/widgets/label.c",
        "../src/render.c",
    ],
    include_dirs=[
        "../include",
        "../src",   # only if needed
    ],
    libraries=["ncursesw"],
)

setup(
    name="nanotui",
    version="0.1.0",
    packages=["nanotui"],
    ext_modules=[ext],
)
