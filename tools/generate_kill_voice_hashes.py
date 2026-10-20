"""Generate hashes for stock CS2 agent kill and round-result voice events.

Pass a directory of decompiled game_sounds_*.vsndevts files from the installed
CS2 soundevents/vo/agents VPK directory.
"""

from pathlib import Path
import re
import sys


def sound_event_hash(name: str) -> int:
    data = name.lower().encode("ascii")
    mask = 0xFFFFFFFF
    multiplier = 0x5BD1E995
    value = (0x53524332 ^ len(data)) & mask
    index = 0
    while index + 4 <= len(data):
        word = int.from_bytes(data[index:index + 4], "little")
        word = (word * multiplier) & mask
        word ^= word >> 24
        word = (word * multiplier) & mask
        value = ((value * multiplier) & mask) ^ word
        index += 4
    remainder = data[index:]
    if len(remainder) == 3:
        value ^= remainder[2] << 16
    if len(remainder) >= 2:
        value ^= remainder[1] << 8
    if remainder:
        value ^= remainder[0]
        value = (value * multiplier) & mask
    value ^= value >> 13
    value = (value * multiplier) & mask
    value ^= value >> 15
    return value


if len(sys.argv) != 2:
    raise SystemExit("Usage: generate_kill_voice_hashes.py <decompiled-agent-soundevents-dir>")
if sound_event_hash("seal.radio_locknload07") != 1394116:
    raise SystemExit("Sound-event hash does not match the CS2 SDK")

directory = Path(sys.argv[1])
if not directory.is_dir():
    raise SystemExit(f"Missing sound-event directory: {directory}")

names = set()
for file in directory.glob("game_sounds_*.vsndevts"):
    source = file.read_text(encoding="utf-8")
    for name in re.findall(r"^\s*([a-z0-9_]+\.[a-z0-9_]+)\s*=", source, re.M):
        suffix = name.split(".", 1)[1]
        if any(term in suffix for term in (
            "kill", "enemydown", "cheer", "taunt", "brag", "clear", "round_won",
            "round_winning", "round_lost", "round_losing", "victory",
        )):
            names.add(name)

if len(names) < 100:
    raise SystemExit(f"Only {len(names)} kill voice events found; check the input directory")

hashes = sorted((sound_event_hash(name), name) for name in names)
output = Path(__file__).resolve().parents[1] / "src" / "kill_voice_hashes.h"
lines = [
    "// Stock CS2 agent kill and round-result voice hashes from the game assets.",
    "#pragma once",
    "",
    "#include <array>",
    "#include <cstdint>",
    "",
    f"constexpr std::array<std::uint32_t, {len(hashes)}> kKillVoiceHashes {{{{",
]
lines.extend(f"    {hash_value}u, // {name}" for hash_value, name in hashes)
lines.extend(("}};", ""))
output.write_text("\n".join(lines), encoding="utf-8", newline="\n")
print(f"Wrote {len(hashes)} kill voice hashes to {output}")
