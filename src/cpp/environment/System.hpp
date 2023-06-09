#pragma once
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include "Environment.hpp"
namespace Sacred {
struct Subscriber;
typedef unsigned long long EventID;
typedef std::vector<Subscriber*> SubscriberList;
struct EventSystem : std::unordered_map<EventID, SubscriberList> {
  void trigger(EventID group_id, std::shared_ptr<void> data) {
    if (count(group_id)) {
      const auto& subscriber_list = at(group_id);
      std::for_each(subscriber_list.begin(), subscriber_list.end(),
                    [&group_id, data](auto& subscriber) {
                      subscriber->receive_event(group_id, data);
                    });
    }
  }
  void register_subscriber(EventID group_id, Subscriber& subscriber) {}
  void unregister_subscriber(EventID group_id, Subscriber& subscriber) {}
};

struct Subscriber {
  virtual void receive_event(
      EventID group_id,
      std::shared_ptr<void>
          data) = 0;  // acknowledge notification with responsive behavior
};

struct System : Subscriber {
  virtual void receive_event(EventID group_id, std::shared_ptr<void> data) = 0;
};
}  // namespace Sacred