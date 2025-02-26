from setuptools import setup, Extension
import pybind11

setup(
    ext_modules=[
        Extension(
            "dijkstra",
            ["dijkstra.cpp"],
            include_dirs=[pybind11.get_include()],
            language="c++"
        )
    ]
)
