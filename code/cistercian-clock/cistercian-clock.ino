/*
 * This file is part of Cistercian Clock.
 *
 * Cistercian Clock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Cistercian Clock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Cistercian Clock. If not, see <https://www.gnu.org/licenses/>.
 */

#include <Wire.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <Commander.h>          // https://github.com/CreativeRobotics/Commander - MIT License
#include <Time.h>               // https://github.com/PaulStoffregen/Time - LGPL License
#include <TimeLib.h>            // https://github.com/PaulStoffregen/Time - LGPL License
#include <FastLED.h>            // https://github.com/FastLED/FastLED
#include <Timezone.h>           // https://github.com/JChristensen/Timezone - GPL License
#include <RtcDS3231.h>          // https://github.com/Makuna/Rtc - GPL License

RtcDS3231<TwoWire> Rtc(Wire);

WiFiUDP ntpUDP;

#include "config_struct.h"

clockConfig_t clockConfig;

// Saskatchewan Time Zone (Regina, Saskatoon, Hepburn)
TimeChangeRule myDST = {"CST", Second, Sun, Mar, 2, -360};    // Daylight time = UTC - 6 hours
TimeChangeRule mySTD = {"CST", First, Sun, Nov, 2, -360};     // Standard time = UTC - 6 hours
Timezone myTZ(myDST, mySTD);

time_t lastRebootTime;

time_t getHwTime(void) {
  RtcDateTime tm = Rtc.GetDateTime();
  return tm.Epoch32Time();
}

void setHwTime(time_t epochTime) {
  RtcDateTime tm;
  tm.InitWithEpoch32Time(epochTime);
  Rtc.SetDateTime(tm);
}

void setup(void) {
  Serial.begin(115200);
  Serial.println(F("Starting"));

  SPIFFS.begin();

  loadConfig(&clockConfig);

  if (strlen(clockConfig.wifiSSID) > 0)
  {
    Serial.println(F("Connecting to WiFi..."));
    WiFi.mode(WIFI_STA);
    WiFi.begin(clockConfig.wifiSSID, clockConfig.wifiKey);

    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      i++;
      if (i > 20)
      {
        break;
      }
    }
  }

  render_setup();

  Wire.begin();
  tmElements_t tm;

  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if (!Rtc.IsDateTimeValid())
  {
    if (Rtc.LastError() != 0)
    {
      // we have a communications error
      // see https://www.arduino.cc/en/Reference/WireEndTransmission for 
      // what the number means
      Serial.print("RTC communications error = ");
      Serial.println(Rtc.LastError());
    }
    else
    {
      // Common Causes:
      //    1) first time you ran and the device wasn't running yet
      //    2) the battery on the device is low or even missing

      Serial.println("RTC lost confidence in the DateTime!");

      // following line sets the RTC to the date & time this sketch was compiled
      // it will also reset the valid flag internally unless the Rtc device is
      // having an issue

      Rtc.SetDateTime(compiled);
    }
  }
  if (!Rtc.GetIsRunning())
  {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
  }
  
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
  
  setTime(getHwTime());
  setSyncProvider(getHwTime);
  setSyncInterval(600);

  Serial.print(F("The current time is: "));

  tmElements_t newTime;
  char strbuf[32];
  time_t tmp_t;
  
  tmp_t = now();
  tmp_t = myTZ.toLocal(tmp_t);
  breakTime(tmp_t, newTime);
  snprintf(strbuf, 32, "%04d-%02d-%02d %02d:%02d:%02d ", tmYearToCalendar(newTime.Year), newTime.Month, newTime.Day, newTime.Hour, newTime.Minute, newTime.Second);
  Serial.println(strbuf);

  ntp_init();
  ntp_config(clockConfig.ntpServer, clockConfig.ntpInterval);

  randomSeed(now());
  command_setup();

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("cistercianclock");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  lastRebootTime = now();
}

void loop(void) {
  tmElements_t nowTime;
  time_t tmp_t, my_tmp_t;
  static time_t last_t;
  static bool didRun = false;
  uint16_t brightTime;
  uint8_t bright;
  static tmElements_t bright_now;


  command_loop();

  tmp_t = now();

  if ((tmp_t - last_t) >= 10)
  {
    if (!didRun)
    {
      last_t = ((tmp_t / 10) * 10);
  
      my_tmp_t = myTZ.toLocal(tmp_t);
      breakTime(my_tmp_t, nowTime);
  
      yield();
      // Time rendering code
      breakTime(my_tmp_t, bright_now);

      brightTime = (bright_now.Hour * 100) + bright_now.Minute;

      if (clockConfig.nightModeStart > clockConfig.dayModeStart)
      {
        if ((brightTime >= clockConfig.dayModeStart) && (brightTime < clockConfig.nightModeStart))
        {
          bright = clockConfig.dayModeBright;
        }
        else
        {
          bright = clockConfig.nightModeBright;
        }
      }
      else
      {
        if ((brightTime >= clockConfig.nightModeStart) && (brightTime < clockConfig.dayModeStart))
        {
          bright = clockConfig.nightModeBright;
        }
        else
        {
          bright = clockConfig.dayModeBright;
        }
      }
    
      render_time(nowTime, bright);

      didRun = true;
    }
  }
  else
  {
    didRun = false;
  }
  yield();

  if (WiFi.status() == WL_CONNECTED)
  {
    ArduinoOTA.handle();
    if (strlen(clockConfig.ntpServer) > 0)
    {
      if (clockConfig.ntpInterval > 0)
      {
        time_t ntp_t;
        if (ntp_handle(&ntp_t))
        {
          setHwTime(ntp_t);
          setTime(ntp_t);
          Serial.print(F("NTP update. Timestamp: "));
          Serial.println(ntp_t);
        }
      }
    }
  }
}
