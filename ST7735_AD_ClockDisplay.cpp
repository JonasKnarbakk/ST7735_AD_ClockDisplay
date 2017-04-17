// Description: New Class
// Created by Jonas Knarbakk (19:27:31-14/04/2017)

#include "ST7735_AD_ClockDisplay.h"

ST7335_AD_ClockDisplay::ST7335_AD_ClockDisplay(uint8_t CS_PIN, uint8_t DC_PIN, uint8_t RST_PIN)
: m_screen(CS_PIN, DC_PIN, RST_PIN){
    // TODO: Fix bug when setting date
}

ST7335_AD_ClockDisplay::~ST7335_AD_ClockDisplay(){
}

void ST7335_AD_ClockDisplay::drawDigitalClock(uint16_t color){

    // Draw a black rect over the seconds value
    m_screen.fillRect(103, m_screen.height()/2-5, 12, 7, m_bgColor);

    // Draw a black rect over the minutes value
    if(m_lastMinute != m_currentMinute){
        m_screen.fillRect(67, m_screen.height()/2-5, 34, 21, m_bgColor);
        m_lastMinute = m_currentMinute;
    }
    // Draw a black rect over the hour value
    if(m_lastHour != m_currentHour){
        m_screen.fillRect(14, m_screen.height()/2-5, 34, 21, m_bgColor);
        m_lastHour = m_currentHour;
    }

    // Get the time value
    char * hh = (char*)calloc(3, sizeof(char));
    itoa(m_currentHour, hh, 10);
    char * mm = (char*)calloc(3, sizeof(char));
    itoa(m_currentMinute, mm, 10);
    char * ss = (char*)calloc(3, sizeof(char));
    itoa(m_currentSecond, ss, 10);

    // Set cursor position and text size for HH:MM
    m_screen.setCursor(m_screen.width()/2-50, m_screen.height()/2-5);
    m_screen.setTextColor(color);
    m_screen.setTextSize(3);

    // Draw hours
    if(m_currentHour < 10){
        m_screen.print("0");
    }
    m_screen.print(hh);
    m_screen.print(":");

    // Draw minutes
    if(m_currentMinute < 10){
        m_screen.print("0");
    }
    m_screen.print(mm);

    // Adjust size for seconds
    m_screen.setTextSize(1);
    
    // Draw seconds
    if(m_currentSecond < 10){
        m_screen.print("0");
    }
    m_screen.print(ss);

    // Free allocated memory
    free(hh);
    free(mm);
    free(ss);

    if(m_date){
        drawDate(m_screen.width()/2, 115, m_fgColor);
    }

    if(m_alarm){
        drawAlarm(m_screen.width()/2, 55, ST7735_RED);
    }
}

void ST7335_AD_ClockDisplay::drawAnalogClock(uint16_t color){
    cleanUpLines();
    if(m_date){
        drawDate(m_screen.width()/2, 100, m_fgColor);
    }
    if(m_alarm){
        drawAlarm(m_screen.width()/2, 55, ST7735_RED);
    }
    drawWatchIndicators(m_screen.width(), m_screen.height(), 62, color);
    drawWatchFace(m_screen.width(), m_screen.height(), 62, color);
}

void ST7335_AD_ClockDisplay::drawWatchFace(uint8_t width, uint8_t height, uint8_t radius, uint16_t color){
    // Outer circle
    m_screen.drawCircle(width/2, height/2, radius, color);
    
    // Hour ticks
    for(uint8_t i = 0; i < 360; i+=30){
        float angle = i;

        angle = (angle * 3.14159265359 / 180); // Convert degrees to radians
        uint8_t x = (width/2+(sin(angle)*radius));
        uint8_t y = (height/2-(cos(angle)*radius));
        uint8_t x2 = (width/2+(sin(angle)*(radius-10)));
        uint8_t y2 = (height/2-(cos(angle)*(radius-10)));
        
        // Hack for the misbehaving hour ticks
        if(i == 90){
            x -= 10;
            x2 -= 10;
        }       
        if(i == 180){
            y -= 10;
            y2 -= 10;
        }

        m_screen.drawLine(x, y, x2, y2, color);
    }
}

