# Honor save append/ACK retention

Run `run-honor-save-tests.ps1` with MSVC x64 loaded. The executable compiles the
actual production `HonorSave.inc` and actual HonorMgr.h, with small player/DB/week
input boundaries. It uses no SQL server, credentials, game process or assets.

`HonorMgr::SaveBatch`, `AppendSaveToDB` and `CompleteSaveToDB` are the checked API
used by the full owned player snapshot. Append copies all ordered CP entries/states
and persisted currency/rank fields. Only NEW CP entries produce INSERTs; identical
credits remain separate rows. Currency REPLACE and stored character UPDATE join
the same owned SQL transaction. No CP entry/state is cleared during preparation.

Completion is actor-side after acknowledged commit and DB ownership deactivation.
It validates the whole ordered list and every persisted scalar before marking any
captured NEW credit unchanged. Changed/additional entries or currency/rank changes
reject completion without consuming even the first dirty entry. Failed append or
abandoned/failed-ACK batch retains all NEW CP states. Completion issues no SQL.

Ordinary Save outside owned mode preserves its historical optimistic consumption
and unsupported-state dropping behavior. It remains part of the A06 limitation;
legacy Save called inside owned mode fails closed instead of clearing pending CP.
Currency/stored-row public wrappers retain their ordinary behavior. The PVP credit
packet is routed through Player's ACK reward-packet helper (friend HonorMgr), while
Player's generic addon routing covers TW_HONOR balance notification.

61 checks cover duplicate credit rows, empty/no-new snapshots, every statement
append failure, failed/abandoned ACK, pre-ACK completion rejection, actor owner
identity, stale list/values/currency/rank, week normalization, registered/faulted
admission, one-shot batches, invalid states, and ordinary compatibility. The week
reset method and all save/consumption methods are production bodies; fixture data
loading is boundary-only. Real normal-header HonorMgr.cpp compilation separately
checks integration; full server/native InnoDB acceptance remains parent-owned.
