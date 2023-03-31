#include "requests.h"

#include <bitset>

namespace {
BreakReason getBreakReason(unsigned char ch)
{
    switch (ch) {
    case 'E':
        return BreakReason::Erroneous;
    case 'C':
        return BreakReason::Consent;
    case 'S':
        return BreakReason::Supervisory;
    case 'X':
        return BreakReason::External;
    default:
        return BreakReason::Unknown;
    }
}

LiquidityIndicator getLiquidityIndicator(const std::bitset<8> & bit_vector)
{
    if (!bit_vector[0] && !bit_vector[1]) {
        return LiquidityIndicator::Added;
    }
    if (bit_vector[0] && !bit_vector[1]) {
        return LiquidityIndicator::Removed;
    }
    return LiquidityIndicator::None;
}
} // namespace

ExecutionDetails decode_executed_order(const std::vector<unsigned char> & message)
{
    ExecutionDetails exec_details;
    size_t offset = 9;

    exec_details.cl_ord_id = decode_text(message, 14, offset);
    exec_details.filled_volume = decode_binary4(message, offset);
    exec_details.price = decode_price(message, offset);
    offset++;
    exec_details.match_number = decode_binary4(message, offset);
    exec_details.counterpart = decode_text(message, 4, offset);
    decode_mmt(message, exec_details.mmt, offset);

    std::bitset<8> bit_vector(message[offset]);
    exec_details.internalized = bit_vector[5];
    exec_details.self_trade = bit_vector[7];
    bit_vector >>= 3;
    bit_vector &= 3;
    exec_details.liquidity_indicator = getLiquidityIndicator(bit_vector);

    return exec_details;
}

BrokenTradeDetails decode_broken_trade(const std::vector<unsigned char> & message)
{
    BrokenTradeDetails break_details;
    size_t offset = 9;
    break_details.cl_ord_id = decode_text(message, 14, offset);
    break_details.match_number = decode_binary4(message, offset);
    break_details.reason = getBreakReason(message[offset++]);
    decode_mmt(message, break_details.mmt, offset);
    return break_details;
}