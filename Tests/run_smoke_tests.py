import sys
import pyjion
import re
import os
import gc
import test.libregrtest.cmdline
import unittest

args = test.libregrtest.cmdline._parse_args(sys.argv[1:])

SAVEDCWD = os.getcwd()
tests = []

# Lifted from libregr.main
regex = re.compile(r'\btest_[a-zA-Z0-9_]+\b')
with open(os.path.join(SAVEDCWD, args.fromfile)) as fp:
    for line in fp:
        line = line.split('#', 1)[0]
        line = line.strip()
        match = regex.search(line)
        if match is not None:
            tests.append(match.group())

has_failures = False

OPT_LEVELS = [1]

for opt_level in OPT_LEVELS:
    for test in tests:
        test_cases = unittest.defaultTestLoader.loadTestsFromName(f"test.{test}")
        print(f"Testing {test}")
        for case in test_cases:
            pyjion.enable()
            print(f"Trying with Optimizations = {opt_level}")
            pyjion.set_optimization_level(opt_level)
            r = unittest.result.TestResult()
            case.run(r)
            if r.wasSuccessful():
                print(f"All tests in case successful.")
            else:
                print(f"Failures occurred.")
                has_failures = True

                for failedcase, reason in r.expectedFailures:
                    print(f"---------------------------------------------------------------")
                    print(f"Test case {failedcase} was expected to fail:")
                    print(reason)
                    print(f"---------------------------------------------------------------")

                for failedcase, reason in r.failures:
                    print(f"---------------------------------------------------------------")
                    print(f"Test case {failedcase} failed:")
                    print(reason)
                    print(f"---------------------------------------------------------------")

                for failedcase, reason in r.errors:
                    print(f"---------------------------------------------------------------")
                    print(f"Test case {failedcase} failed with errors:")
                    print(reason)
                    print(f"---------------------------------------------------------------")

            pyjion.disable()
            gc.collect()

if has_failures:
    exit(1)
else:
    exit(0)
