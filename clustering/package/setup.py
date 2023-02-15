import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="TBAG",
    version="0.0.4",
    author="Inferno Team",
    author_email="sharoni@gmail.com",
    description="Package to cluster trajectories",
    long_description=long_description,
    long_description_content_type="text/markdown",
    packages=setuptools.find_packages(),
    include_package_data=True,
    package_data={'TBAG': ['lib/trajectory_clustering.dll']},
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: Windows",
    ],
    python_requires='>=3.11',
)
