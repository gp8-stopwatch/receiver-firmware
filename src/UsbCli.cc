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
                                 // HAL_NVIC_SystemReset (); // TODO uncoment and test
                         }),
                cl::cmd (String ("help"),
                         [] {
                                 usbWrite ("result, resultMs, last, lastMs, date, isCounting, reset, clear, factory, help, battery, "
                                           "getFlip, "
                                           "setFlip, getIr, setIr, getSn, "
                                           "setSn, getRes, setRes, getAuto, setAuto, periph, getBlind, setBlind\r\n\r\n");
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

                cl::cmd (String ("getFlip"), [] { usbWrite ((getConfig ().isFlip ()) ? ("1\r\n\r\n") : ("0\r\n\r\n")); }),
                cl::cmd (String ("setFlip"),
                         [&] (String const &arg) {
                                 getConfig ().setFlip (bool (std::atoi (arg.c_str ())));
                                 refreshAll ();
                         }),

                cl::cmd (String ("getIr"), [] { usbWrite ((getConfig ().isIrSensorOn ()) ? ("1\r\n\r\n") : ("0\r\n\r\n")); }),
                cl::cmd (String ("setIr"),
                         [&] (String const &arg) {
                                 getConfig ().setIrSensorOn (bool (std::atoi (arg.c_str ())));
                                 refreshAll ();
                         }),

                cl::cmd (String ("getSn"), [] { usbWrite ((getConfig ().isBuzzerOn ()) ? ("1\r\n\r\n") : ("0\r\n\r\n")); }),
                cl::cmd (String ("setSn"),
                         [&] (String const &arg) {
                                 getConfig ().setBuzzerOn (bool (std::atoi (arg.c_str ())));
                                 refreshAll ();
                         }),

                cl::cmd (String ("getRes"),
                         [] {
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
                         }),
                cl::cmd (String ("setRes"),
                         [&] (String const &arg) {
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
                         }),

                cl::cmd (String ("setAuto"),
                         [&] (String const &arg) {
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
                         }),
                cl::cmd (String ("getAuto"),
                         [] {
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
                         }),

                cl::cmd (String ("periph"),
                         [] {
                                 getProtocol ().sendInfoRequest ();
                                 HAL_Delay (RESPONSE_WAIT_TIME_MS);
                                 auto &resp = getProtocol ().getInfoRespDataCollection ();

                                 usbWrite ("Connected peripherals :\r\n");
                                 usbWrite ("type uid beam_state\r\n");

                                 for (auto &periph : resp) {
                                         switch (periph.deviceType) {
                                         case DeviceType::receiver:
                                                 usbWrite ("receiver ");
                                                 break;

                                         case DeviceType::ir_sensor:
                                                 usbWrite ("ir_sensor ");
                                                 break;

                                         default:
                                                 usbWrite ("unknown ");
                                                 break;
                                         }

                                         print ((unsigned)periph.uid);
                                         print (" ");

                                         switch (periph.beamState) {
                                         case BeamState::yes:
                                                 usbWrite ("yes");
                                                 break;

                                         case BeamState::no:
                                                 usbWrite ("no");
                                                 break;

                                         case BeamState::blind:
                                                 usbWrite ("blind");
                                                 break;
                                         }

                                         print ("\r\n");
                                 }

                                 print ("\r\n");
                         }),

                cl::cmd (String ("getBlind"),
                         [&] (String const &arg) {
                                 print ((unsigned)getConfig ().getBlindTime ());
                                 usbWrite ("\r\n\r\n");
                         }),
                cl::cmd (String ("setBlind"),
                         [&] (String const &arg) {
                                 int i = std::atoi (arg.c_str ());

                                 if (i < 0 || i > std::numeric_limits<uint16_t>::max () - 1) {
                                         usbWrite ("Correct values are [0, 65534]\r\n\r\n");
                                         return;
                                 }

                                 getConfig ().setBlindTime (i);
                                 refreshAll ();
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