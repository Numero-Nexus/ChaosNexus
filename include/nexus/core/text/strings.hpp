// ==========================================
// nexus::core::text — String Utilities (Public API)
// ==========================================
//
// Phase 8F: String Utilities.
//
// Lightweight, allocation-conscious string manipulation primitives used
// throughout ChaosNexus. Algorithms that can be constexpr/noexcept and
// don't allocate are defined inline here. Algorithms that necessarily
// allocate (split/split_any/join/replace_*/to_lower_ascii/to_upper_ascii)
// are declared here and implemented in strings.cpp.
//
// Scope (this phase): UTF-8 *byte-sequence* aware utilities — trimming,
// splitting, joining, replacing, prefix/suffix queries, ASCII case
// conversion, numeric <-> string conversion, hex formatting/parsing, and
// an incremental StringBuilder. This header does NOT provide Unicode
// normalization, locale-aware collation, or text encoding conversion;
// those are explicitly out of scope for this phase (see docs).
//
// Layering: this header is part of the Core Utilities layer. It depends
// only on nexus/core/types and the standard library. It must never
// depend on the Simulation Engine, SDK, CLI, or Plugins.

#pragma once

#include "nexus/core/types/types.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <charconv>
#include <concepts>
#include <expected>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace nexus::core::text {

// ------------------------------------------
// String / StringView aliases
// ------------------------------------------
//
// ChaosNexus strings are treated as UTF-8 encoded byte sequences.
// These aliases exist to give that intent a single, explicit,
// project-wide spelling rather than leaving each subsystem to write
// std::string / std::string_view directly. No implicit encoding
// conversion or validation is performed by the alias itself.

/// An owning, UTF-8 encoded byte sequence.
using String = std::string;

/// A non-owning view over a UTF-8 encoded byte sequence. Callers are
/// responsible for ensuring the viewed data outlives the view, as with
/// any std::string_view.
using StringView = std::string_view;

// ------------------------------------------
// Error type for fallible conversions
// ------------------------------------------

/// Reasons a numeric or hex conversion may fail. Deliberately narrow;
/// this phase does not attempt to surface std::from_chars/std::to_chars
/// error_code values verbatim, since callers only need to distinguish
/// these cases to act correctly.
enum class ConversionError : types::U8 {
    /// The input was syntactically invalid for the requested type.
    InvalidArgument,

    /// The input was well-formed but its value does not fit in the
    /// requested type (overflow/underflow).
    OutOfRange,

    /// The input contained trailing characters after a valid value
    /// that were not consumed.
    TrailingCharacters,
};

/// The result type returned by fallible string <-> value conversions
/// in this subsystem.
template<typename T> using ConversionResult = std::expected<T, ConversionError>;

// ------------------------------------------
// Trim
// ------------------------------------------

namespace detail {

/// ASCII-only whitespace predicate. Deliberately does not use
/// std::isspace directly: std::isspace takes an int and has undefined
/// behavior for negative values other than EOF when char is signed
/// (e.g. bytes >= 0x80 in a UTF-8 continuation byte), so callers must
/// funnel through an unsigned char first. Restricting to the classic
/// "C" locale whitespace set (space, tab, LF, VT, FF, CR) keeps this
/// constexpr-friendly and locale-independent.
[[nodiscard]] constexpr auto is_ascii_space(char character) noexcept -> bool
{
    switch (static_cast<unsigned char>(character)) {
        case ' ':
        case '\t':
        case '\n':
        case '\v':
        case '\f':
        case '\r':
            return true;
        default:
            return false;
    }
}

} // namespace detail

/// Returns `view` with leading ASCII whitespace removed. Whitespace is
/// defined as the classic "C" locale whitespace set (space, tab,
/// newline, CR, FF, VT); no locale-dependent behavior is used. Does
/// not allocate.
[[nodiscard]] constexpr auto trim_left(StringView view) noexcept -> StringView
{
    types::Size begin = 0;
    while (begin < view.size() && detail::is_ascii_space(view[begin])) {
        ++begin;
    }
    return view.substr(begin);
}

