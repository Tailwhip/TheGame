from __future__ import print_function

import os
import re
import subprocess
import sys

from setuptools import find_packages
from setuptools import setup


def get_version():
    filename = "./drone_trainer/__init__.py"
    with open(filename) as f:
        match = re.search(
            r"""^__version__ = ['"]([^'"]*)['"]""", f.read(), re.M
        )
    if not match:
        raise RuntimeError("{} doesn't contain __version__".format(filename))
    version = match.groups()[0]
    
    return version


def get_install_requires():

    PY3 = sys.version_info[0] == 3
    assert PY3

    with open("./requirements.txt", "r") as f:
        install_requires = f.readlines()
    install_requires = [l.replace("\n", "") for l in install_requires]

    return install_requires


def get_long_description():

    with open("README.md") as f:
        long_description = f.read()

    return long_description


def main():
    version = get_version()

    setup(
        name="drone_trainer",
        version=version,
        packages=find_packages(),
        description="NN models pipelines creator.",
        long_description=get_long_description(),
        long_description_content_type="text/markdown",
        author="Mateusz Sadowski-Zdunek",
        author_email="masdowskizdunek@gmail.com",
        url="https://github.com/Tailwhip/ml_forge",
        install_requires=get_install_requires(),
        license="GPLv3",
        keywords="Machine Learning",
        classifiers=[
            # "Development Status :: 5 - Production/Stable",
            "Intended Audience :: Developers",
            "Natural Language :: English",
            "Programming Language :: Python",
            "Programming Language :: Python :: 3.8",
        ],
        package_data={"drone_trainer": ["icons/*", "config/*.yaml"]},
        entry_points={
            "console_scripts": [
                "drone_trainer=drone_trainer.__main__:main"
                # "clf=ml_forge.cli.classification:main",
            ],
        },
    )


if __name__ == "__main__":
    main()
