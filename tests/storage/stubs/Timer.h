#pragma once
struct WorldTimer {
    static unsigned getMSTime() { return 0; }
    static unsigned getMSTimeDiff(unsigned, unsigned) { return 0; }
    static unsigned getMSTimeDiffToNow(unsigned) { return 0; }
};
