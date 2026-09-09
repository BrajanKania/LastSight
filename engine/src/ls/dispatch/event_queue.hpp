#pragma once

#include <memory>
#include <vector>

#include "ls/dispatch/event_buffer.hpp"
#include "ls/dispatch/types.hpp"

namespace ls::dispatch {

  class EventQueue {
  public:
    template <typename TEvent>
    void publish(const TEvent& event) {
      getEventBufferPointer<TEvent>()->push(event);
    }

    void clear() {
      for (auto& buffer : eventBuffers_) {
        if (buffer) {
          buffer->clear();
        }
      }
    }

    template <typename TEvent>
    const std::vector<TEvent>& getEvents() {
      return getEventBufferPointer<TEvent>()->getEvents();
    }

  private:
    template <typename TEvent>
    EventId getEventId() {
      static EventId id{ nextEventId_++ };
      return id;
    }

    template <typename TEvent>
    EventBuffer<TEvent>* getEventBufferPointer() {
      EventId eventId{ getEventId<TEvent>() };

      if (eventId >= eventBuffers_.size()) {
        eventBuffers_.resize(eventId + 1);
      }

      if (!eventBuffers_[eventId]) {
        eventBuffers_[eventId] = std::make_unique<EventBuffer<TEvent>>();
      }

      return static_cast<EventBuffer<TEvent>*>(eventBuffers_[eventId].get());
    }

    inline static EventId nextEventId_{ 0 };
    std::vector<std::unique_ptr<IEventBuffer>> eventBuffers_{};
  };

}  // namespace ls::dispatch
