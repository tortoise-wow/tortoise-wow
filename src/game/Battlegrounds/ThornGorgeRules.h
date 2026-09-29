#ifndef TURTLE_THORN_GORGE_RULES_H
#define TURTLE_THORN_GORGE_RULES_H

#include <algorithm>
#include <array>
#include <cstdint>

// Match-local rules. The battleground owner supplies eligible player counts;
// this object owns no players, maps, timers from other threads, or persistence.
namespace ThornGorge
{
    // Announce only crossed final-five-second boundaries. A delayed update
    // emits the current remaining time once, never a burst of stale numbers.
    inline unsigned FlagCountdownSeconds(unsigned before, unsigned after)
    {
        unsigned const remaining = (after + 999) / 1000;
        return after && after < before && remaining <= 5 &&
            (before + 999) / 1000 != remaining ? remaining : 0;
    }

constexpr unsigned NodeCount = 4;
constexpr unsigned MaxScore = 1600;
constexpr unsigned CaptureRadius = 30;
// Recovery must allow travel plus the native ten-second pickup cast.
constexpr unsigned DroppedFlagReturnMs = 30000;
// Match this core's WSG post-capture reset; dropped recovery is separate.
constexpr unsigned FlagRespawnMs = 23000;
enum Team : unsigned { Alliance, Horde, Neutral };
enum Flag : unsigned { Center, Carried, Dropped, Respawning };

// Extracted Turtle AreaPOI.dbc map 821: neutral icon 5, Horde 9, Alliance 10.
// AV assault landmarks independently establish the icon colours (9 red, 10 blue).
constexpr unsigned NodeIconState(unsigned node, Team owner)
{ return 3606 + node * 3 + (owner == Alliance ? 2 : owner == Horde ? 1 : 0); }

struct Rules
{
    std::array<int, NodeCount> progress{{50, 50, 50, 50}};
    std::array<Team, NodeCount> owner{{Neutral, Neutral, Neutral, Neutral}};
    std::array<unsigned, 2> score{{0, 0}};
    Flag flag = Center;
    std::uint64_t carrier = 0;
    unsigned flagTimer = 0;
    unsigned resourceTimer = 0;
    bool ended = false;
    unsigned maxCaptureAdvantage = 2;
    void ConfigureCapture(int advantage) { maxCaptureAdvantage = unsigned(std::clamp(advantage, 1, 5)); }

    unsigned Bases(Team team) const
    {
        return unsigned(std::count(owner.begin(), owner.end(), team));
    }
    void Capture(unsigned node, unsigned allies, unsigned horde)
    {
        if (ended || node >= NodeCount) return;
        int delta = std::clamp(int(allies) - int(horde), -int(maxCaptureAdvantage), int(maxCaptureAdvantage));
        progress[node] = std::max(0, std::min(100, progress[node] + 2 * delta));
        if (progress[node] == 100) owner[node] = Alliance;
        else if (progress[node] == 0) owner[node] = Horde;
        else if ((owner[node] == Alliance && progress[node] < 70) ||
                 (owner[node] == Horde && progress[node] > 30)) owner[node] = Neutral;
    }
    void AddPoints(Team team, unsigned points)
    {
        if (ended || team == Neutral) return;
        score[team] += std::min(points, MaxScore - score[team]);
    }
    Team Leader() const
    {
        return score[0] == score[1] ? Neutral : score[0] > score[1] ? Alliance : Horde;
    }
    bool HasWinner() const { return score[0] >= MaxScore || score[1] >= MaxScore; }
    void Tick(unsigned milliseconds)
    {
        if (ended) return;
        if (flag == Dropped || flag == Respawning)
        {
            if (milliseconds >= flagTimer) { flag = Center; flagTimer = 0; }
            else flagTimer -= milliseconds;
        }
        resourceTimer += milliseconds;
        constexpr unsigned points[5] = {0, 1, 2, 5, 10};
        while (resourceTimer >= 2000 && !HasWinner())
        {
            resourceTimer -= 2000;
            // Both teams earn the same tick before victory is resolved.
            AddPoints(Alliance, points[Bases(Alliance)]);
            AddPoints(Horde, points[Bases(Horde)]);
        }
    }
    bool PickUp(std::uint64_t guid)
    {
        if (ended || !guid || (flag != Center && flag != Dropped)) return false;
        flag = Carried; carrier = guid; flagTimer = 0;
        return true;
    }
    bool Drop(std::uint64_t guid)
    {
        if (flag != Carried || carrier != guid) return false;
        carrier = 0; flag = Dropped; flagTimer = DroppedFlagReturnMs;
        return true;
    }
    bool Deliver(std::uint64_t guid, Team team, unsigned node)
    {
        if (ended || team == Neutral || node >= NodeCount || owner[node] != team ||
            flag != Carried || carrier != guid) return false;
        constexpr unsigned points[5] = {0, 75, 85, 100, 500};
        AddPoints(team, points[Bases(team)]);
        carrier = 0; flag = Respawning; flagTimer = FlagRespawnMs;
        return true;
    }
    void Finish() { ended = true; carrier = 0; flag = Respawning; flagTimer = 0; }
};
}
#endif
