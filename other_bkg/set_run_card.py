#!/usr/bin/env python3
"""Set values in a MadGraph run_card.dat without the interactive card editor.
usage: python3 set_run_card.py Cards/run_card.dat key=value [key=value ...]
A key that is not in the card (a hidden parameter) is appended at the end."""
import re
import sys

path, settings = sys.argv[1], dict(a.split("=", 1) for a in sys.argv[2:])
lines = open(path).read().splitlines()
done = set()
for i, line in enumerate(lines):
    m = re.match(r"^(\s*)(\S.*?)(\s*=\s*)(\w+)(\s*!.*)?$", line)
    if m and not line.lstrip().startswith("#") and m.group(4) in settings:
        k = m.group(4)
        lines[i] = "%s%s%s%s%s" % (m.group(1), settings[k], m.group(3), k, m.group(5) or "")
        done.add(k)
for k, v in settings.items():
    if k not in done:
        lines.append(" %s = %s ! added by set_run_card.py" % (v, k))
open(path, "w").write("\n".join(lines) + "\n")
print("run_card: " + ", ".join("%s = %s" % kv for kv in settings.items()))
