# -*- coding: utf-8 -*-
from distutils.core import setup, Extension

setup(
        name="py_kalman",
        ext_modules=[Extension("py_kalman", sources = ["python_bindings.c"])],
        version="0.1",
        author="Joshua Semar",
        author_email="joshua.semar@mapmyfitness.com",
        url="https://github.com/mapmyfitness/ikalman",
        description = 'Python bindings for the ikalman c library'
      )

