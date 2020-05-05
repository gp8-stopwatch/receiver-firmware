/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef FAKE_MICROCONTROLLER_ENGINE_H
#define FAKE_MICROCONTROLLER_ENGINE_H

#include "Gpio.h"
#include "Usart.h"
#include <chrono>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace lm {

/**
 * @brief The ICommand struct
 */
struct ICommand {
        virtual ~ICommand () = default;
        virtual void run () = 0;
};

/**
 * Simulate a state change on the GPIO pin.
 */
template <bool b> class GpioState : public ICommand {
public:
        explicit GpioState (Gpio &g) : gpio (g) {}
        virtual ~GpioState () = default;

        virtual void run () override
        {
                gpio.state = b;

                if (gpio.onToggle) {
                        gpio.onToggle ();
                }
        }

private:
        Gpio &gpio;
};

/**
 * Simulate one character transmited to the USART from outside.
 */
class UsartChar : public ICommand {
public:
        UsartChar (Usart &u, char c) : usart (u), c (c) {}
        virtual ~UsartChar () override = default;

        virtual void run () override;

private:
        Usart &usart;
        char c;
};

///**
// * Simulate a stream of characters transmited to the USART from outside.
// */
// class UsartString : public ICommand {
// public:
//        UsartString (Usart &u, std::string s) : usart (u), string (std::move (s)) {}
//        virtual ~UsartString () override = default;

//        virtual void run () override;

// private:
//        Usart &usart;
//        std::string string;
//};

template <typename StringT> class UsartString : public ICommand {
public:
        using StringType = StringT;
        using CharacterType = typename StringType::value_type;

        UsartString (Usart &u, StringType s) : usart (u), string (std::move (s)) {}
        virtual ~UsartString () override = default;

        virtual void run () override;

private:
        Usart &usart;
        StringType string;
};

template <typename StringT> void UsartString<StringT>::run ()
{
        if (usart.sink) {
                for (CharacterType c : string) {
                        usart.sink->onData (static_cast<uint8_t> (c));
                }
        }
        else if (usart.onData) {
                for (CharacterType c : string) {
                        usart.onData (static_cast<uint8_t> (c));
                }
        }
}

template <> class UsartString<const char *> : public ICommand {
public:
        using StringType = const char *;
        using CharacterType = char;

        UsartString (Usart &u, const char *s) : usart (u), string (s) {}
        virtual ~UsartString () override = default;

        virtual void run () override;

private:
        Usart &usart;
        std::string string;
};

/**
 * @brief The Event struct
 */
struct Event {

        Event () = default;
        Event (std::chrono::milliseconds w, std::unique_ptr<ICommand> c) : when (w), command (std::move (c)) {}

        Event (Event &&e) = default;
        Event &operator= (Event &&e) = default;

        Event (Event const &) = delete;
        Event &operator= (Event const &) = delete;

        std::chrono::milliseconds when{};
        std::unique_ptr<ICommand> command;
};

/**
 *
 */
template <typename EventCollectionT> class Engine {
public:
        using EventCollectionType = EventCollectionT;

        Engine (EventCollectionType &&v);

        /// Call this from a main loop.
        void run ();

private:
        void threadLoop () {}

private:
        EventCollectionType events;
        typename EventCollectionType::iterator currentEvent;
        std::chrono::system_clock::time_point startTime;
        std::thread thread;
        std::mutex mutex;

        using CommandQueue = std::deque<std::unique_ptr<ICommand>>;
        CommandQueue commandQueue;
};

template <typename EventCollectionT>
Engine<EventCollectionT>::Engine (EventCollectionT &&v)
    : events (std::move (v)), currentEvent (events.begin ()), startTime (std::chrono::system_clock::now ()), thread (&Engine::threadLoop, this)
{
        thread.detach ();
}

// template <typename EventCollectionT> void Engine<EventCollectionT>::threadLoop ()
//{
//        while (true) {
//                if (currentEvent == events.end ()) {
//                        return;
//                }

//                using namespace std::chrono;

//                system_clock::time_point nowTime = system_clock::now ();
//                duration duration = nowTime - startTime;

//                if (duration_cast<milliseconds> (duration) > currentEvent->when) {
//                        {
//                                std::lock_guard<std::mutex> lock (mutex);
//                                commandQueue.push_back (std::move (currentEvent->command));
//                        }

//                        std::this_thread::sleep_for (500us);
//                        ++currentEvent;
//                }
//        }
//}

// template <typename EventCollectionT> void Engine<EventCollectionT>::run ()
//{
//        {
//                std::lock_guard<std::mutex> lock (mutex);

//                for (auto &c : commandQueue) {
//                        c->run ();
//                }

//                commandQueue.clear ();
//        }
//}

template <typename EventCollectionT> void Engine<EventCollectionT>::run ()
{
        if (currentEvent == events.end ()) {
                return;
        }

        using namespace std::chrono;

        system_clock::time_point nowTime = system_clock::now ();
        duration duration = nowTime - startTime;

        if (duration_cast<milliseconds> (duration) > currentEvent->when) {
                currentEvent->command->run ();
                ++currentEvent;
        }
}

} // namespace lm

#endif // ENGINE_H
