#!/usr/bin/env bash
#
#  mc‑repeat‑toggle.sh
#
#  ‑ When the active window’s title matches the Minecraft profile     → repeat_rate 0
#  ‑ Otherwise                                                        → repeat_rate 25 (or whatever you set)
#
#  Requires: jq, hyprctl

GAME_RATE=0 # DONT CHANGE THIS
DEFAULT_RATE=25 # Whatever you want, 25 is default in Hyprland
MATCH='Minecraft 1\.8\.9'   # Tweak to match Minecraft window title, this is what it should be by default for 1.8.9
POLL=0.2   # Seconds between checks, shouldn't need to mess with it

prev=""

while sleep "$POLL"; do
    # ask Hyprland for the currently focused window, JSON‑format
    json=$(hyprctl activewindow -j 2>/dev/null) || continue
    name=$(jq -r '.title // .class // empty' <<<"$json")

    if [[ $name =~ $MATCH ]]; then
        desired=$GAME_RATE
    else
        desired=$DEFAULT_RATE
    fi

    # only fire hyprctl if it actually changed
    if [[ $desired != "$prev" ]]; then
        hyprctl keyword input:repeat_rate "$desired"
        prev=$desired
    fi
done
