/*
    The code in this file uses open source libraries provided by thecoderscorner

    DO NOT EDIT THIS FILE, IT WILL BE GENERATED EVERY TIME YOU USE THE UI DESIGNER
    INSTEAD EITHER PUT CODE IN YOUR SKETCH OR CREATE ANOTHER SOURCE FILE.

    All the variables you may need access to are marked extern in this file for easy
    use elsewhere.
 */

#include <tcMenu.h>
#include "PhotoRX_menu.h"
#include "ThemeDarkModeModern.h"

// Global variable declarations

const PROGMEM  ConnectorLocalInfo applicationInfo = { "PhotoRXMenu", "8eb52efc-817f-4e31-bcde-388523cf9d6d" };
TFT_eSPI tft;
TfteSpiDrawable tftDrawable(&tft, 0);
GraphicsDeviceRenderer renderer(30, applicationInfo.name, &tftDrawable);

// Global Menu Item declarations

RENDERING_CALLBACK_NAME_INVOKE(fnLogMessageRtCall, textItemRenderFn, "Log Message", -1, NO_CALLBACK)
TextMenuItem menuLogMessage(fnLogMessageRtCall, 58, 20, NULL);
const PROGMEM AnalogMenuInfo minfoMotherboardTemp = { "Motherboard Temp", 59, 0xffff, 255, NO_CALLBACK, -127, 1, "C" };
AnalogMenuItem menuMotherboardTemp(&minfoMotherboardTemp, 0, NULL);
const PROGMEM AnalogMenuInfo minfoTemperaturePCP4 = { "Temperature PCP4", 57, 0xffff, 255, NO_CALLBACK, -127, 1, "C" };
AnalogMenuItem menuTemperaturePCP4(&minfoTemperaturePCP4, 0, &menuMotherboardTemp);
const PROGMEM AnalogMenuInfo minfoTemperaturePCP3 = { "Temperature PCP3", 56, 0xffff, 255, NO_CALLBACK, -127, 1, "C" };
AnalogMenuItem menuTemperaturePCP3(&minfoTemperaturePCP3, 0, &menuTemperaturePCP4);
const PROGMEM AnalogMenuInfo minfoTemperaturePCP2 = { "Temperature PCP2", 55, 0xffff, 255, NO_CALLBACK, -127, 1, "C" };
AnalogMenuItem menuTemperaturePCP2(&minfoTemperaturePCP2, 0, &menuTemperaturePCP3);
const PROGMEM AnalogMenuInfo minfoTemperaturePCP1 = { "Temperature PCP1 ", 54, 0xffff, 255, NO_CALLBACK, -127, 1, "C" };
AnalogMenuItem menuTemperaturePCP1(&minfoTemperaturePCP1, 0, &menuTemperaturePCP2);
RENDERING_CALLBACK_NAME_INVOKE(fnTemperaturesRtCall, backSubItemRenderFn, "Temperatures", -1, NO_CALLBACK)
const PROGMEM SubMenuInfo minfoTemperatures = { "Temperatures", 53, 0xffff, 0, NO_CALLBACK };
BackMenuItem menuBackTemperatures(fnTemperaturesRtCall, &menuTemperaturePCP1);
SubMenuItem menuTemperatures(&minfoTemperatures, &menuBackTemperatures, &menuLogMessage);
RENDERING_CALLBACK_NAME_INVOKE(fnTimeRtCall, timeItemRenderFn, "Time", -1, NO_CALLBACK)
TimeFormattedMenuItem menuTime(fnTimeRtCall, 43, (MultiEditWireType)8, &menuTemperatures);
RENDERING_CALLBACK_NAME_INVOKE(fnDateRtCall, dateItemRenderFn, "Date", -1, NO_CALLBACK)
DateFormattedMenuItem menuDate(fnDateRtCall, 42, &menuTime);
const PROGMEM AnalogMenuInfo minfoChannel15 = { "Channel 15", 41, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel15(&minfoChannel15, 0, NULL);
const PROGMEM AnalogMenuInfo minfoChannel14 = { "Channel 14", 40, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel14(&minfoChannel14, 0, &menuChannel15);
const PROGMEM AnalogMenuInfo minfoChannel13 = { "Channel 13", 39, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel13(&minfoChannel13, 0, &menuChannel14);
const PROGMEM AnalogMenuInfo minfoChannel12 = { "Channel 12", 38, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel12(&minfoChannel12, 0, &menuChannel13);
const PROGMEM AnalogMenuInfo minfoChannel11 = { "Channel 11", 37, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel11(&minfoChannel11, 0, &menuChannel12);
const PROGMEM AnalogMenuInfo minfoChannel10 = { "Channel 10", 36, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel10(&minfoChannel10, 0, &menuChannel11);
const PROGMEM AnalogMenuInfo minfoChannel9 = { "Channel 9", 35, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel9(&minfoChannel9, 0, &menuChannel10);
const PROGMEM AnalogMenuInfo minfoChannel8 = { "Channel 8", 34, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel8(&minfoChannel8, 0, &menuChannel9);
const PROGMEM AnalogMenuInfo minfoChannel7 = { "Channel 7", 33, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel7(&minfoChannel7, 0, &menuChannel8);
const PROGMEM AnalogMenuInfo minfoChannel6 = { "Channel 6", 32, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel6(&minfoChannel6, 0, &menuChannel7);
const PROGMEM AnalogMenuInfo minfoChannel5 = { "Channel 5", 31, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel5(&minfoChannel5, 0, &menuChannel6);
const PROGMEM AnalogMenuInfo minfoChannel4 = { "Channel 4", 30, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel4(&minfoChannel4, 0, &menuChannel5);
const PROGMEM AnalogMenuInfo minfoChannel3 = { "Channel 3", 29, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel3(&minfoChannel3, 0, &menuChannel4);
const PROGMEM AnalogMenuInfo minfoChannel2 = { "Channel 2", 28, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel2(&minfoChannel2, 0, &menuChannel3);
const PROGMEM AnalogMenuInfo minfoChannel1 = { "Channel 1", 27, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel1(&minfoChannel1, 0, &menuChannel2);
const PROGMEM AnalogMenuInfo minfoChannel0 = { "Channel 0", 26, 0xffff, 3000, NO_CALLBACK, 0, 1, "mA" };
AnalogMenuItem menuChannel0(&minfoChannel0, 0, &menuChannel1);
RENDERING_CALLBACK_NAME_INVOKE(fnCurrentConsumpionRtCall, backSubItemRenderFn, "Current Consumpion", -1, NO_CALLBACK)
const PROGMEM SubMenuInfo minfoCurrentConsumpion = { "Current Consumpion", 25, 0xffff, 0, NO_CALLBACK };
BackMenuItem menuBackCurrentConsumpion(fnCurrentConsumpionRtCall, &menuChannel0);
SubMenuItem menuCurrentConsumpion(&minfoCurrentConsumpion, &menuBackCurrentConsumpion, &menuDate);
const PROGMEM AnyMenuInfo minfoEmergencyStop = { "Emergency Stop", 4, 0xffff, 0, onChangeEmergencyStop };
ActionMenuItem menuEmergencyStop(&minfoEmergencyStop, &menuCurrentConsumpion);
const PROGMEM AnyMenuInfo minfoStartExperiment = { "Start Experiment", 48, 0xffff, 0, onChangeStartExperiment };
ActionMenuItem menuStartExperiment(&minfoStartExperiment, &menuEmergencyStop);
const PROGMEM AnyMenuInfo minfoSaveToMode1 = { "SaveToMode1", 50, 0xffff, 0, onSaveToMode1 };
ActionMenuItem menuSaveToMode1(&minfoSaveToMode1, NULL);
const PROGMEM AnyMenuInfo minfoSaveToMode2 = { "SaveToMode2", 51, 0xffff, 0, onSaveToMode2 };
ActionMenuItem menuSaveToMode2(&minfoSaveToMode2, &menuSaveToMode1);
const PROGMEM AnalogMenuInfo minfoSeconds = { "Seconds", 47, 0xffff, 60, NO_CALLBACK, 0, 1, "" };
AnalogMenuItem menuSeconds(&minfoSeconds, 0, NULL);
const PROGMEM AnalogMenuInfo minfoMinutes = { "Minutes", 46, 0xffff, 60, NO_CALLBACK, 0, 1, "" };
AnalogMenuItem menuMinutes(&minfoMinutes, 0, &menuSeconds);
const PROGMEM AnalogMenuInfo minfoHours = { "Hours", 45, 0xffff, 168, NO_CALLBACK, 0, 1, "" };
AnalogMenuItem menuHours(&minfoHours, 0, &menuMinutes);
RENDERING_CALLBACK_NAME_INVOKE(fnExperimentTimeRtCall, backSubItemRenderFn, "Experiment Time", -1, NO_CALLBACK)
const PROGMEM SubMenuInfo minfoExperimentTime = { "Experiment Time", 44, 0xffff, 0, NO_CALLBACK };
BackMenuItem menuBackExperimentTime(fnExperimentTimeRtCall, &menuHours);
SubMenuItem menuExperimentTime(&minfoExperimentTime, &menuBackExperimentTime, &menuSaveToMode2);
const PROGMEM AnalogMenuInfo minfoPercentage = { "Percentage", 7, 0xffff, 100, NO_CALLBACK, 0, 1, "%" };
AnalogMenuItem menuPercentage(&minfoPercentage, 0, &menuExperimentTime);
const char enumStrSquareConfig_0[] PROGMEM = "OO";
const char enumStrSquareConfig_1[] PROGMEM = "OX";
const char enumStrSquareConfig_2[] PROGMEM = "XO";
const char enumStrSquareConfig_3[] PROGMEM = "XX";
const char* const enumStrSquareConfig[] PROGMEM  = { enumStrSquareConfig_0, enumStrSquareConfig_1, enumStrSquareConfig_2, enumStrSquareConfig_3 };
const PROGMEM EnumMenuInfo minfoSquareConfig = { "SelectModule", 49, 0xffff, 3, NO_CALLBACK, enumStrSquareConfig };
EnumMenuItem menuSquareConfig(&minfoSquareConfig, 0, &menuPercentage);
const char enumStrReactorSide_0[] PROGMEM = "Left";
const char enumStrReactorSide_1[] PROGMEM = "Right";
const char enumStrReactorSide_2[] PROGMEM = "Both";
const char* const enumStrReactorSide[] PROGMEM  = { enumStrReactorSide_0, enumStrReactorSide_1, enumStrReactorSide_2 };
const PROGMEM EnumMenuInfo minfoReactorSide = { "Reactor Side", 60, 0xffff, 2, NO_CALLBACK, enumStrReactorSide };
EnumMenuItem menuReactorSide(&minfoReactorSide, 0, &menuSquareConfig);
const char enumStrVialConf_0[] PROGMEM = "SMALL (32x)";
const char enumStrVialConf_1[] PROGMEM = "MEDIUM (4x)";
const char enumStrVialConf_2[] PROGMEM = "BIG (1x)";
const char enumStrVialConf_3[] PROGMEM = "KILO (1)";
const char* const enumStrVialConf[] PROGMEM  = { enumStrVialConf_0, enumStrVialConf_1, enumStrVialConf_2, enumStrVialConf_3 };
const PROGMEM EnumMenuInfo minfoVialConf = { "Vial Config", 5, 0xffff, 3, NO_CALLBACK, enumStrVialConf };
EnumMenuItem menuVialConf(&minfoVialConf, 0, &menuReactorSide);
RENDERING_CALLBACK_NAME_INVOKE(fnConfigurationRtCall, backSubItemRenderFn, "Configuration", -1, NO_CALLBACK)
const PROGMEM SubMenuInfo minfoConfiguration = { "Configuration", 3, 0xffff, 0, NO_CALLBACK };
BackMenuItem menuBackConfiguration(fnConfigurationRtCall, &menuVialConf);
SubMenuItem menuConfiguration(&minfoConfiguration, &menuBackConfiguration, NULL);
const PROGMEM AnyMenuInfo minfoResetConfig = { "Reset Config", 62, 0xffff, 0, onResetConfig };
ActionMenuItem menuResetConfig(&minfoResetConfig, &menuConfiguration);
const PROGMEM AnyMenuInfo minfoSetCurrent = { "Set Current Config", 61, 0xffff, 0, onSetCurrent };
ActionMenuItem menuSetCurrent(&minfoSetCurrent, &menuResetConfig);
const PROGMEM AnyMenuInfo minfoSetMode2 = { "Set Mode 2", 9, 0xffff, 0, onSetMode2 };
ActionMenuItem menuSetMode2(&minfoSetMode2, &menuSetCurrent);
const PROGMEM AnyMenuInfo minfoSetMode1 = { "Set Mode 1", 2, 0xffff, 0, onSetMode1 };
ActionMenuItem menuSetMode1(&minfoSetMode1, &menuSetMode2);
RENDERING_CALLBACK_NAME_INVOKE(fnSettingsRtCall, backSubItemRenderFn, "Settings", -1, NO_CALLBACK)
const PROGMEM SubMenuInfo minfoSettings = { "Settings", 1, 0xffff, 0, NO_CALLBACK };
BackMenuItem menuBackSettings(fnSettingsRtCall, &menuSetMode1);
SubMenuItem menuSettings(&minfoSettings, &menuBackSettings, &menuStartExperiment);

// Set up code

void setupMenu() {
    menuChannel7.setReadOnly(true);
    menuTemperaturePCP3.setReadOnly(true);
    menuChannel0.setReadOnly(true);
    menuChannel2.setReadOnly(true);
    menuTemperaturePCP1.setReadOnly(true);
    menuChannel11.setReadOnly(true);
    menuChannel13.setReadOnly(true);
    menuMotherboardTemp.setReadOnly(true);
    menuChannel4.setReadOnly(true);
    menuChannel9.setReadOnly(true);
    menuTime.setReadOnly(true);
    menuChannel8.setReadOnly(true);
    menuDate.setReadOnly(true);
    menuTemperaturePCP2.setReadOnly(true);
    menuChannel1.setReadOnly(true);
    menuChannel6.setReadOnly(true);
    menuChannel10.setReadOnly(true);
    menuChannel15.setReadOnly(true);
    menuChannel12.setReadOnly(true);
    menuChannel5.setReadOnly(true);
    menuChannel14.setReadOnly(true);
    menuChannel3.setReadOnly(true);
    menuTemperaturePCP4.setReadOnly(true);
    menuLogMessage.setReadOnly(true);

    tft.begin();
    tft.setRotation(1);
    renderer.setUpdatesPerSecond(20);
    switches.initialise(internalDigitalIo(), true);
    menuMgr.initForEncoder(&renderer, &menuSettings, 39, 36, 34);
    renderer.setTitleMode(BaseGraphicalRenderer::TITLE_FIRST_ROW);
    renderer.setUseSliderForAnalog(true);
    installDarkModeModernTheme(renderer, MenuFontDef(nullptr, 2), MenuFontDef(nullptr, 4), true);
}