void ST7335_AD_ClockDisplay::drawWatchIndicators(uint8_t width, uint8_t height, uint8_t radius, uint16_t color){
    // Draw the hand that indicates seconds
    drawHand(width/2, height/2, (m_currentSecond*6), 50, 0, color);
    
    // Draw the hand that indicates minutes
    drawHand(width/2, height/2, (m_currentMinute*6), 40, 1, color);

    // Draw the hand that indicates hours
    drawHand(width/2, height/2, (m_currentHour*30), 30, 2, color);
}

void ST7335_AD_ClockDisplay::drawHand(uint8_t x, uint8_t y, float angle, uint8_t lenght, uint8_t index, uint16_t color){
    // Move the hour hand like you
    // would expect of an analog clock
    if(index == 2){
        angle += (m_currentMinute*6)/12;
        Serial.print("Angle Hour: ");
    }
    
    angle = (angle * 3.14159265359 / 180); // Convert degrees to radians
    if(index == 2){
        Serial.println(angle);
        Serial.print("Hour Value: ");
        Serial.println(m_currentHour);
    }
    uint8_t xDestination = (x+(sin(angle)*lenght));
    uint8_t yDestination = (y-(cos(angle)*lenght));
    
    // Handle limitations of drawLine
    if(isProblemArea(angle)){
        m_screen.drawLine(xDestination, yDestination, x, y, color);
    } else {
        m_screen.drawLine(x, y, xDestination, yDestination, color);
    }


    m_drawnLines[index].x = x;
    m_drawnLines[index].y = y;
    m_drawnLines[index].xDestination = xDestination;
    m_drawnLines[index].yDestination = yDestination;
    m_drawnLines[index].angle = angle;
}

void ST7335_AD_ClockDisplay::cleanUpLines(){
    // Clear the seconds hand
    if(isProblemArea(m_drawnLines[0].angle)){
        m_screen.drawLine(m_drawnLines[0].xDestination, m_drawnLines[0].yDestination, m_drawnLines[0].x, m_drawnLines[0].y, m_bgColor);
    } else {
        m_screen.drawLine(m_drawnLines[0].x, m_drawnLines[0].y, m_drawnLines[0].xDestination, m_drawnLines[0].yDestination, m_bgColor);
    }

    // Clear the minute hand
    if(m_lastMinute != m_currentMinute){
        if(isProblemArea(m_drawnLines[1].angle)){
            m_screen.drawLine(m_drawnLines[1].xDestination, m_drawnLines[1].yDestination, m_drawnLines[1].x, m_drawnLines[1].y, m_bgColor);
        } else {
            m_screen.drawLine(m_drawnLines[1].x, m_drawnLines[1].y, m_drawnLines[1].xDestination, m_drawnLines[1].yDestination, m_bgColor);
        }
    }

    // Clear the hour hand
    if(m_lastMinute != m_currentMinute){
        if(isProblemArea(m_drawnLines[2].angle)){
            m_screen.drawLine(m_drawnLines[2].xDestination, m_drawnLines[2].yDestination, m_drawnLines[2].x, m_drawnLines[2].y, m_bgColor);
        } else {
            m_screen.drawLine(m_drawnLines[2].x, m_drawnLines[2].y, m_drawnLines[2].xDestination, m_drawnLines[2].yDestination, m_bgColor);
        }
    }
}

void ST7335_AD_ClockDisplay::ForceCleanUpLines(){
    // Clear the seconds hand
    if(isProblemArea(m_drawnLines[0].angle)){
        m_screen.drawLine(m_drawnLines[0].xDestination, m_drawnLines[0].yDestination, m_drawnLines[0].x, m_drawnLines[0].y, m_bgColor);
    } else {
        m_screen.drawLine(m_drawnLines[0].x, m_drawnLines[0].y, m_drawnLines[0].xDestination, m_drawnLines[0].yDestination, m_bgColor);
    }

    // Clear the minute hand
    if(isProblemArea(m_drawnLines[1].angle)){
        m_screen.drawLine(m_drawnLines[1].xDestination, m_drawnLines[1].yDestination, m_drawnLines[1].x, m_drawnLines[1].y, m_bgColor);
    } else {
        m_screen.drawLine(m_drawnLines[1].x, m_drawnLines[1].y, m_drawnLines[1].xDestination, m_drawnLines[1].yDestination, m_bgColor);
    }

    // Clear the hour hand
    if(isProblemArea(m_drawnLines[2].angle)){
        m_screen.drawLine(m_drawnLines[2].xDestination, m_drawnLines[2].yDestination, m_drawnLines[2].x, m_drawnLines[2].y, m_bgColor);
    } else {
        m_screen.drawLine(m_drawnLines[2].x, m_drawnLines[2].y, m_drawnLines[2].xDestination, m_drawnLines[2].yDestination, m_bgColor);
    }
}

