#!/usr/bin/env python

import numpy as np
from distutils.core import setup, Extension

setup(name='computeAUC',
      version='1.0',
      description='Area under the ROC curve',
      author='Fernando Cervantes',
      author_email='iie.fercer@gmail.com',
      
      ext_modules=[Extension('computeAUC', ['include/computeAUC.c'],
                             define_macros=[('BUILDING_PYTHON_MODULE',''), ('NDEBUG',)],
                             include_dirs=[np.get_include()]
                             )],
      )
