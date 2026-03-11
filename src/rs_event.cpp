#include "rs_event.h"

void RS::Event::Event_Sink::emit(EventHeader header, Event_TypeID ID, const void *payload = NULL)
{
    
    std::vector<RS::Event::Event_Sink::Entry> callbacks;
    {
        std::lock_guard<std::mutex> lock(mtx_);

        auto map_it = listener.find(ID);
        if(map_it == listener.end()) { return; }

        callbacks = map_it->second;
    }

    for(auto it = callbacks.begin(); it != callbacks.end(); it++)
    {
        it->cb(header, payload);
    }
}

RS::Event::EventHandle RS::Event::Event_Sink::subscribe(Event_TypeID type, EventCallbackFn cb)
{
    return subscribe(type, cb);
}

RS::Event::EventHandle RS::Event::Event_Sink::subscribe(Event_TypeID_Register type, EventCallbackFn cb)
{
    std::lock_guard<std::mutex> lock(mtx_);

    Entry handle;
    handle.cb = cb;
    handle.id = EventHandle_NextID.fetch_add(1);
    if(listener.contains(type))
    {
        listener[type].push_back(std::move(handle));
    }
    else
    {         
        std::vector<Entry> Et;
        Et.push_back(std::move(handle));
        listener.insert_or_assign(type, std::move(Et)); 
    }
    EventHandle Ehandle(type, handle.id);

    return Ehandle;
}

void RS::Event::Event_Sink::unsubscribe(EventHandle sub)
{
    std::lock_guard<std::mutex> lock(mtx_);

    auto map_it = listener.find(sub.type);
    if(map_it == listener.end()) { return; }

    auto& vec = map_it->second;
    
    std::erase_if(vec, [&sub](const Entry& e)
    {
        return e.id == sub.id;
    });

   if (vec.empty())
   {
       listener.erase(map_it);
   }
}