/// Returns `view` with trailing ASCII whitespace removed. See trim_left
/// for the whitespace definition. Does not allocate.
[[nodiscard]] constexpr auto trim_right(StringView view) noexcept -> StringView
{
    types::Size end = view.size();
    while (end > 0 && detail::is_ascii_space(view[end - 1])) {
        --end;
    }
    return view.substr(0, end);
}

/// Returns `view` with leading and trailing ASCII whitespace removed.
/// Does not allocate.
[[nodiscard]] constexpr auto trim(StringView view) noexcept -> StringView
{
    return trim_right(trim_left(view));
}

// ------------------------------------------
// Split / Join
// ------------------------------------------

/// Splits `view` on every occurrence of `delimiter`, returning the
/// resulting substrings in order. An empty `view` yields a single
/// empty element. Consecutive delimiters yield empty elements between
/// them (i.e. this is not a "skip empty" split). An empty `delimiter`
/// yields `view` as a single element. The returned views alias `view`
/// and must not outlive it.
[[nodiscard]] auto split(StringView view, StringView delimiter) -> std::vector<StringView>;

/// Splits `view` on any single occurrence of any character in
/// `delimiters`, returning the resulting substrings in order. Follows
/// the same empty-element semantics as split. The returned views alias
/// `view` and must not outlive it.
[[nodiscard]] auto split_any(StringView view, StringView delimiters) -> std::vector<StringView>;

/// Joins `parts` into a single owned String, inserting `delimiter`
/// between consecutive elements. Returns an empty String if `parts` is
/// empty. Allocates exactly once for the result.
[[nodiscard]] auto join(std::span<const StringView> parts, StringView delimiter) -> String;

// ------------------------------------------
// Replace
// ------------------------------------------

/// Returns a copy of `view` with every non-overlapping occurrence of
/// `target` replaced by `replacement`. If `target` is empty, `view` is
/// returned unchanged (as a copy). Allocates exactly once for the
/// result.
[[nodiscard]] auto replace_all(StringView view, StringView target, StringView replacement)
    -> String;

/// Returns a copy of `view` with only the first occurrence of `target`
/// replaced by `replacement`. If `target` is not found, `view` is
/// returned unchanged (as a copy). Allocates exactly once for the
/// result.
[[nodiscard]] auto replace_first(StringView view, StringView target, StringView replacement)
    -> String;

// ------------------------------------------
// Prefix / Suffix queries and removal
// ------------------------------------------

/// Returns true if `view` begins with `prefix`. Byte-wise comparison;
/// no case folding.
[[nodiscard]] constexpr auto starts_with(StringView view, StringView prefix) noexcept -> bool
{
    return view.starts_with(prefix);
}

/// Returns true if `view` ends with `suffix`. Byte-wise comparison; no
/// case folding.
[[nodiscard]] constexpr auto ends_with(StringView view, StringView suffix) noexcept -> bool
{
    return view.ends_with(suffix);
}

/// Returns true if `view` contains `needle` anywhere within it.
/// Byte-wise comparison; no case folding.
[[nodiscard]] constexpr auto contains(StringView view, StringView needle) noexcept -> bool
{
    return view.find(needle) != StringView::npos;
}

/// Returns `view` with `prefix` removed if `view` starts with it,
/// otherwise returns `view` unchanged. Does not allocate.
[[nodiscard]] constexpr auto remove_prefix(StringView view, StringView prefix) noexcept
    -> StringView
{
    return starts_with(view, prefix) ? view.substr(prefix.size()) : view;
}

/// Returns `view` with `suffix` removed if `view` ends with it,
/// otherwise returns `view` unchanged. Does not allocate.
[[nodiscard]] constexpr auto remove_suffix(StringView view, StringView suffix) noexcept
    -> StringView
{
    return ends_with(view, suffix) ? view.substr(0, view.size() - suffix.size()) : view;
}

// ------------------------------------------
// ASCII case conversion
// ------------------------------------------
//
// These operate strictly on the ASCII subset (bytes 0x00-0x7F). Bytes
// outside that range (including any byte that is part of a multi-byte
// UTF-8 sequence) are passed through unchanged. This is intentional and
// documented: full Unicode case folding is out of scope for this phase.

