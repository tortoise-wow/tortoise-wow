# Thorn Gorge playtest follow-up, September 9, 2026

Implemented changes below still require a coordinated client/server restart and live acceptance. Compilation and offline asset checks are not a complete battleground playtest.

## Completed match evidence

Build 97fb6d6c, instance 101: Horde 1600 / Alliance 1271, 1,287,604ms active simulation, 30 participants, 13 deliveries, 16 pickups, three drops and 148 deaths. Twelve completed delivery resets took 10,004â€“10,134ms. The final delivery ended the match. Three Horde bots initially stalled on one-point paths. Elevated unmounted bot speeds were observed; the human tester's explicit GM speed override is excluded from the defect evidence.

## Report-by-report implementation

| Report | Change and remaining live check |
| --- | --- |
| Capture speed | Existing prototype tuning retained: solo neutral capture about 30s, net two-player advantage about 15.6s. Historical rules are not established. |
| Center position and base icons | Previously corrected user GPS and native neutral/faction base icons retained; user confirmed both. |
| Inconsistent flag size | Center and dropped objects both scale 2.5 before initial visibility and collision publication. Verify first spawn, drop and repeated reset. |
| Timer and sounds | Ten-second delivery reset and 30-second dropped return retained. Explicit chat countdown, native WSG capture/reset sounds and companion HUD using the actual server timer. |
| Horde and Alliance gates | Native collision doors, closed during preparation and opened at start. Horde doorway uses supplied GPS; Alliance placement comes from the tunnel model opening. Countdown containment is retained. Both visual fit and open passage need acceptance. |
| Carrier chases kills | TG-only delivery priority above proactive attacks, below critical survival; native casts and eligibility preserved. |
| Objective allocation | Stable runner, escort/interceptor, defender and distributed capture assignments. Nearby fighting allowed; excessive pursuit yields to objective travel. Enemy carriers recognized through native TG membership and carry aura. Other BG strategies unchanged. |
| Ignoring humans | Earlier noncombat PvP reset fix retained. Human targeting and objective combat need another live run. God mode is not treated as GM invisibility. |
| Floating and mountain shortcuts | Earlier ground-spline continuation repair retained. Map821 now opts into native steep-slope exclusion with no blind/forced shortcut through an excluded route. Native query budget increased only for821. Tight-corner oscillation repaired in the native smoother. |
| Bots stuck in hut | Unchanged one-point paths now report failure. TG-only bounded native ballistic jumps can traverse small obstacles when walking fails; collision, landing and subsequent walking progress must pass. This does not guarantee every spawn exit is solved until tested live. |
| Unmounted high speed | Socketless controlled players apply native speed transitions immediately; they no longer queue impossible client ACKs that can restore stale mounted speed after dismount. Real-client ACK behavior and aura calculations preserved. |
| Four bridge step-ups | Client patch adds eight existing wooden ramps over the raised beams; matching native vmaps/mmaps rebuilt. No shared model change or teleport workaround. Test a gnome in both directions at all four ends. |
| Horde flag icon blue | Companion receives carrier team and selects faction texture on world map and battlefield minimap. Native coordinates retained. |
| Player icons missing at start | Expanded map821 WorldMapArea bounds include both spawns; companion refits/crops map art to those bounds. Other DBC rows unchanged. Native player positions are not clamped or spoofed. |
| Queue NPCs and zone | Existing spawned battlemaster NPCs: Fanwyn Wildbrand in Ironforge, Ingwelda Wildbrand in Stormwind, Suda Steelweaver in Thunder Bluff. Imported unspawned templates remain unspawned. Quests42098/42099 corrected from Moonwhisper Coast5642 to Thorn Gorge5722 only. |
| Rewards | Existing 40 honor per delivery, 100 completion honor plus200 winner honor retained. Victory quests retain XP5900, max-level money35400 and150 Ironforge/Thunder Bluff reputation. No invented marks, dedicated faction, vendor ladder or reward sets: imported evidence does not establish them. |
| Diagnostics | Bounded level2 human/bot floor, flags, speed/aura and spline samples; bounded traversal acceptance/rejection records. Controls and overhead in TURTLE_DIAGNOSTICS.md. |

## Validation and delivery

65 architecture tests pass, covering native speed ACK variants, filter exclusion and map scoping, no-progress paths, bounded native traversal, objective allocation/priorities, object publication, timers/sounds and diagnostic admission. Companion Lua tests cover stale/forged messages, faction textures, countdown, map/minimap crop/resize and restoration on other maps. Release compilation completes.

