import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name='cirrus',
    version='0.1dev',
    description='Python library for Cirrus',
    author="João Carreira",
    author_email='joao [at] berkeley [dot] edu',
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/jcarreira/cirrus.git",
    license=open('LICENSE')
    packages=find_packages(exclude=('tests', 'docs')),
    classifiers=(
        "Programming Language :: C++",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: Unix",
    ),
)
