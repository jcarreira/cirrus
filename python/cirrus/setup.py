import setuptools

with open("README.md", "r") as fh:
  long_description = fh.read()

setuptools.setup(
    name="cirrus",
    version="0.0.1",
    author="jcarreira",
    author_email="joao@berkeley.edu",
    description="Serverless ML Training on Lambdas",
    long_description = long_description,
    url="https://github.com/jcarreira/cirrus.git",
    packages=setuptools.find_packages(),
    classifiers=(
      "Programming Language :: Python :: 3",
      "License :: OSI Approved :: MIT License",
      "Operating System :: Unix"
    )
)
