"""Copy the native opening visual into Thorn pickup; retain every other DBC byte.
Usage: python patch_pickup_visual.py extracted-Spell.dbc output-Spell.dbc
Input is the user's full 1.12 client table, not the server's SQL SpellEntry layout.
"""
import struct, sys
from pathlib import Path
source, output = map(Path, sys.argv[1:])
before = source.read_bytes()
assert before[:4] == b"WDBC"
count, fields, size, strings_size = struct.unpack_from("<4I", before, 4)
assert fields == 173 and size == fields * 4
assert len(before) == 20 + count * size + strings_size
rows = {struct.unpack_from("<I", before, 20+i*size)[0]:20+i*size for i in range(count)}
strings = before[20+count*size:]
def name(spell):
    offset = struct.unpack_from("<I", before, rows[spell]+120*4)[0]
    return strings[offset:].split(b"\0",1)[0]
assert name(59011) == b"Thorn Gorge Flag Pickup" and name(21651) == b"Opening"
# 1.12 Spell.dbc: SpellVisual at field115, names at120. The server's
# expanded SQL SpellEntry field comments describe a different layout.
visual = before[rows[21651]+115*4:rows[21651]+116*4]
assert struct.unpack("<I",visual)[0] == 6139
at = rows[59011]+115*4
assert struct.unpack_from("<I",before,at)[0] in (0,6139)
after = before[:at]+visual+before[at+4:]
assert len(after)==len(before) and after[:at]==before[:at] and after[at+4:]==before[at+4:]
output.parent.mkdir(parents=True,exist_ok=True)
output.write_bytes(after)
print("Verified: only spell59011 SpellVisual changes to native Opening6139; all other bytes preserved")
