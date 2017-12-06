#!/usr/bin/env python3

import subprocess
import glob
import os
import sys

script_dir = os.path.dirname(os.path.realpath(__file__))
os.chdir(script_dir)

subprocess.run(['tup'])
num_passed = 0

def run_mal_with(args):
    res = subprocess.run(['./mal'] + args, stdout=subprocess.PIPE)
    return res.stdout

def passed(filename):
    global num_passed
    num_passed += 1
    sys.stdout.write("\033[1;32m PASS: {}\033[0;0m\n".format(filename))

def failed(filename):
    sys.stdout.write("\033[1;31m FAIL: {}\033[0;0m\n".format(filename))

test_dir = 'examples/tests/'
files = glob.glob(test_dir + "*.ma")
for f in files:
    expected = ""
    actual = run_mal_with(['--quiet', f])
    with open(f + ".output", "rb") as exp:
        expected = exp.read()
    #print(actual)
    #print(expected)
    if expected == actual:
        passed(f)
    else:
        failed(f)

print("{}/{} tests produced expected output!".format(num_passed, len(files)))
