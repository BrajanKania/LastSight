#pragma once

#include <vector>

namespace ls::dispatch {

  class IEventBuffer {
  public:
    virtual ~IEventBuffer() = default;
    virtual void clear() = 0;
  };

  template <typename TEvent>
  class EventBuffer : public IEventBuffer {
  public:
    void push(const TEvent& event) { events_.push_back(event); }

    void clear() override { events_.clear(); }

    const std::vector<TEvent>& getEvents() const { return events_; }

  private:
    std::vector<TEvent> events_{};
  };

}  // namespace ls::dispatch
