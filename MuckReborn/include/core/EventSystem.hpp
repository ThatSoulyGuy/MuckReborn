#ifndef EVENT_SYSTEM_HPP
#define EVENT_SYSTEM_HPP

#include <string>
#include <map>
#include <vector>

#define stringify(name) #name

enum class EventType
{
    MR_PRE_INIT_EVENT,
    MR_INIT_EVENT,
    MR_CLEANUP_EVENT,
    WINDOW_INIT_EVENT,
    WINDOW_CLEANUP_EVENT,
    RENDERER_CLEANUP_EVENT,
};

std::map<EventType, std::string> EventType2String = 
{
    {EventType::MR_PRE_INIT_EVENT, "TC_PRE_INIT_EVENT"},
    {EventType::MR_INIT_EVENT, "TC_INIT_EVENT"},
    {EventType::MR_CLEANUP_EVENT, "TC_CLEANUP_EVENT"},
    {EventType::WINDOW_INIT_EVENT, "WINDOW_INIT_EVENT"},
    {EventType::WINDOW_CLEANUP_EVENT, "WINDOW_CLEANUP_EVENT"},
    {EventType::RENDERER_CLEANUP_EVENT, "RENDERER_CLEANUP_EVENT"},
};

struct EventListener
{
    virtual void OnEvent(EventType type, void* data, bool& cancel) { }
};

namespace EventSystem
{
	std::vector<EventListener> events;

    void RegisterListener(EventListener& listener)
    {
        events.push_back(listener);
    }

    void DispatchEvent(EventType type, void* data)
    {
        for (auto& listener : events)
        {
            bool cancel;

            listener.OnEvent(type, data, cancel);
        }

        //Logger_WriteConsole(fmt::format("Event: '{}' dispached.", EventType2String[type].c_str()), LogLevel::DEBUG);
    }
}

#endif // !EVENT_SYSTEM_HPP