/// Returns a copy of `view` with every ASCII uppercase letter converted
/// to lowercase. Non-ASCII bytes are left unchanged. Allocates exactly
/// once for the result.
[[nodiscard]] auto to_lower_ascii(StringView view) -> String;

/// Returns a copy of `view` with every ASCII lowercase letter converted
/// to uppercase. Non-ASCII bytes are left unchanged. Allocates exactly
/// once for the result.
[[nodiscard]] auto to_upper_ascii(StringView view) -> String;

/// Compares `lhs` and `rhs` for equality, treating ASCII letters as
/// case-insensitive. Non-ASCII bytes are compared byte-for-byte. Does
/// not allocate.
[[nodiscard]] constexpr auto equals_ignore_case_ascii(StringView lhs, StringView rhs) noexcept
    -> bool
{
    if (lhs.size() != rhs.size()) {
        return false;
    }

    auto to_lower_ascii_char = [](char character) noexcept -> char {
        return (character >= 'A' && character <= 'Z') ? static_cast<char>(character - 'A' + 'a')
                                                      : character;
    };

    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), [&](char left, char right) noexcept {
        return to_lower_ascii_char(left) == to_lower_ascii_char(right);
    });
}

// ------------------------------------------
// Numeric conversion
// ------------------------------------------

/// Parses `view` as a value of type `T` using std::from_chars
/// semantics. Fails with ConversionError::TrailingCharacters if `view`
/// is not consumed in its entirety; callers who want partial-parse
/// behavior should call std::from_chars directly. Does not allocate.
template<typename T>
[[nodiscard]] auto parse_number(StringView view) noexcept -> ConversionResult<T>
{
    T value{};
    const auto* begin = view.data();
    const auto* end = view.data() + view.size();

    const std::from_chars_result parse_result = std::from_chars(begin, end, value);

    if (parse_result.ec == std::errc::invalid_argument) {
        return std::unexpected(ConversionError::InvalidArgument);
    }
    if (parse_result.ec == std::errc::result_out_of_range) {
        return std::unexpected(ConversionError::OutOfRange);
    }
    if (parse_result.ptr != end) {
        return std::unexpected(ConversionError::TrailingCharacters);
    }

    return value;
}

/// Formats `value` using std::to_chars semantics and returns the
/// result as an owned String. Allocates exactly once for the result.
/// Throws std::length_error in the (practically unreachable) case
/// that `value`'s formatted representation exceeds the internal
/// fixed-size staging buffer.
template<typename T> [[nodiscard]] auto format_number(T value) -> String
{
    // 64 bytes comfortably covers the longest std::to_chars output for
    // any built-in integer or floating-point type in default format.
    std::array<char, 64> buffer{};

    const std::to_chars_result format_result =
        std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);

    if (format_result.ec != std::errc{}) {
        throw std::length_error("nexus::core::text::format_number: fixed-size buffer too small "
                                "for the requested value");
    }

    return String{buffer.data(), format_result.ptr};
}

// ------------------------------------------
// Hexadecimal formatting / parsing
// ------------------------------------------

/// Formats `value` as a fixed-width, lowercase hexadecimal string with
/// no "0x" prefix (e.g. U32{0xAB} -> "000000ab"). Width is exactly
/// `2 * sizeof(T)` characters. Allocates exactly once for the result.
template<std::unsigned_integral T> [[nodiscard]] auto to_hex(T value) -> String
{
    constexpr types::Size width = 2 * sizeof(T);

    std::array<char, width> buffer{};
    const std::to_chars_result format_result =
        std::to_chars(buffer.data(), buffer.data() + buffer.size(), value, 16);

    // std::to_chars never fails here (the buffer is sized for the
    // widest possible value of T), but it left-pads nothing itself, so
    // the zero-padding below is our responsibility.
    const auto digits_written = static_cast<types::Size>(format_result.ptr - buffer.data());
    const types::Size pad_count = width - digits_written;

    String result(width, '0');
    std::copy(buffer.data(), format_result.ptr,
              result.begin() + static_cast<types::Diff>(pad_count));
    return result;
}

