import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="cirrus",
    version="0.0.1",
    author="jcarreira",
    author_email="joao@berkeley.edu",
    description="Serverless Machine Learning platform",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/jcarreira/cirrus.git",
    packages=setuptools.find_packages(),
    classifiers=(
        "Programming Language :: C++",
        "License :: OSI Approved :: MIT License",
        "Operating System :: Unix",
    ),
)