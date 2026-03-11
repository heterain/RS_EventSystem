#pragma once

#include "rs_type.h"

#include <functional>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <utility>
#include <mutex>
#include <memory>
#include <cstdint>
#include <string_view>

namespace RS::Event
{
    class Event_Sink;

    // 编译期哈希：给 EventId 用（跨库一致） 
    constexpr uint64_t fnv1a64(const char* s) 
    { 
        constexpr uint64_t FNV1A_64_OFFSET_BASIS = 14695981039346656037ULL;
        constexpr uint64_t FNV1A_64_PRIME = 1099511628211ULL;

        uint64_t hash = FNV1A_64_OFFSET_BASIS;

        for (const char* p = s; *p != '\0'; ++p) {
            hash ^= static_cast<uint64_t>(static_cast<std::uint8_t>(*p)); // XOR the bottom with the current octet
            hash *= FNV1A_64_PRIME;                                      // Multiply by the FNV magic prime
        }

        return hash;
    }

    using EventHandle_ID = RS_uint64_t;
    using Event_TypeID = RS_uint64_t;

    //必须是 UTF-8 / 只允许 ASCII
    //统一大小写
    //fmt : xxx.xx.xx.xxx.x...
    class Event_TypeID_Register
    {
    private:
        Event_TypeID id_;
    public:
        consteval explicit Event_TypeID_Register(const char* str) // todo : 字符串格式一致性
        {
            auto s = std::string_view{str};
            if (s.empty())
                throw "Event name must not be empty";

            // --- FNV-1a 64 ---
            constexpr std::uint64_t OFFSET = 14695981039346656037ull;
            constexpr std::uint64_t PRIME  = 1099511628211ull;

            std::uint64_t h = OFFSET;

            for (std::size_t i = 0; i < s.size(); ++i)
            {
                unsigned char c = static_cast<unsigned char>(s[i]);
            
                // 只允许 ASCII
                if (c > 0x7F)
                    throw "Event name has invalid character (must be ASCII)";
            
                // 统一小写：直接拒绝大写
                if (c >= 'A' && c <= 'Z')
                    throw "Event name must be lowercase (A-Z not allowed)";
            
                // 允许：a-z 0-9 . _ -
                const bool allowed =
                    (c >= 'a' && c <= 'z') ||
                    (c >= '0' && c <= '9') ||
                    (c == '.') || (c == '_') || (c == '-');
            
                if (!allowed)
                    throw "Event name has invalid character (must be ASCII lowercase + [a-z0-9._-])";
            
                // 额外格式约束：不允许以 '.' 开头/结尾，不允许连续 ".."
                if (c == '.')
                {
                    if (i == 0 || i + 1 == s.size())
                        throw "Event name must not start/end with '.'";
                    if (i > 0 && s[i - 1] == '.')
                        throw "Event name must not contain '..'";
                }
            
                // FNV-1a
                h ^= static_cast<std::uint64_t>(c);
                h *= PRIME;
            }
        
            id_ = static_cast<RS::Event::Event_TypeID>(h);
        }

        constexpr Event_TypeID value() const noexcept { return id_; }
        constexpr operator Event_TypeID() const noexcept { return id_; }
    };

    struct EventHandle
    {
        const Event_TypeID type;
        const EventHandle_ID id;
        EventHandle(const Event_TypeID type, const EventHandle_ID id) : 
        type(type), id(id){}
    };

    struct EventHeader
    {
        u32 flags;
        size_t payload_size;
    };
    

    using EventCallbackFn = std::function<void(const EventHeader& header ,const void* payload)>;

    class Event_Sink
    {
    private:
        struct Entry
        {
            EventHandle_ID id;
            EventCallbackFn cb;
        };

        std::unordered_map<Event_TypeID, std::vector<Entry>> listener;
        inline static std::atomic<EventHandle_ID> EventHandle_NextID{1};

        std::mutex mtx_;

    public:
        void emit(EventHeader header, Event_TypeID ID, const void* payload);
        EventHandle subscribe(Event_TypeID type, EventCallbackFn cb);
        EventHandle subscribe(Event_TypeID_Register type, EventCallbackFn cb);
        void unsubscribe(EventHandle sub);
    };
} // namespace RS::Event