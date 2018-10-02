import os
import setuptools

# find the shared library that is in the same directory as this script.
lib = next(f for f in os.listdir('.') if f.endswith('.so'))

# TODO: install cli tool too.
setuptools.setup(
    name='pylmadb',
    version='0.0.1',
    author='Sam Kellett',
    author_email='samkellett@gmail.com',
    url='https://github.com/samkellett/lmadb',
    description='LMADB Python bindings',
    long_description='',
    packages=[''],
    package_dir={'': '.'},
    package_data={'': [lib]},
    zip_safe=False,
)