Offline extraction rebuilt map821 from the same four patched ADTs shipped to clients. Eight ramp transforms match assembled collision, slopes approximately4â€“27 degrees; repeat builders reproduce identical bytes. The native asset probe at16384 nodes produces42 usable paths, zero query exhaustion and zero smoothing failures. With steep exclusion,20 central-objective routes complete as walking paths;22 routes involving spawns remain partial and require native traversal. Usable partial paths are not proof that bots arrive.

Deployment selects a separate complete data directory, leaving the running server's original geometry intact until restart. Ship exactly five client MPQ files: four ADTs and WorldMapArea.dbc. Never ship the extraction-only reduced Map.dbc. The client addon and MPQ are both necessary; fully restart the client as well as the world server.

Live acceptance: both gates/countdown exits, gnome bridge passage, bot spawn departure and terrain routes, human combat, flag delivery under pressure, carrier colour, first/drop/respawn flag scale, timers/sounds, player icons at both starts and queue NPCs. Review fresh movement/traversal logs before declaring these verified.


## Follow-up: e33ddcd3 match 102 and generic movement repair

This section supersedes the earlier one-minute preparation, ten-second delivery
reset and map-specific movement restrictions. The user explicitly authorized
shared bot fixes across PvP, instances, questing/grinding and open-world travel.
The sole development branch remains `feature/thorn-gorge-prototype`.

The completed log segment is September 9, server clock13:36:43–13:58:03,
instance102 (not the older same-numbered instance). Native start was13:37:44:
61seconds of preparation. Result: Horde1600/Alliance1021;1,198,826ms active
simulation,26join events,45deaths,12pickups,11deliveries and10resets. The last
delivery ended the match.6571movement samples include6308bot samples:984mounted
and5324unmounted. These are samples, not a percentage of bots refusing mounts.
81moving bot samples have a reported floor gap above4yards. Some gaps may be
WMO/overhang support-query differences; a gap alone does not prove invalid travel.

Concrete invalid handoff evidence includes Penno/guid2000012410 at13:44:40:
a native POINT move with a straight3-vertex spline from approximately
(2315.1,1506.65,1230.55) to(2286.36,1402.54,1197.3), and a sampled floor gap55.143.
Unti/guid2000011802 at13:39:09 shows the same shape with54.52yards of floor gap.
No bot_traversal events were emitted. The failed route was being converted to a
single destination; the subsequent native point request copied NOPATH shortcut
coordinates into the spline. Existing traversal recovery therefore never ran.
Startup confirms `data-thorn-20260909-r1` vmaps/mmaps and enabled pathfinding.
This is the rebuilt map821 data, not another map substituted for Thorn.

Changes:
- A required walking route remains empty on failure on every map. Explicit
  direct movement retains its separate flight/swim/script contract. Existing
  cross-map travel nodes, portals, transport handling, lifecycle and retry cache
  remain in place.
- Native spline handoff rejects NOPATH/invalid status for all callers. Player
  ground requests additionally reject unverified shortcut coordinates; valid
  native flight, underwater paths, partial walking prefixes and explicit direct
  movement retain their handling. No teleport or forced destination masks failure.
- `mmap.PlayerWalkable=1` applies slope exclusion to ground players on all maps.
  Flight and explicit ignore-pathfinding states retain native capabilities;
  ordinary creatures retain their own terrain/swim filter. QueryNodes821 remains
  a per-map capacity setting because its measured mesh needs more query nodes.
- The existing bounded ballistic traversal is now shared `TryGroundTraversal`,
  called only after required walking fails and the AI permits detailed movement.
  It preserves preparation, death, casting, roots, transport, flight, swimming,
  falling and overlapping-jump guards. Maximum16candidates perbot per5seconds;
  native collision, walkable landing and subsequent route progress must pass.
  No unvalidated direct step is substituted if all candidates fail.
- Mount eligibility no longer treats a missing current target as an enemy at
  zero distance. Real close-enemy/combat, indoor, water, flag-carrying and other
  native mount restrictions remain. This change is shared across destinations.
- Horde gate visual pivot is recentered using the imported model bounds;
  measured doorway containment plane remains unchanged.
- Preparation now uses the native two-minute BG schedule. Delivery reset now
  matches WSG's23seconds; dropped return remains30seconds. Existing timer/sounds
  are retained and the user confirmed they worked.
