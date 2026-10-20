#!/usr/bin/env bash
set -euo pipefail

# Copy this script into the server's game directory, or set CS2_GAME_DIR.
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd -- "${CS2_GAME_DIR:-$script_dir}"
if [[ ! -f ./cs2.sh ]]; then
  printf '%s\n' 'CS2 launcher not found. Copy this script into the game directory or set CS2_GAME_DIR.' >&2
  exit 1
fi

args=(-dedicated -console -usercon -secure -port "${CS2_PORT:-27015}"
  +sv_lan "${CS2_LAN:-0}" +game_type 0 +game_mode 0 +sv_hibernate_when_empty 0
  +sf_enable "${SHROUDFIRE_ENABLE:-1}" +bot_quota 0)
# Read the private token from the config before Steam connects. Do not echo it.
steam_token=''
token_pattern='^[[:space:]]*sv_setsteamaccount[[:space:]]+"?([[:xdigit:]]{32})"?([[:space:]]|$)'
if [[ -r csgo/cfg/server.cfg ]]; then
  while IFS= read -r line || [[ -n "$line" ]]; do
    if [[ "$line" =~ $token_pattern ]]; then
      steam_token="${BASH_REMATCH[1]}"
    fi
  done < csgo/cfg/server.cfg
fi
if [[ -n "$steam_token" ]]; then
  args+=(+sv_setsteamaccount "$steam_token")
fi
exec ./cs2.sh "${args[@]}" +map "${CS2_MAP:-de_dust2}" "$@"
