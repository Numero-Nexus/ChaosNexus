// ==========================================
// nexus::core::text — String Utilities (Allocating Algorithms)
// ==========================================
//
// Phase 8F: String Utilities.
//
// Implements the algorithms from strings.hpp that necessarily allocate
// (split/split_any/join/replace_all/replace_first/to_lower_ascii/
// to_upper_ascii) and therefore cannot live as constexpr definitions
// in the header.

#include "nexus/core/text/strings.hpp"

namespace nexus::core::text {

// ------------------------------------------
// Split / Join
// ------------------------------------------

auto split(StringView view, StringView delimiter) -> std::vector<StringView>
{
    std::vector<StringView> parts;

    if (delimiter.empty()) {
        parts.push_back(view);
        return parts;
    }

    types::Size start = 0;
    while (true) {
        const types::Size found = view.find(delimiter, start);
        if (found == StringView::npos) {
            parts.push_back(view.substr(start));
            break;
        }

        parts.push_back(view.substr(start, found - start));
        start = found + delimiter.size();
    }

    return parts;
}

auto split_any(StringView view, StringView delimiters) -> std::vector<StringView>
{
    std::vector<StringView> parts;

    if (delimiters.empty()) {
        parts.push_back(view);
        return parts;
    }

    types::Size start = 0;
    while (true) {
        const types::Size found = view.find_first_of(delimiters, start);
        if (found == StringView::npos) {
            parts.push_back(view.substr(start));
            break;
        }

        parts.push_back(view.substr(start, found - start));
        start = found + 1;
    }

    return parts;
}

auto join(std::span<const StringView> parts, StringView delimiter) -> String
{
    if (parts.empty()) {
        return String{};
    }

    types::Size total_size = delimiter.size() * (parts.size() - 1);
    for (const StringView& part : parts) {
        total_size += part.size();
    }

    String result;
    result.reserve(total_size);

    result.append(parts.front());
    for (types::Size index = 1; index < parts.size(); ++index) {
        result.append(delimiter);
        result.append(parts[index]);
    }

    return result;
}

// ------------------------------------------
// Replace
// ------------------------------------------

auto replace_all(StringView view, StringView target, StringView replacement) -> String
{
    if (target.empty()) {
        return String{view};
    }

    String result;
    result.reserve(view.size());

    types::Size start = 0;
    while (true) {
        const types::Size found = view.find(target, start);
        if (found == StringView::npos) {
            result.append(view.substr(start));
            break;
        }

        result.append(view.substr(start, found - start));
        result.append(replacement);
        start = found + target.size();
    }

    return result;
}

auto replace_first(StringView view, StringView target, StringView replacement) -> String
{
    if (target.empty()) {
        return String{view};
    }

    const types::Size found = view.find(target);
    if (found == StringView::npos) {
        return String{view};
    }

    String result;
    result.reserve(view.size() - target.size() + replacement.size());
    result.append(view.substr(0, found));
    result.append(replacement);
    result.append(view.substr(found + target.size()));
    return result;
}

// ------------------------------------------
// ASCII case conversion
// ------------------------------------------

auto to_lower_ascii(StringView view) -> String
{
    String result{view};
    std::ranges::transform(result, result.begin(), [](char character) noexcept {
        return (character >= 'A' && character <= 'Z') ? static_cast<char>(character - 'A' + 'a')
                                                      : character;
    });
    return result;
}

auto to_upper_ascii(StringView view) -> String
{
    String result{view};
    std::ranges::transform(result, result.begin(), [](char character) noexcept {
        return (character >= 'a' && character <= 'z') ? static_cast<char>(character - 'a' + 'A')
                                                      : character;
    });
    return result;
}

} // namespace nexus::core::text
