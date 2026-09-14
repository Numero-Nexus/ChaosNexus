#include "nexus/core/error/exception.hpp"
#include "nexus/core/events/event.hpp"
#include "nexus/core/events/event_queue.hpp"
#include "nexus/core/identity/ids.hpp"
#include "nexus/core/identity/uuid.hpp"
#include "nexus/core/kernel/component.hpp"
#include "nexus/core/kernel/component_registry.hpp"
#include "nexus/core/time/time_point.hpp"

#include <gtest/gtest.h>
#include <memory>

namespace nexus::core::kernel::test {

namespace {

using nexus::core::InternalException;
using nexus::core::NodeId;
using nexus::core::Uuid;
using nexus::core::events::Event;
using nexus::core::events::EventQueue;
using nexus::core::events::EventType;
using nexus::core::time::TimePoint;

[[nodiscard]] auto make_node_id(core::types::U8 marker) -> NodeId
{
    Uuid::ByteArray bytes{};
    bytes[0] = marker;
    return NodeId{Uuid{bytes}};
}

[[nodiscard]] auto make_event_id(core::types::U8 marker) -> EventId
{
    Uuid::ByteArray bytes{};
    bytes[1] = marker; // distinct byte from make_node_id to avoid accidental UUID collision
    return EventId{Uuid{bytes}};
}

/// Test double recording every Event it receives and, when
/// configured, pushing a fixed follow-up Event onto the queue handed
/// to it -- exercising the on_event(event, queue) follow-up path
/// without depending on any real simulation behaviour.
class RecordingComponent final : public SimulationComponent {
public:
    auto on_event(const Event& event, EventQueue& queue) -> void override
    {
        received_.push_back(event);
        if (follow_up_.has_value()) {
            queue.push(*follow_up_);
        }
    }

    [[nodiscard]] auto received() const noexcept -> const std::vector<Event>&
    {
        return received_;
    }

    auto set_follow_up(Event event) -> void
    {
        follow_up_ = event;
    }

private:
    std::vector<Event> received_;
    std::optional<Event> follow_up_;
};

} // namespace

TEST(ComponentRegistry, StartsEmpty)
{
    const ComponentRegistry registry;
    EXPECT_TRUE(registry.empty());
    EXPECT_EQ(registry.size(), 0U);
}

TEST(ComponentRegistry, RegisterMakesComponentRetrievable)
{
    ComponentRegistry registry;
    const auto id = make_node_id(1);
    registry.register_component(id, std::make_unique<RecordingComponent>());

    EXPECT_TRUE(registry.contains(id));
    EXPECT_EQ(registry.size(), 1U);
}

TEST(ComponentRegistry, RegisterNilIdThrows)
{
    ComponentRegistry registry;
    EXPECT_THROW(registry.register_component(NodeId::nil(), std::make_unique<RecordingComponent>()),
                 InternalException);
}

TEST(ComponentRegistry, RegisterNullComponentThrows)
{
    ComponentRegistry registry;
    EXPECT_THROW(registry.register_component(make_node_id(1), nullptr), InternalException);
}

TEST(ComponentRegistry, RegisterDuplicateIdThrows)
{
    ComponentRegistry registry;
    const auto id = make_node_id(1);
    registry.register_component(id, std::make_unique<RecordingComponent>());

    EXPECT_THROW(registry.register_component(id, std::make_unique<RecordingComponent>()),
                 InternalException);
}

TEST(ComponentRegistry, GetOnUnregisteredIdThrows)
{
    ComponentRegistry registry;
    EXPECT_THROW((void)registry.get(make_node_id(1)), InternalException);
}

TEST(ComponentRegistry, GetReturnsRegisteredComponentInstance)
{
    ComponentRegistry registry;
    const auto id = make_node_id(1);
    auto* raw = new RecordingComponent{};
    registry.register_component(id, std::unique_ptr<SimulationComponent>{raw});

    EXPECT_EQ(&registry.get(id), static_cast<SimulationComponent*>(raw));
}

TEST(ComponentRegistry, DispatchedEventReachesComponent)
{
    ComponentRegistry registry;
    const auto id = make_node_id(1);
    registry.register_component(id, std::make_unique<RecordingComponent>());

    EventQueue queue;
    const Event event{make_event_id(1), TimePoint::from_ticks(5), EventType{1U}};

    auto& component = registry.get(id);
    component.on_event(event, queue);

    EXPECT_TRUE(queue.empty()); // no follow-up configured
}

TEST(ComponentRegistry, ComponentCanScheduleFollowUpEventOnQueue)
{
    ComponentRegistry registry;
    const auto id = make_node_id(1);
    auto owned = std::make_unique<RecordingComponent>();
    auto* raw = owned.get();
    registry.register_component(id, std::move(owned));

    EventQueue queue;
    const Event trigger{make_event_id(1), TimePoint::from_ticks(5), EventType{1U}};
    const Event follow_up{make_event_id(2), TimePoint::from_ticks(7), EventType{2U}, trigger.id()};
    raw->set_follow_up(follow_up);

    registry.get(id).on_event(trigger, queue);

    ASSERT_FALSE(queue.empty());
    EXPECT_EQ(queue.pop(), follow_up);
    EXPECT_EQ(raw->received().size(), 1U);
    EXPECT_EQ(raw->received().front(), trigger);
}

} // namespace nexus::core::kernel::test
