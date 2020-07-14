/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "DisplayMenu.h"

/****************************************************************************/

void DisplayMenu::onEvent (menu::Event p)
{
#ifdef WITH_MENU
        using namespace ls;
        static auto longPress = [] { return [] (menu::Event p) { return p == menu::Event::longPress; }; };
        static auto shortPress = [] { return [] (menu::Event p) { return p == menu::Event::shortPress; }; };
        static auto timePassed = [] { return [] (menu::Event p) { return p == menu::Event::timePassed; }; };
        static int currentResult{};

        auto tryDisplayResult = [&] {
                currentResult %= History::MAX_RESULTS_NUM;
                auto en = history.getEntry (currentResult);

                if (en.result != std::numeric_limits<uint32_t>::max ()) {
                        display.setTime (en.result, getConfig ().resolution);
                }
                else { // Last
                        if (currentResult == 0) {
                                display.setText ("empty ");
                        }
                        else {
                                currentResult = 0;
                                display.setText ("end   ");
                                return;
                        }
                }

                ++currentResult;
        };

        // If true then it blinks
        auto displayTime = [&] (Rtc::Set s) {
                auto d = rtc.getDate ();
                auto &t = d.second;
                static uint8_t b{};
                ++b;

                if (s == Rtc::Set::hour && b % 2 == 0) {
                        display.setDigit (0, ' ');
                        display.setDigit (1, ' ');
                }
                else {
                        display.setDigit (0, t.Hours / 10);
                        display.setDigit (1, t.Hours % 10);
                }

                if (s == Rtc::Set::minute && b % 2 == 0) {
                        display.setDigit (2, ' ');
                        display.setDigit (3, ' ');
                }
                else {
                        display.setDigit (2, t.Minutes / 10);
                        display.setDigit (3, t.Minutes % 10);
                }

                if (s == Rtc::Set::second && b % 2 == 0) {
                        display.setDigit (4, ' ');
                        display.setDigit (5, ' ');
                }
                else {
                        display.setDigit (4, t.Seconds / 10);
                        display.setDigit (5, t.Seconds % 10);
                }
                display.setDots (0b001010);
        };

        // s tells what blinks
        auto displayDate = [&] (Rtc::Set s) {
                auto d = rtc.getDate ();
                auto &r = d.first;
                static uint8_t b{};
                ++b;

                if (s == Rtc::Set::year && b % 2 == 0) {
                        display.setDigit (0, ' ');
                        display.setDigit (1, ' ');
                }
                else {
                        display.setDigit (0, r.Year / 10);
                        display.setDigit (1, r.Year % 10);
                }

                if (s == Rtc::Set::month && b % 2 == 0) {
                        display.setDigit (2, ' ');
                        display.setDigit (3, ' ');
                }
                else {
                        display.setDigit (2, (r.Month + 1) / 10);
                        display.setDigit (3, (r.Month + 1) % 10);
                }

                if (s == Rtc::Set::day && b % 2 == 0) {
                        display.setDigit (4, ' ');
                        display.setDigit (5, ' ');
                }
                else {
                        display.setDigit (4, (r.Date + 1) / 10);
                        display.setDigit (5, (r.Date + 1) % 10);
                }

                display.setDots (0b001010);
        };

        static auto m = ls::machine (
                // Main screen : the stopwatch
                state ("STOP_WATCH"_ST, entry ([&] { machine.run (Event::reset); }), exit ([&] {
                               display.clear ();
                               machine.run (Event::pause);
                               currentResult = 0;
                       }),
                       transition ("RESULT"_ST, shortPress ())),

                // Results
                state ("RESULT"_ST, entry (tryDisplayResult), exit ([] {}),
                       transition ("NEXT_RESULT"_ST, longPress (), [] (auto /*a*/) { /* ++currentResult; */ }),
                       transition ("FLIP"_ST, shortPress ())),

                state ("NEXT_RESULT"_ST, entry (tryDisplayResult), exit ([] {}),
                       transition ("NEXT_RESULT"_ST, shortPress (), [] (auto /*a*/) { /* ++currentResult; */ }),
                       transition ("FLIP"_ST, longPress ())),

                // Screen orientation
                state ("FLIP"_ST, entry ([&] { display.setText ("1.FLIP "); }), exit ([] {}),
                       transition ("FLIP"_ST, longPress (),
                                   [&] (auto /*a*/) {
                                           config.orientationFlip = !config.orientationFlip;
                                           cfg::changed () = true;
                                   }),
                       transition ("IR_ON"_ST, shortPress ())),

                // IR sensor on/off
                state ("IR_ON"_ST, entry ([&] { display.setText ((config.irSensorOn) ? ("2.I.r.on ") : ("2.I.r.off")); }), exit ([] {}),
                       transition ("IR_ON"_ST, longPress (),
                                   [&] (auto /*a*/) {
                                           config.irSensorOn = !config.irSensorOn;
                                           cfg::changed () = true;
                                   }),
                       transition ("BUZZER"_ST, shortPress ())),

                // Buzzer on/off
                state ("BUZZER"_ST, entry ([&] { display.setText ((config.buzzerOn) ? ("3.Sn.on ") : ("3.Sn.off")); }), exit ([] {}),
                       transition ("BUZZER"_ST, longPress (),
                                   [&] (auto /*a*/) {
                                           config.buzzerOn = !config.buzzerOn;
                                           cfg::changed () = true;
                                   }),
                       transition ("RESOLUTION"_ST, shortPress ())),

                // Resolution
                state ("RESOLUTION"_ST, entry ([&] {
                               switch (config.resolution) {
                               case Resolution::ms_10:
                                       display.setText ("4.ms.10 ");
                                       break;

                               case Resolution::ms_1:
                                       display.setText ("4.ms.1  ");
                                       break;

                               case Resolution::us_100:
                                       display.setText ("4.us.100");
                                       break;

                               case Resolution::us_10:
                                       display.setText ("4.us.10 ");
                                       break;

                               default:
                                       break;
                               }
                       }),
                       exit ([] {}),
                       transition ("RESOLUTION"_ST, longPress (),
                                   [&] (auto /*a*/) {
                                           config.resolution = Resolution ((int (config.resolution) - 1) % RESOLUTION_NUMBER_OF_OPTIONS);
                                           cfg::changed () = true;
                                   }),
                       transition ("STOP_MODE"_ST, shortPress ())),

                // Stop mode loop/stop. Loop is for trainig, stop is for competition.
                state ("STOP_MODE"_ST, entry ([&] { display.setText ((config.stopMode) ? ("5.Stop ") : ("5.Loop ")); }), exit ([] {}),
                       transition ("STOP_MODE"_ST, longPress (),
                                   [&] (auto /*a*/) {
                                           config.stopMode = StopMode (!bool (config.stopMode));
                                           cfg::changed () = true;
                                   }),
                       transition ("TIME"_ST, shortPress ())),

                // Time viewing and setting
                state ("TIME"_ST, entry ([&] { displayTime (Rtc::Set::none); }), exit ([] {}), //
                       transition ("TIME"_ST, timePassed (), [&] (auto /*a*/) {}),             //
                       transition ("SET_HOUR"_ST, longPress (), [&] (auto /*a*/) {}),          //
                       transition ("DATE"_ST, shortPress ())),                                 //

                state ("SET_HOUR"_ST, entry ([] {}), exit ([] {}), //
                       transition ("SET_HOUR"_ST, timePassed (), [&] (auto /*a*/) { displayTime (Rtc::Set::hour); }),
                       transition ("SET_HOUR"_ST, shortPress (), [&] (auto /*a*/) { rtc.timeAdd (Rtc::Set::hour); }),
                       transition ("SET_MINUTES"_ST, longPress (), [&] (auto /*a*/) {})),

                state ("SET_MINUTES"_ST, entry ([] {}), exit ([] {}), //
                       transition ("SET_MINUTES"_ST, timePassed (), [&] (auto /*a*/) { displayTime (Rtc::Set::minute); }),
                       transition ("SET_MINUTES"_ST, shortPress (), [&] (auto /*a*/) { rtc.timeAdd (Rtc::Set::minute); }),
                       transition ("SET_SECONDS"_ST, longPress (), [&] (auto /*a*/) {})),

                state ("SET_SECONDS"_ST, entry ([] {}), exit ([] {}), //
                       transition ("SET_SECONDS"_ST, timePassed (), [&] (auto /*a*/) { displayTime (Rtc::Set::second); }),
                       transition ("SET_SECONDS"_ST, shortPress (), [&] (auto /*a*/) { rtc.timeAdd (Rtc::Set::second); }),
                       transition ("TIME"_ST, longPress (), [&] (auto /*a*/) {})),

                // Date viewing and setting
                state ("DATE"_ST, entry ([&] { displayDate (Rtc::Set::none); }), exit ([] {}), //
                       transition ("DATE"_ST, timePassed (), [&] (auto /*a*/) {}),             //
                       transition ("SET_YEAR"_ST, longPress (), [&] (auto /*a*/) {}),          //
                       transition ("STOP_WATCH"_ST, shortPress ())),                           //

                state ("SET_YEAR"_ST, entry ([] {}), exit ([] {}), //
                       transition ("SET_YEAR"_ST, timePassed (), [&] (auto /*a*/) { displayDate (Rtc::Set::year); }),
                       transition ("SET_YEAR"_ST, shortPress (), [&] (auto /*a*/) { rtc.dateAdd (Rtc::Set::year); }),
                       transition ("SET_MONTH"_ST, longPress (), [&] (auto /*a*/) {})),

                state ("SET_MONTH"_ST, entry ([] {}), exit ([] {}), //
                       transition ("SET_MONTH"_ST, timePassed (), [&] (auto /*a*/) { displayDate (Rtc::Set::month); }),
                       transition ("SET_MONTH"_ST, shortPress (), [&] (auto /*a*/) { rtc.dateAdd (Rtc::Set::month); }),
                       transition ("SET_DAY"_ST, longPress (), [&] (auto /*a*/) {})),

                state ("SET_DAY"_ST, entry ([] {}), exit ([] {}), //
                       transition ("SET_DAY"_ST, timePassed (), [&] (auto /*a*/) { displayDate (Rtc::Set::day); }),
                       transition ("SET_DAY"_ST, shortPress (), [&] (auto /*a*/) { rtc.dateAdd (Rtc::Set::day); }),
                       transition ("DATE"_ST, longPress (), [&] (auto /*a*/) {})));

        m.run (p);
#endif
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

void DisplayMenu::onShortPress ()
{
        // Cycle the menu.
        option = Option ((int (option) + 1) % int (Option::last_option));
        prepareMenuForOption (option);
}

/*****************************************************************************/

void DisplayMenu::onLongPress ()
{
        switch (option) {
        case Option::stop_watch:
                break;

        case Option::flip:
                config.orientationFlip = !config.orientationFlip;
                break;

        case Option::ir_on:
                config.irSensorOn = !config.irSensorOn;
                break;

        case Option::buzzer_on:
                config.buzzerOn = !config.buzzerOn;
                break;

        case Option::resolution:
                config.resolution = Resolution ((int (config.resolution) - 1) % RESOLUTION_NUMBER_OF_OPTIONS);
                break;

        case Option::stopMode:
                config.stopMode = StopMode (!bool (config.stopMode));
                break;

        case Option::time:
                ++settingState;
                settingState %= 4; // 1 : set hour, 2 : set minutes, 3 : set seconds, 0 : exit setting
                break;

        case Option::date:
                break;

        default:
                break;
        }

        prepareMenuForOption (option);

        if (option != Option::stop_watch) {
                cfg::changed () = true;
        }
}

/*****************************************************************************/

void DisplayMenu::prepareMenuForOption (Option o)
{
        display.clear ();

        if (o == Option::stop_watch) {
                machine.run (Event::reset);
        }
        else {
                machine.run (Event::pause);
        }

        switch (o) {
        case Option::stop_watch:
                break;

        case Option::flip:
                display.setText ("1.FLIP");
                break;

        case Option::ir_on:
                if (config.irSensorOn) {
                        display.setText ("2.I.r.on");
                }
                else {
                        display.setText ("2.I.r.off");
                }

                break;

        case Option::buzzer_on:
                if (config.buzzerOn) {
                        display.setText ("3.Sn.on");
                }
                else {
                        display.setText ("3.Sn.off");
                }
                break;

        case Option::resolution:
                switch (config.resolution) {
                case Resolution::ms_10:
                        display.setText ("4.ms.10");
                        break;

                case Resolution::ms_1:
                        display.setText ("4.ms.1");
                        break;

                case Resolution::us_100:
                        display.setText ("4.us.100");
                        break;

                case Resolution::us_10:
                        display.setText ("4.us.10");
                        break;

                default:
                        break;
                }

                break;

        case Option::stopMode:
                if (config.stopMode == StopMode::stop) {
                        display.setText ("5.Stop");
                }
                else {
                        display.setText ("5.loop");
                }
                break;

        case Option::time: {
                auto d = rtc.getDate ();
                auto &t = d.second;

                display.setDigit (0, t.Hours / 10);
                display.setDigit (1, t.Hours % 10);
                display.setDigit (2, t.Minutes / 10);
                display.setDigit (3, t.Minutes % 10);
                display.setDigit (4, t.Seconds / 10);
                display.setDigit (5, t.Seconds % 10);
                display.setDots (0b001010);
        } break;

        case Option::date: {
                auto d = rtc.getDate ();
                auto &r = d.first;

                display.setDigit (0, r.Year / 10);
                display.setDigit (1, r.Year % 10);
                display.setDigit (2, r.Month / 10);
                display.setDigit (3, r.Month % 10);
                display.setDigit (4, r.Date / 10);
                display.setDigit (5, r.Date % 10);
                display.setDots (0b001010);
        } break;

        default:
                break;
        }
}

/****************************************************************************/

void DisplayMenu::run ()
{
        if ((option == Option::time || option == Option::date) && timeDisplay.isExpired () && settingState == 0) {
                prepareMenuForOption (option);
                timeDisplay.start (1000);
        }
}