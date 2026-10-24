"""Regression test: compares the JSON output of JtDump.exe with stored references.

Usage: python TestJtDump.py [--update] [--exe PATH] [--jt NAME_OR_PATH ...]
Returns 0 if all outputs match their references, 1 otherwise.
--update writes the current outputs as the new references.
"""
import argparse
import difflib
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXAMPLES = ROOT / "Data" / "ExampleFiles"
REFERENCES = ROOT / "Data" / "Reference"

DEFAULT_FILES = [
    "example_block_jt8.1.jt",
    "example_block_jt9.5.jt",
    "example_block_jt10.3.jt",
    "opening_protection_plate1_jt8.0.jt",
    "opening_protection_plate1_jt9.5.jt",
]


def dump(exe, jt):
    """Runs JtDump and returns the normalized JSON text, or raises RuntimeError."""
    res = subprocess.run([str(exe), str(jt)], capture_output=True)
    if res.returncode != 0:
        raise RuntimeError(f"JtDump failed with exit code {res.returncode}\n"
                           + res.stderr.decode("utf-8", "replace"))
    try:
        # normalize formatting so that only content differences count
        return json.dumps(json.loads(res.stdout.decode("utf-8", "replace")), indent=2)
    except json.JSONDecodeError as e:
        raise RuntimeError(f"JtDump output is not valid JSON: {e}")


def check(exe, jt, update):
    ref = REFERENCES / (jt.stem + ".json")
    try:
        actual = dump(exe, jt)
    except RuntimeError as e:
        print(f"FAIL {jt.name}: {e}", file=sys.stderr)
        return False

    if update:
        ref.parent.mkdir(parents=True, exist_ok=True)
        ref.write_text(actual + "\n", encoding="utf-8", newline="\n")
        print(f"Reference written: {ref}")
        return True

    if not ref.exists():
        print(f"FAIL {jt.name}: reference missing: {ref} (run with --update to create)",
              file=sys.stderr)
        return False

    expected = ref.read_text(encoding="utf-8").rstrip("\n")
    if expected == actual:
        print(f"PASS {jt.name}")
        return True

    print(f"FAIL {jt.name}: JSON output differs from reference")
    for line in difflib.unified_diff(expected.splitlines(), actual.splitlines(),
                                     "reference", "actual", lineterm=""):
        print(line)
    return False


def main():
    p = argparse.ArgumentParser()
    p.add_argument("--exe", default=ROOT / "WinBuild" / "Debug" / "JtDump.exe")
    p.add_argument("--jt", nargs="+", default=DEFAULT_FILES,
                   help="JT files (name inside Data/ExampleFiles or path)")
    p.add_argument("--update", action="store_true", help="store current output as reference")
    args = p.parse_args()

    exe = Path(args.exe)
    if not exe.exists():
        print(f"JtDump not found: {exe}", file=sys.stderr)
        return 1

    ok = True
    for name in args.jt:
        path = Path(name)
        if not path.exists():
            path = EXAMPLES / name
        ok = check(exe, path, args.update) and ok
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
