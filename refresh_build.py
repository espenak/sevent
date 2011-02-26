#!/usr/bin/env python

import os, shutil

basedir = os.path.dirname(__file__)
builddir = os.path.join(basedir, "build")
if os.path.exists(builddir):
    print "Removing", builddir
    shutil.rmtree(os.path.join(basedir, "build"))
os.mkdir(builddir)
os.chdir(builddir)

cmakecmd = "cmake "\
        "-DCMAKE_CXX_COMPILER=$(which clang++) "\
        "-DCMAKE_BUILD_TYPE=Debug .."
print "Executing:", cmakecmd
os.system(cmakecmd)
