#!/usr/bin/env python3

from subprocess import run, PIPE
from sys import exit

input = """2
1 1 0 2 1 3
5 0 3 1 5 2
"""

xfree_process = run(["./XFREE/xfree"], stdout=PIPE, input=input, encoding='ascii')

if xfree_process.returncode is not 0:
  print("xfree command failed.")
  exit(1)

xfree_output = xfree_process.stdout

print("xfree output:\n{}".format(xfree_output))

topo_process = run(["./CTR/topo_simplify"], stdout=PIPE, input=xfree_output, encoding='ascii')

if topo_process.returncode is not 0:
  print("topo_simplify command failed.")
  exit(1)

topo_output = topo_process.stdout

print("topo_simplify output:\n{}".format(topo_output))