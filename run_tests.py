#!/usr/bin/env python3

import subprocess
import glob
import os

script_dir = os.path.dirname(os.path.realpath(__file__))
os.chdir(script_dir)

subprocess.run(['tup'])

def run_mal_with(args):
    res = subprocess.run(['./mal'] + args, stdout=subprocess.PIPE)
    return res.stdout

test_dir = 'examples/tests/'
files = glob.glob(test_dir + "*.ma")
for f in files:
    expected = ""
    actual = run_mal_with(['--quiet', f])
    with open(f + ".output", "rb") as exp:
        expected = exp.read()
    #print(actual)
    #print(expected)
    print("{} : {}".format(expected == actual, f))
