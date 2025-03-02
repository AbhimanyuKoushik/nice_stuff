#!/usr/bin/env python

from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        "linear_de",
        ["linear_de.cpp"],
        include_dirs=[
            pybind11.get_include(),
            pybind11.get_include(user=True),
        ],
        language="c++",
        extra_compile_args=["-std=c++11"],
    )
]

setup(
    name="linear_de",
    version="0.1",
    author="Your Name",
    description="A Python module for solving linear differential equations using Euler and RK2 methods.",
    ext_modules=ext_modules,
    zip_safe=False,
)
