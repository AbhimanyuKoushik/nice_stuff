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
    version="1",
    author="Abhimanyu_Koushik",
    description="A Python module for solving linear differential equations using Euler, RK2, RK4, and Trapezoidal methods.",
    ext_modules=ext_modules,
    zip_safe=False,
)
