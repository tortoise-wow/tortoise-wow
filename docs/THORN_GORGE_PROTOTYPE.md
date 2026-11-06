# Thorn Gorge prototype

2026-09-08; developed from baseline `94c76b96` on `feature/thorn-gorge-prototype`.
This is an adaptation for testing with provisional balance, not a claim to
reproduce the official server's implementation.

## Enable on a separate test realm

1. Use isolated test world, character and login databases. Do not point a second
   world process at the production character database. Keep its realm ID, ports
   and login endpoint separate from production.
2. With the test process stopped, apply `sql/custom/thorn_gorge_prototype.sql`
   to its world database. It adds battleground 6, six battlemaster bindings and
   two start orientations. It deliberately rejects existing rows; inspect any
   conflict instead of replacing it. Some affected tables are MyISAM, so a
   transaction alone cannot undo a partially applied script.
3. Install the prototype `mangosd.exe` with its matching PDB and existing runtime
   DLLs. Use extracted Turtle DBC/maps/vmaps/mmaps, including map 821.
4. In the battleground section of the test `mangosd.conf`, add:

   ```ini
   Battleground.ThornGorge.Enabled = 1
   Battleground.ThornGorge.FlagX = 2174.469482
   Battleground.ThornGorge.FlagY = 1569.349243
   ```

5. Start the test realm. Join Thorn Gorge from the Turtle minimap queue or an
   existing battlemaster (63203-63208). A client with imported map/UI is required.
   The addon request is `JoinBattlegroundQueue("ThornGorge")`; a GM teleport to
   an uninstanced map does not create an active match.
6. Use level 31-60 characters, one per faction in the same bracket for initial
   testing (31-40, 41-50, 51-60). Native bot queue fill can participate if enabled
   in the test playerbot configuration. Turn `.gm off` to participate; GM mode
   intentionally cannot capture objectives or carry the flag.
7. Inside the match, administrators can run `.bg thorn` for scores, node
   progress/ownership, flag state and elapsed time. `.bg status` remains the
   existing global battleground status command.

The feature defaults to disabled and requires a restart to enable. To disable,
set `Battleground.ThornGorge.Enabled = 0` and restart the test process. Additional
rows can remain while disabled. Building/packaging does not change production.

## First-match checklist

- Queue both factions, accept invites, verify starts and the 60-second countdown.
  Crossing the 35-yard start boundary during countdown returns a player to their
  start; physical gates are not placed in this prototype.
- Capture a neutral banner, contest with equal numbers, then take it over.
  Verify the progress bar, base counts and resource totals.
- Pick up the flag, interrupt a pickup, and have two players try simultaneously.
  Deliver to an owned base; enemy/neutral bases must not award points.
- Die, disconnect, cancel the flag aura and leave while carrying it. Verify one
  dropped flag, another player's pickup, and the 10-second automatic return.
- Release at an owned graveyard, allow its base to be taken, and verify ghost
  relocation and the native spirit-guide resurrection cycle.
- Watch bots leave both starts, reach all objectives and deliver flags.
  Navigation connectivity alone does not prove live movement works.
- Finish a match (1600 points or 30-minute limit), check honor and scoreboard,
  leave and queue a second match to exercise instance cleanup.

## Rules and compatibility

- Maximum 15 per team; SQL minimum is 1 for testing. Raise it to 5 for ordinary
  matches. Balance values below are provisional.
- Nodes use DBC locations 161-164, starts 165-166, graveyards 167-170. Capture
  samples once per second within 30 yards and 12 vertical yards. Progress is
  0-100, initially 50; each net player moves it two points, capped at five net
  players. Ownership is awarded at endpoints and lost crossing 30/70 toward the
  enemy. Mounted players can capture.
- Resources tick every two seconds: 1/2/5/10 for 1/2/3/4 bases. Flag deliveries
  award 75/85/100/500. Scores clamp at 1600. Simultaneous tied final ticks and
  tied time limits finish without a winning faction.
- Pickup uses native spell 59011 and its fixed DBC cast time, independent of
  combat haste. Completion rechecks membership, object identity, distance, LOS,
  life state and native object-use eligibility. Aura 59005 carries the flag;
  native aura/death/leave callbacks clear it.
