# Thorn Gorge client companion

Install the supplied `Data/patch-Z.MPQ` and `Interface/AddOns/ManTechThornGorge`
folder in the Turtle 1.18.1 client, then fully exit and reopen the client.
The server's matching geometry/navmesh update must be installed as well.
Reloading the UI alone does not load an MPQ added after client startup.
Keep another mod's existing patch-Z; do not overwrite it with this package.

The patch contains exactly five files: Thorn Gorge's WorldMapArea bounds and
four `World/Maps/netherstormbg` ADTs. It adds eight existing wooden ramp models
over the raised beams at the four side-bridge ends. No shared model is edited.
The ramps use native map placements and matching extracted server collision.
Their measured centerline slopes are approximately 4–27 degrees.

The addon fits the original map artwork into the expanded geographic rectangle
on the world map and the load-on-demand battlefield minimap. Native player,
landmark and carrier coordinate APIs are retained. It changes the carrier icon
to the holder's faction colour and displays the server's flag-return countdown.
The existing 1.12 carrier-position packet has no team/timer field, so the server
sends a small `MT_TG1` addon message only to human sessions in this BG.
The message contains no account information and causes no server-side action.

## Rebuilding

- Extract the four original ADTs named in `bridge-ramps.json` from the matching
  client patch-9. `build_bridge_adts.py SOURCE_ADT_DIRECTORY PATCH_CONTENTS`
  verifies their SHA-256 hashes, adds the placements, updates doodad references
  and offsets, and validates the resulting chunk layout.
- `build_map_bounds.py ORIGINAL_WORLDMAPAREA.dbc PATCH_CONTENTS/DBFilesClient/WorldMapArea.dbc`
  changes only map 821's rectangle. Every other row and the string block remain
  byte-for-byte intact. Use the matching addon when changing these bounds.
- Pack only the five game files into a new MPQ. Keep the build manifest outside
  the archive. A map-821-only Map.dbc used to limit an offline extraction is a
  build input only: **never package that reduced table for a client or server**.
- Use the native vmap extractor/assembler and MoveMapGen with the patched ADTs
  to rebuild all map-821 tree/tile files. Do not edit live data during a match.
- Run the native asset probe with `--nodes 16384 --exclude-steep`, and run
  `tests/architecture/ThornClientUiTest.lua` with this addon as its first argument.

For rollback, remove this companion from the active client (retain a backup),
restore the prior server binary/config, and select the previous complete data
directory. Both client and server require restarting. Live visual fitting,
short-character passage and bot traversal still need in-game acceptance.


Pickup animation update: the matching patch-Z.MPQ also contains the full client
Spell.dbc with only spell59011 SpellVisual copied from native Opening21651.
Use patch_pickup_visual.py on the extracted full1.12 table to reproduce it. The
script validates field115 and name anchors at120; server SQL SpellEntry indices
are different. Do not substitute a reduced table. Fully restart the client.
