// SPDX-License-Identifier: GPL-2.0-or-later
#ifndef MANGOS_TRADE_MONEY_H
#define MANGOS_TRADE_MONEY_H

#include <cstdint>

// Evaluate the final balance, including the gold offered in the same exchange.
inline bool CanApplyTradeMoney(std::uint32_t balance, std::uint32_t offered,
    std::uint32_t received, std::uint32_t limit)
{
    return offered <= balance && std::uint64_t(balance) - offered + received <= limit;
}

#endif