bool ST7335_AD_ClockDisplay::isProblemArea(float angle){
    return (angle >= 0.0f && angle < 0.1f) || (angle > 4.71f && angle < 4.72f) || (angle > 10.96f && angle < 11.04f);
}

void ST7335_AD_ClockDisplay::drawDate(uint8_t x, uint8_t y, uint16_t color){
    uint8_t currentDay = m_currentWeekday;

    if(m_lastWeekday != currentDay){
        m_screen.fillRect(x-30, y-5, 62, 10, m_bgColor);
        m_lastWeekday = currentDay;
    }
    if(m_lastDay != m_currentDay){
        m_screen.fillRect(x-30, y+5, 62, 10, m_bgColor);
        m_lastDay = m_currentDay;
    }

    switch(currentDay){
        case 1: 
            drawTextCentered(x, y, "Sunday", 1, color);
            break;
        case 2:
            drawTextCentered(x, y, "Monday", 1, color);
            break;
        case 3:
            drawTextCentered(x, y, "Tuesday", 1, color);
            break;
        case 4:
            drawTextCentered(x, y, "Wednesday", 1, color);
            break;
        case 5:
            drawTextCentered(x, y, "Thursday", 1, color);
            break;
        case 6:
            drawTextCentered(x, y, "Friday", 1, color);
            break;
        case 7:
            drawTextCentered(x, y, "Saturday", 1, color);
            break;
    } 

    char *dayText = (char*)calloc(3, sizeof(char));
    char *monthText = (char*)calloc(3, sizeof(char));
    char *yearText = (char*)calloc(5, sizeof(char));
    char *displayText = (char*)calloc(11, sizeof(char));

    const char *seperator = "/";

    itoa(m_currentDay, dayText, 10);
    itoa(month(m_timer), monthText, 10);
    itoa(year(m_timer), yearText, 10);

    if(m_currentDay < 10){
        strcat(displayText, "0"); 
    }
    strcat(displayText, dayText);
    strcat(displayText, seperator);
    if(month(m_timer) < 10){
        strcat(displayText, "0"); 
    }
    strcat(displayText, monthText);
    strcat(displayText, seperator);
    strcat(displayText, yearText);

    drawTextCentered(x, y+10, displayText, 1, color);
    
    // Free allocated memory
    free(dayText);
    free(monthText);
    free(yearText);
    free(displayText);
}

void ST7335_AD_ClockDisplay::drawAlarm(uint8_t x, uint8_t y, uint16_t color){
    const char *infoText = "Alarm: ";
    char *hourText = (char*)calloc(3, sizeof(char));
    itoa(m_alarmHour, hourText, 10);
    const char *seperator = ":";
    char *minuteText = (char*)calloc(3, sizeof(char));
    itoa(m_alarmMinute, minuteText, 10);
    char * m_alarmText = (char*)calloc(strlen(infoText)
                                + strlen(hourText)
                                + strlen(seperator)
                                + strlen(minuteText) + 3, sizeof(char));
    strcat(m_alarmText, infoText);
    if(m_alarmHour < 10){
        strcat(m_alarmText, "0");
    }
    strcat(m_alarmText, hourText);
    strcat(m_alarmText, seperator);
    if(m_alarmMinute < 10){
        strcat(m_alarmText, "0");
    }
    strcat(m_alarmText, minuteText);
    drawTextCentered(x, y, m_alarmText, 1, color);

    // Free allocated memory
    free(hourText);
    free(minuteText);
    free(m_alarmText);
}

