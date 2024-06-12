from setuptools import setup, find_packages

setup(
    name="send_osc",
    version="0.1",
    packages=find_packages(),
    entry_points={
        "console_scripts": [
            "send_osc = send_osc.main:main",
        ],
    },
    install_requires=[
        'python-osc'
    ],
)