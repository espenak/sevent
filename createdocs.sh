#!/bin/bash

TARFILE=/tmp/sevent-docs.tar
doxygen
tar -C docs/html/ -czf $TARFILE .
git checkout gh-pages
rm -r *
tar -xvf $TARFILE
git add .
