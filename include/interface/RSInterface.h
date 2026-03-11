#pragma once

#include <queue>
#include <functional>

#include "rs_event.h"

namespace RS
{
    class Event_Pip
    {
        struct Event_Context
        {
            Event::EventHeader header;
            Event::Event_TypeID ID;
            const void* payload = NULL;
        };
        Event::Event_Sink MassageSink;
        std::queue<Event_Context> MessageQueue;
    };
    

    struct Rs_Sink
    {
        Event_Pip Pip;
    };

    class Initiators
    {
    private:
        std::reference_wrapper<Event_Pip> Pip;

        Initiators(Initiators&);
        Initiators(Event_Pip& Ep) : Pip(Ep){}
        Initiators(Rs_Sink& Ep) : Pip(Ep.Pip){}

        void connect(Initiators& );
        void emit(Event::EventHeader header,
                Event::Event_TypeID ID,
                const void* payload = NULL);
        Event::EventHandle subscribe(Event::Event_TypeID_Register type, Event::EventCallbackFn cb);
        Event::EventHandle subscribe(Event::Event_TypeID type, Event::EventCallbackFn cb);
        void unsubscribe(Event::EventHandle sub);
    };
}