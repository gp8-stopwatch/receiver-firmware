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
        static auto refreshMenu = [] { return [] (menu::Event p) { return p == menu::Event::refreshMenu; }; };
        static int currentResult{};

        auto tryDisplayResult = [&] {
                currentResult %= History::MAX_RESULTS_NUM;
                auto en = history->getEntry (currentResult);

                if (en.result != std::numeric_limits<uint32_t>::max ()) {
                        display.setTime (en.result, getConfig ().getResolution ());
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
                auto d = rtc->getDate ();
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
                auto d = rtc->getDate ();
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
                state ("STOP_WATCH"_ST, entry ([&] { machine.run (Event::Type::reset); }), exit ([&] {
                               display.clear ();
                               machine.run (Event::Type::pause);
                               currentResult = 0;
                       }),
#ifdef WITH_HISTORY
                       transition ("RESULT"_ST, shortPress ())),
#else
                       transition ("FLIP"_ST, shortPress ())),
#endif

#ifdef WITH_HISTORY
                // Results
                state ("RESULT"_ST, entry (tryDisplayResult), exit ([] {}),
                       transition ("NEXT_RESULT"_ST, longPress (), [] (auto /*a*/) { /* ++currentResult; */ }),
                       transition ("FLIP"_ST, shortPress ())),

                state ("NEXT_RESULT"_ST, entry (tryDisplayResult), exit ([] {}),
                       transition ("NEXT_RESULT"_ST, shortPress (), [] (auto /*a*/) { /* ++currentResult; */ }),
                       transition ("FLIP"_ST, longPress ())),
#endif

                // Screen orientation
                state ("FLIP"_ST, entry ([&] { display.setText ("1.FLIP "); }), exit ([] {}),
                       transition ("FLIP"_ST, longPress (),
                                   [&] (auto /*a*/) {
                                           config.setDisplayRightSideUp (!config.isDisplayRightSideUp ());
                                           cfg::changed () = true;
                                   }),
                       transition ("IR_ON"_ST, shortPress ())),

                // IR sensor on/off
                state ("IR_ON"_ST, entry ([&] { display.setText ((config.isIrSensorOn ()) ? ("2.I.r.on ") : ("2.I.r.off")); }), exit ([] {}),
                       transition ("IR_ON"_ST, refreshMenu ()),
                       transition ("IR_ON"_ST, longPress (),
                                   [&] (auto /*a*/) {
                                           config.setIrSensorOn (!config.isIrSensorOn ());
                                           cfg::changed () = true;
                                   }),
                       transition ("BUZZER"_ST, shortPress ())),

                // Buzzer on/off
                state ("BUZZER"_ST, entry ([&] { display.setText ((config.isBuzzerOn ()) ? ("3.Sn.on ") : ("3.Sn.off")); }), exit ([] {}),
                       transition ("BUZZER"_ST, refreshMenu ()),
                       transition ("BUZZER"_ST, longPress (),
                                   [&] (auto /*a*/) {
                                           config.setBuzzerOn (!config.isBuzzerOn ());
                                           cfg::changed () = true;
                                   }),
                       transition ("RESOLUTION"_ST, shortPress ())),

                // Resolution
                state ("RESOLUTION"_ST, entry ([&] {
                               switch (config.getResolution ()) {
                               case Resolution::ms_10:
                                       display.setText ("4.10ms ");
                                       break;

                               case Resolution::ms_1:
                                       display.setText ("4.1ms  ");
                                       break;

                               case Resolution::us_100:
                                       display.setText ("4.100us");
                                       break;

                               case Resolution::us_10:
                                       display.setText ("4.10us ");
                                       break;

                               default:
                                       break;
                               }
                       }),
                       exit ([] {}), transition ("RESOLUTION"_ST, refreshMenu ()),
                       transition ("RESOLUTION"_ST, longPress (),
                                   [&] (auto /*a*/) {
                                           config.setResolution (
                                                   Resolution ((int (config.getResolution ()) - 1) % RESOLUTION_NUMBER_OF_OPTIONS));
                                           cfg::changed () = true;
                                   }),
                       transition ("STOP_MODE"_ST, shortPress ())),

                // Stop mode loop/stop. Loop is for trainig, stop is for competition.
                state ("STOP_MODE"_ST, entry ([&] { display.setText ((config.getStopMode ()) ? ("5.Stop ") : ("5.Loop ")); }), exit ([] {}),
                       transition ("STOP_MODE"_ST, refreshMenu ()),
                       transition ("STOP_MODE"_ST, longPress (),
                                   [&] (auto /*a*/) {
                                           config.setStopMode (StopMode (!bool (config.getStopMode ())));
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
                       transition ("SET_HOUR"_ST, shortPress (), [&] (auto /*a*/) { rtc->timeAdd (Rtc::Set::hour); }),
                       transition ("SET_MINUTES"_ST, longPress (), [&] (auto /*a*/) {})),

                state ("SET_MINUTES"_ST, entry ([] {}), exit ([] {}), //
                       transition ("SET_MINUTES"_ST, timePassed (), [&] (auto /*a*/) { displayTime (Rtc::Set::minute); }),
                       transition ("SET_MINUTES"_ST, shortPress (), [&] (auto /*a*/) { rtc->timeAdd (Rtc::Set::minute); }),
                       transition ("SET_SECONDS"_ST, longPress (), [&] (auto /*a*/) {})),

                state ("SET_SECONDS"_ST, entry ([] {}), exit ([] {}), //
                       transition ("SET_SECONDS"_ST, timePassed (), [&] (auto /*a*/) { displayTime (Rtc::Set::second); }),
                       transition ("SET_SECONDS"_ST, shortPress (), [&] (auto /*a*/) { rtc->timeAdd (Rtc::Set::second); }),
                       transition ("TIME"_ST, longPress (), [&] (auto /*a*/) {})),

                // Date viewing and setting
                state ("DATE"_ST, entry ([&] { displayDate (Rtc::Set::none); }), exit ([] {}), //
                       transition ("DATE"_ST, timePassed (), [&] (auto /*a*/) {}),             //
                       transition ("SET_YEAR"_ST, longPress (), [&] (auto /*a*/) {}),          //
                       transition ("STOP_WATCH"_ST, shortPress ())),                           //

                state ("SET_YEAR"_ST, entry ([] {}), exit ([] {}), //
                       transition ("SET_YEAR"_ST, timePassed (), [&] (auto /*a*/) { displayDate (Rtc::Set::year); }),
                       transition ("SET_YEAR"_ST, shortPress (), [&] (auto /*a*/) { rtc->dateAdd (Rtc::Set::year); }),
                       transition ("SET_MONTH"_ST, longPress (), [&] (auto /*a*/) {})),

                state ("SET_MONTH"_ST, entry ([] {}), exit ([] {}), //
                       transition ("SET_MONTH"_ST, timePassed (), [&] (auto /*a*/) { displayDate (Rtc::Set::month); }),
                       transition ("SET_MONTH"_ST, shortPress (), [&] (auto /*a*/) { rtc->dateAdd (Rtc::Set::month); }),
                       transition ("SET_DAY"_ST, longPress (), [&] (auto /*a*/) {})),

                state ("SET_DAY"_ST, entry ([] {}), exit ([] {}), //
                       transition ("SET_DAY"_ST, timePassed (), [&] (auto /*a*/) { displayDate (Rtc::Set::day); }),
                       transition ("SET_DAY"_ST, shortPress (), [&] (auto /*a*/) { rtc->dateAdd (Rtc::Set::day); }),
                       transition ("DATE"_ST, longPress (), [&] (auto /*a*/) {})));

        m.run (p);
#endif
}