void ST7335_AD_ClockDisplay::drawTextCentered(uint8_t x, uint8_t y, const char *string, uint8_t size, uint16_t color){
    if(string != NULL){
        m_screen.setTextColor(color);
        m_screen.setTextSize(size);
        m_screen.setCursor(x - (strlen(string)*3*size), y-(4*size));
        m_screen.print(string);
    }
}

void ST7335_AD_ClockDisplay::init(){
    // Use this initializer if you're using a 1.8" TFT
    m_screen.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

    m_screen.fillScreen(m_bgColor);
}

void ST7335_AD_ClockDisplay::setTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t month, uint8_t year){
    ::setTime(hour, minute, second, day, month, year);
    m_timer = now();
}

void ST7335_AD_ClockDisplay::drawClock(){
    // Update timers
    m_timer = now();
    m_currentSecond = second(m_timer);
    m_currentMinute = minute(m_timer);
    m_currentHour = hour(m_timer);
    m_currentDay = day(m_timer);
    m_currentWeekday = weekday(m_timer);

    // Draw each second
    if(m_currentSecond != m_lastSecond){
        if(m_analog){
            drawAnalogClock(m_fgColor);
        } else {
            drawDigitalClock(m_fgColor);
        }
    }

    // Update differential timers
    m_lastSecond = m_currentSecond;
    m_lastMinute = m_currentMinute;
    m_lastHour = m_currentHour;
    m_lastDay = m_currentDay;
    m_lastWeekday = m_currentWeekday;
}

void ST7335_AD_ClockDisplay::redraw(){
    if(m_analog){
        drawDigitalClock(m_bgColor);
        // Erase date
        m_screen.fillRect(32, 110, 62, 10, m_bgColor);
        m_screen.fillRect(32, 120, 62, 10, m_bgColor);
        // Erase alarm
        m_screen.fillRect(25, 50, 80, 10, m_bgColor);
        drawAnalogClock(m_fgColor);
    } else {
        drawAnalogClock(m_bgColor);
        ForceCleanUpLines();
        // Erase date
        m_screen.fillRect(32, 95, 62, 10, m_bgColor);
        m_screen.fillRect(32, 105, 62, 10, m_bgColor);
        // Erase alarm
        m_screen.fillRect(25, 50, 80, 10, m_bgColor);
        drawDigitalClock(m_fgColor);
    }
}

void ST7335_AD_ClockDisplay::showAlarm(){
    m_alarm = true;
    if(m_analog){
        drawAlarm(64, 55, ST7735_RED);
    } else {
        drawAlarm(64, 55, ST7735_RED);
    }
}
void ST7335_AD_ClockDisplay::hideAlarm(){
    m_alarm = false;
    m_screen.fillRect(25, 50, 80, 10, m_bgColor);
    redraw();
}
void ST7335_AD_ClockDisplay::showDate(){
    m_date = true;
    if(m_analog){
        drawDate(64, 100, m_fgColor);
    } else {
        drawDate(64, 115, m_fgColor);
    }
}

void ST7335_AD_ClockDisplay::hideDate(){
    m_date = false;
    if(m_analog){
        m_screen.fillRect(32, 95, 62, 10, m_bgColor);
        m_screen.fillRect(32, 105, 62, 10, m_bgColor);
    } else {
        m_screen.fillRect(32, 110, 62, 10, m_bgColor);
        m_screen.fillRect(32, 120, 62, 10, m_bgColor);
    }
    redraw();
}

void ST7335_AD_ClockDisplay::incrementFgColor(){
    fgIndex++;
    if(fgIndex > 7){
        fgIndex = 0;
    }
    m_fgColor = m_colors[fgIndex];
    if(m_analog){
        drawAnalogClock(m_fgColor);
    } else {
        drawDigitalClock(m_fgColor);
    }
}

void ST7335_AD_ClockDisplay::incrementBgColor(){
    bgIndex++;
    if(bgIndex > 7){
        bgIndex = 0;
    }
    m_bgColor = m_colors[bgIndex];
    m_screen.fillScreen(m_bgColor);
    if(m_analog){
        drawAnalogClock(m_fgColor);
    } else {
        drawDigitalClock(m_fgColor);
    }
}
