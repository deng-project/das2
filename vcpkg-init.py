#!/usr/bin/python3

import os
import sys
import subprocess

VCPKG_PATH = "./ThirdParty/vcpkg"
PLATFORM = ""

if sys.platform.startswith("linux"):
    PLATFORM = "linux"
elif sys.platform.startswith("win32"):
    PLATFORM = "windows"
elif sys.platform.startswith("darwin"):
    PLATFORM = "osx"

VCPKG_PACKAGES = [ f"zlib:x64-{PLATFORM}" ]

def check_and_install_vcpkg():
    vcpkg_exec = VCPKG_PATH + "/vcpkg"
    if (PLATFORM == "windows"):
        vcpkg_exec = vcpkg_exec + ".exe"

    if not os.path.isfile(vcpkg_exec):
        if (PLATFORM == "windows"):
            cmd = [ "cmd", "/C", os.path.abspath(VCPKG_PATH + "/bootstrap-vcpkg.bat"), "-disableMetrics" ]
            p = subprocess.run(cmd)
        else:
            cmd = [ 'sh', os.path.abspath(VCPKG_PATH + "/bootstrap-vcpkg.sh"), "-disableMetrics" ]
            p = subprocess.run(cmd)

    cmd = [ os.path.abspath(vcpkg_exec), "install" ]
    for dep in VCPKG_PACKAGES:
        cmd.append(dep)

    p = subprocess.run(cmd)

check_and_install_vcpkg()
