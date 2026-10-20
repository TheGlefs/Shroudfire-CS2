"""Check custom_hud_layout signatures against a CS2 Linux libserver.so."""

import re
import sys
from pathlib import Path


binary = Path(sys.argv[1]).read_bytes()
patterns = {
    "set_player_dialog": "55 48 89 E5 41 56 41 55 41 54 53 89 F3 48 83 EC ? 85 F6 78 ? 49 89 FD 39 B7 ? ? ? ? 7F ? 48 83 C4 ? 5B 41 5C 41 5D 41 5E 5D C3 ? ? ? 48 89 D6 31 D2 49 89 CE 66 89 55 ? 48 8D 55 ? 4D 89 C4",
    "set_player_class": "55 48 89 E5 41 56 41 55 41 54 53 89 F3 48 83 EC ? 85 F6 78 ? 49 89 FD 39 B7 ? ? ? ? 7F ? 48 83 C4 ? 5B 41 5C 41 5D 41 5E 5D C3 ? ? ? 48 89 D6 31 D2 49 89 CE 66 89 55 ? 48 8D 55 ? 45 89 C4 E8",
    "keyvalues_constructor": "55 48 89 E5 53 48 89 FB 48 89 F7 48 83 EC ? 48 8D 05",
    "keyvalues_find_or_create": "55 48 89 E5 41 56 41 55 41 54 53 48 83 EC ? 8B 06 66 83 7F ? ? 89 45 ? 8B 46 ? 89 45 ? 0F 8F",
    "keyvalues_set_string": "55 48 89 E5 41 56 41 55 49 89 D5 41 54 49 89 F4 53 48 89 FB 48 8B 3F",
}
for name, pattern in patterns.items():
    expression = b"".join(
        b"." if part == "?" else re.escape(bytes([int(part, 16)]))
        for part in pattern.split()
    )
    matches = [match.start() for match in re.finditer(expression, binary, re.DOTALL)]
    print(name, len(matches), [hex(offset) for offset in matches[:5]])