/// Parses `view` as a hexadecimal-encoded value of type `T`. Accepts an
/// optional "0x"/"0X" prefix. Fails with ConversionError::InvalidArgument
/// on non-hex-digit characters or an empty digit sequence, and
/// ConversionError::OutOfRange if the parsed value does not fit in `T`.
/// Does not allocate.
template<std::unsigned_integral T>
[[nodiscard]] auto from_hex(StringView view) noexcept -> ConversionResult<T>
{
    StringView digits = view;
    if (starts_with(digits, "0x") || starts_with(digits, "0X")) {
        digits = digits.substr(2);
    }

    if (digits.empty()) {
        return std::unexpected(ConversionError::InvalidArgument);
    }

    T value{};
    const auto* begin = digits.data();
    const auto* end = digits.data() + digits.size();

    const std::from_chars_result parse_result = std::from_chars(begin, end, value, 16);

    if (parse_result.ec == std::errc::invalid_argument) {
        return std::unexpected(ConversionError::InvalidArgument);
    }
    if (parse_result.ec == std::errc::result_out_of_range) {
        return std::unexpected(ConversionError::OutOfRange);
    }
    if (parse_result.ptr != end) {
        return std::unexpected(ConversionError::TrailingCharacters);
    }

    return value;
}

// ------------------------------------------
// StringBuilder
// ------------------------------------------

/// A lightweight, allocation-conscious utility for incremental string
/// construction. Wraps a single owned buffer; every append call
/// amortizes to O(1) via the buffer's own growth strategy, avoiding
/// the repeated temporary allocations of chained std::string
/// concatenation (`a + b + c + ...`).
///
/// Not thread-safe: a single StringBuilder instance must not be
/// written to concurrently from multiple threads.
class StringBuilder {
public:
    StringBuilder() = default;

    /// Constructs a StringBuilder that reserves `initial_capacity`
    /// bytes up front, avoiding reallocation for callers who can
    /// estimate the final size in advance.
    explicit StringBuilder(types::Size initial_capacity)
    {
        buffer_.reserve(initial_capacity);
    }

    /// Appends `text` to the builder. Returns *this to allow chaining
    /// (e.g. `builder.append("a").append("b")`).
    auto append(StringView text) -> StringBuilder&
    {
        buffer_.append(text);
        return *this;
    }

    /// Appends a single character to the builder. Returns *this to
    /// allow chaining.
    auto append(char character) -> StringBuilder&
    {
        buffer_.push_back(character);
        return *this;
    }

    /// Appends the decimal formatting of `value` to the builder, as
    /// per format_number. Returns *this to allow chaining.
    template<typename T> auto append_number(T value) -> StringBuilder&
    {
        return append(StringView{format_number(value)});
    }

    /// Reserves at least `capacity` bytes in the underlying buffer.
    auto reserve(types::Size capacity) -> void
    {
        buffer_.reserve(capacity);
    }

    /// Returns the number of bytes currently held by the builder.
    [[nodiscard]] auto size() const noexcept -> types::Size
    {
        return buffer_.size();
    }

    /// Returns true if the builder currently holds no characters.
    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return buffer_.empty();
    }

    /// Resets the builder to an empty state without releasing its
    /// underlying capacity, so the buffer can be reused for another
    /// round of construction without reallocating.
    auto clear() noexcept -> void
    {
        buffer_.clear();
    }

    /// Returns a view over the builder's current contents. The view is
    /// only valid until the next non-const call on this StringBuilder
    /// (append/reserve/clear/build may reallocate or mutate the
    /// underlying buffer).
    [[nodiscard]] auto view() const noexcept -> StringView
    {
        return buffer_;
    }

    /// Consumes the builder and returns its contents as an owned
    /// String, moving out of the internal buffer rather than copying.
    /// The builder is left empty after this call.
    [[nodiscard]] auto build() && -> String
    {
        return std::move(buffer_);
    }

    /// Returns a copy of the builder's current contents as an owned
    /// String, leaving the builder unchanged. Allocates exactly once.
    [[nodiscard]] auto build() const& -> String
    {
        return buffer_;
    }

private:
    String buffer_;
};

} // namespace nexus::core::text
