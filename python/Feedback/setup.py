# -*- coding: utf-8 -*-
"""
Created on Mon June 12

@author: Felix Bauer

Setup for Feedback
"""

from distutils.core import setup


setup(name='Feedback',
    version='1.0',
    description='New feedback for BCI MI',
    author='Felix Bauer',
    author_email='bauerfe@student.ethz.ch',
    packages=['pybci'],
    py_modules=['trialstages'],
    scripts=['mi_online_python'],
    package_data={'pybci': ['pylpttrigger.so']},
    requires=['argparse', 'atexit', 'ConfigParser', 'inspect', 'math', 'lxml',
              'numpy', 'PyQt4', 're', 'socket', 'sys', 'threading', 'time',    
              'xml.dom.minidom', 'websocket']
    )