# Atomic actor scope tests

`run-atomic-scope-tests.ps1` compiles the actual `src/game/AtomicPlayerSave.cpp`
unchanged, using MSVC `include_alias` for Player/Mail/Database/World/Log boundary
fixtures. It uses the real AtomicPlayerSave.h class declaration. There is no copy
of the scope algorithm, SQL server, game process, credentials or game assets.

102 checks cover nested singleton A inside pair A/B, full-A/inventory-B selection
once at outer seal, no cleanup/mail/actions before ACK, and TLS/mail-collector/SQL
ownership being off before completion/publication/callbacks. They also cover
prepare and failed/ambiguous ACK, busy admission without fault, abandoned owner,
participant/Player snapshot identity mismatch, late mail during sealing, immediate
outside-scope callbacks, and post-ACK callback opening a new owned scope.

The Player serializers, DB scheduler and PreparedMail publication implementations
remain independently tested by their production-body/real-source suites; these
fixtures only verify the actual root scope's orchestration and failure ordering.
Real InnoDB and native gameplay acceptance remain parent-owned.

`run-far-teleport-scope-tests.ps1` extracts the exact `ScheduleFarTeleport` body
from MapManager.cpp and compiles it with the real AtomicPlayerSave.cpp. It checks
all four combinations of map updating/owned scope: ordinary transfers stay
immediate, while an admitted reward retains the existing pending queue and its
data until ACK. The fixture drains the queue only after the scope is detached.
It does not reproduce map removal, pet persistence or client transfer handshakes;
those lifecycle operations are precisely what must not run inside the owner.

`run-mail-inventory-attachment-test.ps1` is a focused regression using the actual
PreparedMail.inc through the existing real-Mail.h boundary fixture. A clean,
unbagged item still registered in a Player inventory has no world/container/dirty
queue flags. Before the final guard it was incorrectly adopted and deleted after
failed ACK while the inventory retained its pointer. After the guard, it is
refused before SQL/adoption and its actor owner keeps it. The existing prepared
mail suite additionally checks all three world/container/update-queue flags.

These runners write only to their chosen isolated evidence directories. They do
not edit source implementations, build the server, connect to DB or start services.
