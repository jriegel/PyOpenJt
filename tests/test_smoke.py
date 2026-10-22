"""Smoke test for an installed PyOpenJt wheel.

Usage: python test_smoke.py <path-to-jt-file>
"""
import sys

import PyOpenJt


def main(jt_path):
    jt = PyOpenJt.JtFile(jt_path)
    assert jt.FileName == jt_path

    jt.FileName = "umlaut-\u00e4.jt"
    assert jt.FileName == "umlaut-\u00e4.jt"
    print("PyOpenJt smoke test passed")


if __name__ == "__main__":
    main(sys.argv[1])
