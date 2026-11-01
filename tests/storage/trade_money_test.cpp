// SPDX-License-Identifier: GPL-2.0-or-later
#include "../../src/game/Handlers/TradeMoney.h"
#include <cassert>
#include <iostream>
#include <limits>

int main()
{
    constexpr std::uint32_t cap = 0x7FFFFFFE;
    assert(!CanApplyTradeMoney(cap - 5, 0, 10, cap)); // Previously destroyed five copper.
    assert(CanApplyTradeMoney(cap - 5, 0, 5, cap));
    assert(CanApplyTradeMoney(cap, 0, 0, cap));
    assert(CanApplyTradeMoney(cap, 10, 10, cap)); // Equal two-way exchange is valid at the cap.
    assert(CanApplyTradeMoney(cap, 10, 5, cap));
    assert(!CanApplyTradeMoney(cap, 5, 10, cap));
    assert(CanApplyTradeMoney(10, 10, 0, cap));
    assert(!CanApplyTradeMoney(0, 1, 0, cap));
    assert(!CanApplyTradeMoney(cap, 0, cap, cap));
    constexpr auto wide = std::numeric_limits<std::uint32_t>::max();
    assert(!CanApplyTradeMoney(wide, 0, wide, wide)); // The sum must not wrap to 32 bits.
    assert(CanApplyTradeMoney(wide, wide, wide, wide));
    std::cout << "PASS 11 production trade-money boundary cases\n";
}
