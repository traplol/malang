#!/usr/bin/env python3

import subprocess
import glob

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
    print("{} : {}".format(expected == actual, f))
