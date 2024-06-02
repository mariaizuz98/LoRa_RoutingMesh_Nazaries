#include "lcd.h"

extern SSD1306 display;

extern byte localID;
extern routeTableEntry routeTable;

void representLCD_Node(void){

    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "LORA NODE");
    display.drawString(0, 12, "ID: 0x" + String(localID, HEX));
    display.drawString(0, 24, routeTable.sequenceRoute);
    display.display();

}

void representLCD_Gateway(void){

    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "LORA GATEWAY");
    display.drawString(0, 12, "ID: 0x" + String(localID, HEX));
    display.display();

}