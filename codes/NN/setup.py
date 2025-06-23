from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext

ext_modules = [
    Pybind11Extension(
        "neural_net",  # Python module name
        ["Neural_net.cpp", "bindings.cpp"],  # your C++ sources
        cxx_std=17,  # C++17 standard
    ),
]

setup(
    name="neural_net",
    version="0.1.0",
    author="You",
    description="C++ Neural Network with Python bindings via pybind11",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
)