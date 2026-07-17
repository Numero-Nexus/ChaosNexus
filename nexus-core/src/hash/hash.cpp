// ==========================================
// nexus::core::hash — TransparentHash Implementation
// ==========================================
//
// Out-of-line body for TransparentHash::operator(), kept separate
// from hash.hpp since it is not a template/constexpr entity (unlike
// the rest of this subsystem).

#include "nexus/core/hash/hash.hpp"

namespace nexus::core::hash {

auto TransparentHash::operator()(std::string_view value) const noexcept -> std::size_t
{
    return std::hash<std::string_view>{}(value);
}

} // namespace nexus::core::hash
