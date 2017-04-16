#ifndef ST7735_AD_CLOCKDISPLAY_HPP
#define ST7735_AD_CLOCKDISPLAY_HPP

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Time.h>

// To keep track of analog watch hands
// so we can erase them on update
struct drawnLine {
    int x;
    int y;
    int xDestination;
    int yDestination;
    float angle;
};

class ST7335_AD_ClockDisplay{
    private:
        Adafruit_ST7735 m_screen;

        bool m_analog = false,
            m_alarm = false,
            m_date = false;

        time_t m_timer;

        // Pins
        int m_SCLK = 0,
            m_MOSI = 0,
            m_CS = 0,
            M_RST = 0,
            m_DC = 0;

        // Timer variables to check if we need
        // to redraw a value
        int m_lastSecond = 0,
            m_currentSecond = 0,
            m_lastMinute = 0,
            m_currentMinute = 56,
            m_currentHour = 20,
            m_lastHour = 0,
            m_currentDay = 0,
            m_lastDay = 0,
            m_currentWeekday = 1,
            m_lastWeekday = 0;

        // Alarm values
        int m_alarmMinute = 0,
            m_alarmHour = 0;

        int m_colors[8] = {
            ST7735_BLACK,
            ST7735_BLUE,
            ST7735_RED,
            ST7735_GREEN,
            ST7735_CYAN,
            ST7735_MAGENTA,
            ST7735_YELLOW,
            ST7735_WHITE
        };

        // Colors
        int fgIndex = 7;
        int bgIndex = 0;
        int m_fgColor = m_colors[fgIndex];
        int m_bgColor = m_colors[bgIndex];

        drawnLine m_drawnLines[3];

        void drawDigitalClock(int color);
        void drawAnalogClock(int color);
        void drawWatchFace(int width, int height, int radius, int color);
        void drawWatchIndicators(int width, int height, int radius, int color);
        void drawHand(int x, int y, float angle, int lenght, int index, int color);
        void cleanUpLines();
        void ForceCleanUpLines();
        bool isProblemArea(float angle);
        void drawDate(int x, int y, int color);
        void drawAlarm(int x, int y, int color);
        // void drawTextCentered(int16_t x, int16_t y, const char *string, int8_t size,uint16_t color);
        void drawTextCentered(int x, int y, const char *string, int size, int color);
    public:
        ST7335_AD_ClockDisplay(int8_t CS_PIN, int8_t DC_PIN, int8_t RST_PIN);
        ~ST7335_AD_ClockDisplay();

        void init();
        void setTime(int hour, int minute, int second, int day, int month, int year);
        void drawClock();
        void redraw();
        void showAlarm();
        void hideAlarm();
        void showDate();
        void hideDate();
        void setAnalog(bool state) { m_analog = state; }
        bool isAnalog() { return m_analog; }
        void incrementFgColor();
        void incrementBgColor();

        Adafruit_ST7735 getScreen() { return m_screen; }
        time_t getTimer() { return m_timer; }
        void setTimer(time_t time) { m_timer = time; }

        int getCurrentSecond() { return m_currentSecond; }
        int getCurrentMinute() { return m_currentMinute; }
        int getCurrentHour() { return m_currentHour; }
        int getCurrentDay() { return m_currentDay; }
        int getCurrentWeekday() { return m_currentWeekday; }

        int getLastSecond() { return m_lastSecond; }
        int getLastMinute() { return m_lastMinute; }
        int getLastHour() { return m_lastHour; }
        int getLastDay() { return m_lastDay; }
        int getLastWeekday() { return m_lastWeekday; }
};

#endif
