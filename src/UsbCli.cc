/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "UsbCli.h"
#include "Cli.h"
#include "Container.h"
#include "usb/UsbHelpers.h"
#include "usbd_cdc.h"
#include "usbd_composite.h"
#include "usbd_desc.h"

USBD_HandleTypeDef USBD_Device{};
bool showGreeting{};

namespace cl {

template <> struct Traits<String> {
        static constexpr LineEnd outputLineEnd{LineEnd::crlf};
        static constexpr size_t maxTokenSize = 16;
        static constexpr bool echo = true;
        static constexpr Case comparison = Case::insensitive;
};

template <> void output<String> (String const &tok) { usbWrite (tok.c_str ()); }
template <> void output<char> (char const &tok) { usbWriteData (reinterpret_cast<uint8_t const *> (&tok), 1); }
template <> void output<const char *> (const char *const &tok) { usbWrite (tok); }

} // namespace cl

namespace usbcli {

static auto refreshAll = [] {
        getMenu ().onEvent (menu::Event::refreshMenu);
        cfg::changed () = true;
};

auto &getCli ()
{
        static auto cli = cl::cli<String> (
                cl::cmd (String ("result"), [] { getHistory ().printHistory (); }),
                cl::cmd (String ("resultMs"), [] { getHistory ().printHistory (ResultDisplayStyle::milisecondOnly); }),
                cl::cmd (String ("last"), [] { getHistory ().printLast (); }),
                cl::cmd (String ("lastMs"), [] { getHistory ().printLast (ResultDisplayStyle::milisecondOnly); }),
                cl::cmd (String ("date"),
                         [] {
                                 auto r = getRtc ().getDate ();
                                 printDate (r.first, r.second);
                                 usbWrite ("\r\n\r\n");
                         }),

                cl::cmd (String ("iscounting"), [] { usbWrite ((getFastStateMachine ().isCounting ()) ? ("1\r\n\r\n") : ("0\r\n\r\n")); }),
                cl::cmd (String ("reset"), [] { getFastStateMachine ().run (Event::Type::reset); }),

                cl::cmd (String ("clear"),
                         [] {
                                 getHistory ().clearHiScore ();
                                 getHistory ().clearResults ();
                         }),
                cl::cmd (String ("factory"),
                         [&] {
                                 getConfigFlashEepromStorage ().clear ();
                                 readConfigFromFlash ();
                                 getConfig ().restoreDefaults ();
                                 refreshAll ();
                                 //  HAL_NVIC_SystemReset (); // TODO does not work
                         }),
                cl::cmd (String ("help"),
                         [] {
                                 usbWrite ("result, resultMs, last, lastMs, date, isCounting, reset, clear, factory, help, battery, "
                                           "flip, ir, sn, res, auto, bright, "
                                           "periph, fps, blind, trigger\r\n\r\n");
                         }),
                cl::cmd (String ("battery"),
                         [] {
                                 std::array<char, 11> buf{};
                                 itoa ((unsigned int)(getPowerManager ().getBatteryVoltage ()), buf.data ());
                                 usbWrite (buf.cbegin ());
                                 usbWrite ("mV, ");

                                 itoa ((unsigned int)(getPowerManager ().getBatteryPercent ()), buf.data ());
                                 usbWrite (buf.cbegin ());
                                 usbWrite ("%\r\n\r\n");
                         }),

                cl::cmd (String ("flip"),
                         [&] (String const &arg) {
                                 if (arg.empty ()) {
                                         usbWrite ((getConfig ().isFlip ()) ? ("1\r\n\r\n") : ("0\r\n\r\n"));
                                 }
                                 else {
                                         getConfig ().setFlip (bool (std::atoi (arg.c_str ())));
                                         refreshAll ();
                                 }
                         }),

                cl::cmd (String ("ir"),
                         [&] (String const &arg) {
                                 if (arg.empty ()) {
                                         usbWrite ((getConfig ().isIrSensorOn ()) ? ("1\r\n\r\n") : ("0\r\n\r\n"));
                                 }
                                 else {
                                         getConfig ().setIrSensorOn (bool (std::atoi (arg.c_str ())));
                                         refreshAll ();
                                 }
                         }),

                cl::cmd (String ("sn"),
                         [&] (String const &arg) {
                                 if (arg.empty ()) {
                                         usbWrite ((getConfig ().isBuzzerOn ()) ? ("1\r\n\r\n") : ("0\r\n\r\n"));
                                 }
                                 else {
                                         getConfig ().setBuzzerOn (bool (std::atoi (arg.c_str ())));
                                         refreshAll ();
                                 }
                         }),

                cl::cmd (String ("res"),
                         [&] (String const &arg) {
                                 if (arg.empty ()) {
                                         switch (getConfig ().getResolution ()) {
                                         case Resolution::ms_10:
                                                 usbWrite ("10ms\r\n\r\n");
                                                 break;

                                         case Resolution::ms_1:
                                                 usbWrite ("1ms\r\n\r\n");
                                                 break;

                                         case Resolution::us_100:
                                                 usbWrite ("100us\r\n\r\n");
                                                 break;

                                         case Resolution::us_10:
                                                 usbWrite ("10us\r\n\r\n");
                                                 break;

                                         default:
                                                 break;
                                         }
                                 }
                                 else {
                                         if (arg == "10ms") {
                                                 getConfig ().setResolution (Resolution::ms_10);
                                         }
                                         else if (arg == "1ms") {
                                                 getConfig ().setResolution (Resolution::ms_1);
                                         }
                                         else if (arg == "100us") {
                                                 getConfig ().setResolution (Resolution::us_100);
                                         }
                                         else if (arg == "10us") {
                                                 getConfig ().setResolution (Resolution::us_10);
                                         }
                                         else {
                                                 usbWrite ("Valid options : 10ms, 1ms, 100us, 10us\r\n\r\n");
                                         }

                                         refreshAll ();
                                 }
                         }),

                cl::cmd (String ("auto"),
                         [&] (String const &arg) {
                                 if (arg.empty ()) {
                                         switch (getConfig ().getAutoDisplayResult ()) {
                                         case ResultDisplayStyle::secondFraction:
                                                 usbWrite ("s\r\n\r\n");
                                                 break;

                                         case ResultDisplayStyle::milisecondOnly:
                                                 usbWrite ("ms\r\n\r\n");
                                                 break;

                                         case ResultDisplayStyle::none:
                                                 usbWrite ("none\r\n\r\n");
                                                 break;

                                         default:
                                                 break;
                                         }
                                 }
                                 else {
                                         if (arg == "s") {
                                                 getConfig ().setAutoDisplayResult (ResultDisplayStyle::secondFraction);
                                         }
                                         else if (arg == "ms") {
                                                 getConfig ().setAutoDisplayResult (ResultDisplayStyle::milisecondOnly);
                                         }
                                         else if (arg == "none") {
                                                 getConfig ().setAutoDisplayResult (ResultDisplayStyle::none);
                                         }
                                         else {
                                                 usbWrite ("Valid options : s, ms, none\r\n\r\n");
                                         }
                                 }
                         }),

                cl::cmd (String ("bright"),
                         [&] (String const &arg) {
                                 if (arg.empty ()) {
                                         auto br = getConfig ().getBrightness ();

                                         if (br == Brightness::levelAuto) {
                                                 usbWrite ("auto");
                                         }
                                         else {
                                                 std::array<char, 11> buf{};
                                                 itoa ((unsigned int)(br), buf.data ());
                                                 usbWrite (buf.cbegin ());
                                         }

                                         usbWrite ("\r\n\r\n");
                                 }
                                 else {

                                         if (arg == "auto") {
                                                 getConfig ().setBrightness (Brightness::levelAuto);
                                         }
                                         else if (!arg.empty () && arg[0] >= '1' && arg[0] <= '4') {
                                                 getConfig ().setBrightness (Brightness (arg[0] - '0'));
                                         }
                                         else {
                                                 usbWrite ("1-4 or auto\r\n\r\n");
                                         }

                                         refreshAll ();
                                 }
                         }),

                cl::cmd (String ("fps"),
                         [&] (String const &arg) {
                                 if (arg.empty ()) {
                                         print ((unsigned)getConfig ().getFps ());
                                         usbWrite ("\r\n\r\n");
                                 }
                                 else {
                                         int i = std::atoi (arg.c_str ());

                                         if (i < MIN_FPS || i > MAX_FPS) {
                                                 usbWrite ("Correct values are [20, 1000]\r\n\r\n"); // TODO FPS is assumed to be between 20 and
                                                                                                     // 1000. Hardcoded
                                                 return;
                                         }

                                         getConfig ().setFps (i);
                                         refreshAll ();
                                 }
                         }),

                cl::cmd (String ("periph"),
                         [] {
                                 getProtocol ().sendInfoRequest ();
                                 HAL_Delay (RESPONSE_WAIT_TIME_MS);
                                 auto &resp = getProtocol ().getInfoRespDataCollection ();

                                 usbWrite ("All devices (this dev at the top):\r\n");
                                 usbWrite ("type uid active beam noise noise_level\r\n");
                                 printInfoResp (getMyOwnInfo ());

                                 for (auto &periph : resp) {
                                         printInfoResp (periph);
                                 }

                                 print ("\r\n");
                         }),

                cl::cmd (String ("blind"),
                         [&] (String const &arg) {
                                 if (arg.empty ()) {
                                         print ((unsigned)getConfig ().getBlindTime ());
                                         usbWrite ("\r\n\r\n");
                                 }
                                 else {
                                         int i = std::atoi (arg.c_str ());

                                         if (i < 0 || i > std::numeric_limits<uint16_t>::max () - 1) {
                                                 usbWrite ("Correct values are [0, 65534]ms\r\n\r\n");
                                                 return;
                                         }

                                         getConfig ().setBlindTime (i);
                                         refreshAll ();
                                 }
                         }),

                cl::cmd (String ("trigger"),
                         [&] (String const &arg) {
                                 if (arg.empty ()) {
                                         print ((unsigned)getConfig ().getMinTreggerEventMs ());
                                         usbWrite ("\r\n\r\n");
                                 }
                                 else {
                                         int i = std::atoi (arg.c_str ());

                                         if (i < 0 || i > std::numeric_limits<uint16_t>::max () - 1) {
                                                 usbWrite ("Correct values are [0, 65534]ms\r\n\r\n");
                                                 return;
                                         }

                                         getConfig ().setMinTriggerEventMs (i);
                                         refreshAll ();
                                 }
                         })

        );

        return cli;
}

/****************************************************************************/

void init ()
{
#ifdef WITH_USB
        __disable_irq ();

        /* Initialize Device Library */
        USBD_Init (&USBD_Device, &USBD_Desc, 0);

        /* to work within ST's drivers, I've written a special USBD_Composite class that then invokes several classes */
        USBD_RegisterClass (&USBD_Device, &USBD_Composite);

        /* Start Device Process */
        USBD_Start (&USBD_Device);

        usbOnConnected ([] { showGreeting = true; });

        getCli ();

        usbOnData ([] (const uint8_t *data, size_t len) {
                for (size_t i = 0; i < len; ++i) {
                        auto dt = gsl::span{data, len};
                        getCli ().input (char (dt[i]));
                }
        });

        /* OK, only *now* it is OK for the USB interrupts to fire */
        __enable_irq ();
#endif
}

/****************************************************************************/

void run ()
{
#ifdef WITH_USB
        getCli ().run ();

        if (showGreeting) {
                usbWrite ("GP8 stopwatch version: ");
                usbWrite (VERSION);
                usbWrite ("\r\n");
                showGreeting = false;
        }
#endif
}

} // namespace usbcli