- Companion map margins cover exposed backing art without falsifying player
  coordinates. Cosmetic labels identify Thorn Gorge/Battleground where native
  continent lists have no entry. Art still occupies its real geographic extent;
  this does not invent missing terrain art around the spawn. Other-map labels,
  texture coordinates, tile geometry and native markers are restored/preserved.

Validation:67native-fragment architecture tests plus the production Lua fixture.
Coverage includes failure/partial/direct spline handoffs; player ground filters
on maps0,1,30,33,409,489,529,821; creature filters, swimming, flight and explicit
ignore states; bounded jump eligibility/progress; mount target absence/proximity;
flag timer boundaries and repeated map changes. Map identifiers exercise generic
code with fixtures: they do not constitute live dungeon/raid/continent playtests.
The new native-handoff regression fails against the old MoveSplineInit::Move
fragment and passes when the fixed fragment is restored. Full binary build,
packaging and deployment receipts are delivered separately.

Acceptance still required after restart: repeat the reported air routes and Mage
Tower approach, observe native jumps at the hut, mounting during clear travel,
Horde gate fit, both teams' preparation, map borders/labels,23second flag resets,
and normal combat/instance travel. A refused route can expose a remaining navmesh
connectivity gap rather than hide it. Keep movement logging enabled for the next
run. Human Yes was still using a GM5x speed override (35vsnormal7) in this log;
use `.modify speed 1` for normal-speed comparisons. GM kill/invisibility actions
also make this match unsuitable as a normal PvP balance benchmark.


## Follow-up: match104 gate, mounting and pickup presentation

Captured the complete17:20–17:28 server-clock instance104 from the latest bg.log.
Result Alliance1600/Horde119,404051ms active,30join/leave events,20deaths,
5pickups,4deliveries/returns and7rejected pickup completions. There were2870
movement samples:407mounted and2463unmounted, including the human. The bounded
AI history contains50successful mount-action records,119failed and24useless
records; histories overlap and these are not unique attempts. Mounting was not
entirely absent.17:20:20 countdown was60seconds, and screenshots show10second
returns. The server still used the pre-2aa82994 behavior; deployment replaces
files for next launch and does not restart a running world process.

Independent corrections made from these reports:
- Alliance gate base previously used tunnel localZ+1.08. Ray/triangle intersection
  at local(.075,4.5) finds floor-1.080931 and ceiling7.329544. Correct world base
  to1267.775758 and scale native portcullis to2.25: height3.75785184*2.25 covers
  the8.410475-yard opening, with extra width hidden behind the stone sides.
  Native GO_STATE_READY creation and DoorClose/DoorOpen remain unchanged.
- Long ground travel now consults the existing native mount action before
  dispatching movement, regardless of destination/map. High objective priority
  no longer starves idle mount maintenance. Reuse Engine::ExecuteAction through
  DoSpecificAction, retaining usefulness, possibility and action listeners.
  Propagate the mount action's actual cast duration to the outer travel action.
  Exclude short(<=40yard), idle/reaction/direct requests, combat, existing mounts,
  flight/fall/jump, transport/taxi and casts. Native eligibility retains outdoors,
  mount list, flag, class-form, map and threat restrictions. A refusal continues
  travel; it does not force a mount or block movement. This shared mechanism
  applies to qualifying quest, grinding, instance and PvP travel requests.
- Noncarrier Thorn objective travel now yields to a nearby native enemy target
  around its assignment even before combat begins, with line of sight. Requiring
  an existing combat victim created a circular dependency: objective priority
  prevented the attack that would have allowed combat to outrank the objective.
  Carrier delivery and the45yard assignment leash remain. Bots may legitimately
  pass enemies while traveling to a different assignment; no arbitrary fight-on-
  sight probability or global PvP target rewrite was added.
- Client spell59011 has zero SpellVisual. Copy the existing Opening21651 visual
  6139 into that single field of the full1.12 Spell.dbc. Reproducible patch script
  checks record layout, localized name anchors and byte-for-byte preservation
  outside this4byte field. Native pickup cast/interrupt/completion logic stays
  unchanged. This is the native opening animation, not a class crafting spell.

Validation:68architecture tests pass, including new shared travel mount guards,
failed native eligibility and1.5/3second cast-duration propagation, plus enemy
acquisition/LOS/assignment-leash cases. Client MPQ roundtrip extracts the exact
patched Spell.dbc; all five earlier geometry/map members are retained unchanged.
Client and world restart are required for acceptance. Visual gate fit and animation,
mounting on clear paths, and mixed-team objective encounters still need live checks.
