"""A setuptools based setup module.
See:
https://packaging.python.org/en/latest/distributing.html
https://github.com/pypa/sampleproject
"""

# Always prefer setuptools over distutils
from setuptools import setup, find_packages
# To use a consistent encoding
from codecs import open
from os import path
import glob
import os
here = path.abspath(path.dirname(__file__))

data_files = []
directories = ["cser_dbg/"]
for directory in directories:
    files = glob.glob(directory+'*')
    files = [x for x in files if os.path.isfile(x)]
    data_files.append((directory, files))
# then pass data_files to setup()


setup(
    name='cser',
    version='0.1.2',

    description='Embedded serial interface tools',
    long_description="",
    author='mike7c2',
    author_email='mike7c2@gmail.com',
    data_files = data_files,
    # You can just specify package directories manually here if your project is
    # simple. Or you can use find_packages().
    #
    # Alternatively, if you just want to distribute a single Python file, use
    # the `py_modules` argument instead as follows, which will expect a file
    # called `my_module.py` to exist:
    #
    #   py_modules=["my_module"],
    #
    packages=["cser"],  # Required
    include_package_data = True,
    # To provide executable scripts, use entry points in preference to the
    # "scripts" keyword. Entry points provide cross-platform support and allow
    # `pip` to create the appropriate form of executable for the target
    # platform.
    #
    # For example, the following would provide a command called `sample` which
    # executes the function `main` from this package when invoked:
    entry_points={
        "console_scripts": [
            'cser-dbg=cser.cser_dbg_target:main',
        ]
    },
    install_requires=[]


)
