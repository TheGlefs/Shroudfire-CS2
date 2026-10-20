"""Count CS2 Linux signatures from a game-data JSON file in a server binary.

Usage: python scan_linux_signatures.py <libserver.so> <gamedata.json>
This only checks byte-pattern uniqueness, not ABI compatibility or safe calls.
"""

import json
import re
import sys
from pathlib import Path


binary = Path(sys.argv[1]).read_bytes()
gamedata = json.loads(Path(sys.argv[2]).read_text(encoding="utf-8"))

for name, entry in gamedata.items():
    signature = entry.get("signatures", {}).get("linux")
    if not signature or entry["signatures"].get("library") != "server":
        continue
    expression = b"".join(
        b"." if token in ("?", "??") else re.escape(bytes([int(token, 16)]))
        for token in signature.split()
    )
    matches = [match.start() for match in re.finditer(expression, binary, re.DOTALL)]
    locations = ",".join(f"0x{offset:x}" for offset in matches[:3])
    print(f"{name}: {len(matches)} {locations}")
