// Implementation of UUID parsing and formatting (Phase 8E).

#include "nexus/core/identity/uuid_format.hpp"

#include <array>
#include <charconv>
#include <cstdio>

namespace nexus::core {

namespace {

constexpr std::array<types::Size, 5> GROUP_BYTE_LENGTHS{4, 2, 2, 2,
                                                        6}; // NOLINT(readability-identifier-naming)

[[nodiscard]] auto hex_digit(char c) noexcept -> int
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

} // namespace

auto to_string(const Uuid& id) -> std::string
{
    static constexpr std::array<char, 16> hex_digits{'0', '1', '2', '3', '4', '5', '6', '7',
                                                     '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    const auto& bytes = id.bytes();

    std::string out;
    out.reserve(36);

    types::Size byte_index = 0;
    for (types::Size group = 0; group < GROUP_BYTE_LENGTHS.size(); ++group) {
        if (group != 0) {
            out.push_back('-');
        }
        for (types::Size i = 0; i < GROUP_BYTE_LENGTHS.at(group); ++i, ++byte_index) {
            const types::U8 byte = bytes.at(byte_index);
            out.push_back(hex_digits.at((byte >> 4) & 0x0F));
            out.push_back(hex_digits.at(byte & 0x0F));
        }
    }

    return out;
}

auto uuid_from_string(std::string_view text) -> Result<Uuid>
{
    constexpr auto parse_failure = [](std::string_view reason) -> Result<Uuid> {
        return Result<Uuid>{Error{ErrorCode::ConfigParseFailed, std::string{reason}}};
    };

    if (text.size() != 36) {
        return parse_failure("UUID string must be exactly 36 characters");
    }
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    if (text[8] != '-' || text[13] != '-' || text[18] != '-' || text[23] != '-')
    // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    {
        return parse_failure("UUID string must be in 8-4-4-4-12 hyphenated form");
    }

    Uuid::ByteArray bytes{};
    types::Size text_index = 0;
    types::Size byte_index = 0;

    for (types::Size group = 0; group < GROUP_BYTE_LENGTHS.size(); ++group) {
        if (group != 0) {
            ++text_index; // skip hyphen
        }
        for (types::Size i = 0; i < GROUP_BYTE_LENGTHS.at(group); ++i, ++byte_index) {
            // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
            const int high = hex_digit(text[text_index]);
            const int low = hex_digit(text[text_index + 1]);
            // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
            if (high < 0 || low < 0) {
                return parse_failure("UUID string contains a non-hexadecimal character");
            }
            bytes.at(byte_index) = static_cast<types::U8>((high << 4) | low);
            text_index += 2;
        }
    }

    return Result<Uuid>{Uuid{bytes}};
}

} // namespace nexus::core