- Native map-local objects, queues, team raids, honor, resurrection and teardown
  are reused. No persistent objective spawns or character migrations are needed.
  Capture-point GO templates have no timed proximity implementation in this
  core; progression belongs to the battleground's map-owner update. Client
  area-trigger packets cannot advance capture progress or grant deliveries.
- Center/drop uses imported flagstand 2020421 so the generic flagdrop handler
  cannot delete it during timed pickup. Standalone summon spell 59006 is not
  also cast. Banners 2020400-2020402 reflect ownership. Center Z comes from native
  terrain/collision, with a bounds check that cancels setup on invalid terrain.
- Extracted `WorldStateUI.dbc` records 158-160 specify scores 3601/3602, maximum
  3603, bases 3621/3622, progress display 3623, value 3624 and neutral width 3625.
  Client presentation still needs an in-game check.
- Bots use native movement and GameObject::Use; stable GUIDs spread them across
  uncaptured bases, a subset seeks the flag, and carriers select an owned base.
  They do not teleport or bypass flag validation to achieve objectives.
- Honor: 40 per delivery to the team, 100 participation, 200 extra for winners.
  Existing quests, vendors, marks and reputation are not redefined. The native
  generic scoreboard remains; custom flag-capture columns and map ownership
  icons are not yet implemented.

## Verification

- `ThornGorgeRulesTest`: contested capture, takeover, timer boundaries, repeated
  and stale pickup/drop/delivery, end cleanup, ties, caps, batched ticks and
  1,000 deterministic randomized matches. All 47 architecture tests passed.
- `ThornGorgeFlagTest` compiles the actual native flag callbacks into a harness
  and checks range/LOS changes, eligibility, foreign objects, competing pickup,
  stale carriers, recursive aura removal, failed object/aura creation and reset.
- The Release server binary compiled with playerbots enabled. The SQL script
  was executed against uniquely named session-only temporary tables copied from
  the current schema: 1 template, 6 bindings and 2 orientations; duplicate
  application was rejected. No persistent production rows were changed.
- `ThornGorgeAssetProbe <mmaps-directory>` loads real assets with this core's
  Detour. All 27 map 821 tiles loaded, all seven objective/start positions had
  nearby polygons, and all 42 directed routes completed. Center navigation Z
  was approximately 1159.09. This does not establish live client/server behavior.
- A real two-faction match, terrain and flag visuals, spirit-guide behavior,
  and quest/reward design remain acceptance work before production.


## Collecting a test match

Enable `Battleground.ThornGorge.LogLevel = 2` and
`Battleground.ThornGorge.LogIntervalMs = 5000` in the battleground configuration
section. Restart with the logging build, queue and play normally. Match events
and five-second player/node snapshots append to the native `logs/bg.log` when
`BgLogFile="bg.log"`. Filter for `THORN_GORGE` and the instance's `inst=` value
to reconstruct the test. Keep the matching executable/PDB and server/crash logs
for failures. No DB migration is required for logging. See
`doc/TURTLE_DIAGNOSTICS.md` for fields, overhead, limits and removal controls.
Logging regression verification: 48/48 architecture tests passed, including
the actual native flag callbacks and the diagnostic budget checks. Live log
acceptance requires a new match after the new binary is loaded.


## First live-match corrections (2026-09-09)

Match 103 ended Alliance 1600-1179 after 932 seconds of active simulation.
Patch made three flag deliveries. The initial prototype sampled capture once
per second with up to five net players; groups could take a neutral node in
five samples. New defaults are CaptureTickMs=1200 and CaptureMaxAdvantage=2:
25 samples (30 seconds) solo and 13 samples (15.6 seconds) with a net advantage
of two or more. Equal teams cancel. A full enemy takeover requires 50 solo
samples (60 seconds) or 25 grouped samples (30 seconds). These remain test
tuning, not a claim about the original release. Settings are read on Reset;
interval clamps to 1000-10000ms and advantage to 1-5. Stalls grant no retroactive
capture at a player's new location.

Native AreaPOI.dbc records 2749-2760 on map 821 define worldstates 3606-3617.
Each node's three states are neutral, Horde, Alliance; icon IDs are 5, 9, 10.
The colour ordering is independently consistent with the native AV assault
landmarks. Initial and changed states publish exactly one active icon per
node, clearing the other two. No client patch is required for the inspected
assets. Server log confirmation is not proof of client rendering; check both
teams' icons and late entry in the next live test.

