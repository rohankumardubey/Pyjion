import pyjion
import pyjion.dis
import re
from base import PyjionTestCase


class ProblemTestCase(PyjionTestCase):
    """
    Test problematic and complex functions
    """

    def test_regexps(self):
        print(pyjion.dis.dis(re.sre_compile.compile, True))
        print(pyjion.dis.dis_native(re.sre_compile.compile, True))

        def by(s):
            return bytearray(map(ord, s))
        b = by("Hello, world")
        self.assertEqual(re.findall(br"\w+", b), [by("Hello"), by("world")])