Victory quests 42098/42099 use Player::AreaExploredOrEventHappens for an accepted,
incomplete quest on an online participating winner. Dead winners remain
eligible. GMs, spectators, losing/tied teams, unavailable members and already
complete/absent quests receive no new credit. Apply
sql/custom/thorn_gorge_victory_quests.sql to set SpecialFlags bit 2, preventing
completion before the victory event. Existing character progress is untouched.
Players still turn in the quest normally for its imported reward; the BG
does not grant quest items, XP or reputation directly.

Added diagnostic records: layout (actual flag XYZ and capture tuning),
capture_transition (old/new owner/progress, nearby counts, interval),
map_icon_sent (active client worldstate), quest_credit / quest_credit_skipped,
and update_delay (owner updates of at least 2000ms). Node snapshots include
active_icon. Existing event budgets apply. No packet/combat spam or extra
thread is added. Native-fragment tests exercise initial/live icon agreement
and quest filters/repeat protection; rule tests cover capture caps.

Flag position was subsequently supplied by the player using .gps on map 821,
instance 104: X=2174.469482, Y=1569.349243, Z=1160.459473, O=3.306524.
The GPS also reported FloorZ=1160.459351 and GroundZ=1160.445312. Defaults,
deployment configuration and the real-asset navigation probe now use that XY.
The native terrain/collision query still resolves Z and adds the existing
0.1-yard object offset; no fixed-height override or teleport is introduced.
The d7f50f8 binary already supports these config keys, so deploying the config
is sufficient for the next restart without another executable build. Visual
alignment at the supplied point still requires the next in-game check.
The start enclosure still uses the prototype's 35-yard countdown leash; its
physical gate has not been positioned. Screenshot feedback records that gap.

### September 9 Horde match 102 playtest

Build b7e93426 completed with Horde 1600 / Alliance 861 after 1,114,104ms
including countdown. Player screenshots confirm neutral/owned node map icons
and center alignment. Eight deliveries, eleven pickups and 159 player-death
events were recorded. These are match observations, not completeness criteria.

Next build adds carrier coordinates through the native BG positions opcode,
keeps proactive PvP in noncombat defaults across resets, and scales both flag
objects to 2.5 by default (the scale of native WSG templates, with a different
model). Configure Battleground.ThornGorge.FlagScale in the TG section; clamp
1-5, nonfinite fallback 2.5. No pickup radius, cast time, score or reward changes.
Client carrier rendering and visual flag size still need the next playtest.
The native ground-packet continuation correction preserves untraversed turns
and timing. It does not modify navigation, movement speed or collision assets.

Live read-only checks found vmap LOS/height and mmaps enabled. Neither configured
BotCheats nor RndBotCheats contains movespeed (taxi/repair/breath, plus item for
random bots). No bot exceeded 16 yards/sec between consecutive five-second
position samples with unchanged death count and alive at both ends; this cannot
exclude brief bursts or mount/speed changes inside a sample. New bounded log
fields distinguish these cases. Do not call the bridge collision problem fixed.

Existing queue NPC templates 63203-63208 are all bound to TG (BG type 6).
Currently spawned: Fanwyn Wildbrand in Ironforge (63203), Ingwelda Wildbrand in
Stormwind (63204), and Suda Steelweaver in Thunder Bluff (63206). Rubertus,
Fahesa and Esoch exist as templates without spawns. These are imported NPCs,
not new inventions. Existing custom BG bindings are Blood Ring type 4 and
Sunnyglade Valley type 5. This records this server's data, not historical
public Turtle release behavior. Map 821/area 5722 already identifies Thorn
Gorge in the inspected client and server; a zone label does not imply finished
quests, rewards or collision.

Still open: physical start gates and side-bridge ledges. Native templates
2020407/2020408 exist but are not positioned; the user will show the Horde hut
exit next match. The countdown leash remains in force. Screenshots identify
four side-bridge ends that small characters cannot climb. A binary cannot
by itself establish that their client collision geometry is repaired. Do not
invent verified placements, add teleport shortcuts or claim complete geometry
from the existing 42-route navmesh connectivity probe.

### September 9 completed Horde match 101

See [the report-by-report status](THORN_GORGE_PLAYTEST_2026-09-09.md) for the next
batch, verified match timings, reproduced navigation failure and unresolved
client geometry/UI work. This supersedes the earlier gate/flag status above.
