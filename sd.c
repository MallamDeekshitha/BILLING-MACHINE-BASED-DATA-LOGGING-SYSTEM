#include "ff.h"
#include "diskio.h"
#include <string.h>
#include "LPC17xx.h"
#include <stdio.h>

#define ROWS 0x0F
#define COLUMNS 0xF0
#define OKSWITCH (1<<8)
#define RESET (1<<9)
#define BACK (1<<10)
#define PRINT (1<<11)    // P2.11
#define CANCEL (1<<12)   // P2.12

// LCD control pins
#define RS (1<<10)
#define RW (1<<9)
#define ENABLE (1<<11)
#define DATA 0x780000

int total = 0;

char keys[4][4]={
    {'1','2','3','4'},
    {'5','6','7','8'},
    {'9',':',';','<'},
    {'=','>','?','@'}
};

char *items[16] = {
    "Idly-10", "Vada-10", "Garay-15", "Plain Dosa-20",
    "Sambar Idly-20", "Sambar Vada-20", "Upma-20", "Onion Dosa-30",
    "Masala Dosa-30", "Ghee Dosa-30", "Mushroom Dosa-40", "Kaju Dosa-40",
    "Peanut Dosa-40", "Corn Dosa-40", "GheeOnion Dosa-50", "GheeCorn Dosa-50"
};

int price[16] = {10,10,15,20,20,20,20,30,30,30,40,40,40,40,50,50};

// File system variables
FATFS fs;
FIL file;
FRESULT res;
UINT bytesWritten, bytesRead;
char readBuffer[128];

// GPIO Initialization
void initGPIO() {
    LPC_GPIO2->FIODIR |= ROWS;
    LPC_GPIO2->FIODIR &= ~(COLUMNS | OKSWITCH);
    LPC_GPIO2->FIODIR &= ~(PRINT | CANCEL);

    LPC_GPIO0->FIODIR |= RS | RW | ENABLE | DATA;
    LPC_GPIO0->FIOCLR = RS | RW | ENABLE | DATA;
}

// Timer delay in ms
void delay(int ms) {
    LPC_TIM0->PR = 999;
    LPC_TIM0->TCR = 0x02;
    LPC_TIM0->MR0 = 25 * ms - 1;
    LPC_TIM0->TCR = 0x01;
    while (LPC_TIM0->TC < LPC_TIM0->MR0);
    LPC_TIM0->TCR = 0x02;
}

// Keypad scanning
char keypadScan() {
    for (int i = 0; i < 4; i++) {
        LPC_GPIO2->FIOSET = ROWS;
        LPC_GPIO2->FIOCLR = (1 << i);
        for (int j = 0; j < 4; j++) {
            if (!(LPC_GPIO2->FIOPIN & (1 << (j + 4)))) {
                while (!(LPC_GPIO2->FIOPIN & (1 << (j + 4))));
                return keys[i][j];
            }
        }
        LPC_GPIO2->FIOSET = (1 << i);
    }
    return '\0';
}

// LCD data/command transfer
void commandOrData(uint8_t addr, uint8_t rs) {
    if (rs == 0)
        LPC_GPIO0->FIOCLR = RS;
    else
        LPC_GPIO0->FIOSET = RS;

    uint8_t upperNibble = (addr & 0xF0) >> 4;
    uint8_t lowerNibble = (addr & 0x0F);

    LPC_GPIO0->FIOCLR = DATA;
    LPC_GPIO0->FIOSET = upperNibble << 19;
    LPC_GPIO0->FIOSET = ENABLE;
    delay(20);
    LPC_GPIO0->FIOCLR = ENABLE;

    LPC_GPIO0->FIOCLR = DATA;
    LPC_GPIO0->FIOSET = lowerNibble << 19;
    LPC_GPIO0->FIOSET = ENABLE;
    delay(20);
    LPC_GPIO0->FIOCLR = ENABLE;
}

// LCD initialization
void initLCD() {
    delay(40);
    commandOrData(0x02, 0);
    commandOrData(0x28, 0);
    commandOrData(0x0C, 0);
    commandOrData(0x06, 0);
    commandOrData(0x01, 0);
}

// Send string to LCD
void data(char *str) {
    while (*str) {
        commandOrData(*str++, 1);
    }
}

// Add price to total
int totalAmount(uint8_t item) {
    total += item;
    return total;
}

// File write and read
void write_file_to_sd(void) {
    char log_entry[128];
    sprintf(log_entry, "%02d-%02d-%04d %02d:%02d:%02d -> Total Amount = %d\r\n",
            LPC_RTC->DOM, LPC_RTC->MONTH, LPC_RTC->YEAR,
            LPC_RTC->HOUR, LPC_RTC->MIN, LPC_RTC->SEC, total);

    res = f_open(&file, "/Bills/log.txt", FA_WRITE | FA_OPEN_APPEND);
    if (res == FR_OK) {
        res = f_write(&file, log_entry, strlen(log_entry), &bytesWritten);
        if (res == FR_OK && bytesWritten > 0) {
            UART0_Print("File written successfully\r\n");
        }
        f_close(&file);
    } else {
        UART0_Print("Failed to open file for writing\r\n");
    }

    // Read back file
    res = f_open(&file, "/Bills/log.txt", FA_READ);
    if (res == FR_OK) {
        res = f_read(&file, readBuffer, sizeof(readBuffer) - 1, &bytesRead);
        if (res == FR_OK && bytesRead > 0) {
            readBuffer[bytesRead] = '\0';
            UART0_Print("File read:\r\n");
            UART0_Print(readBuffer);
        }
        f_close(&file);
    } else {
        UART0_Print("Failed to open file for reading\r\n");
    }
}

int main() {
    initGPIO();
    initLCD();
    RTC_Init();
    UART0_Init();

    int amount = 0;
    int itemPrice = 0;
    char buffer[40];

    // Mount file system
    res = f_mount(&fs, "", 1);
    if (res != FR_OK) {
        UART0_Print("Mount failed\r\n");
        return 0;
    }
    UART0_Print("Mounted successfully\r\n");

    // Create folder
    res = f_mkdir("/Bills");
    if (res == FR_OK)
        UART0_Print("Folder created successfully\r\n");
    else if (res == FR_EXIST)
        UART0_Print("Folder already exists\r\n");
    else
        UART0_Print("Failed to create folder\r\n");

    data("Billing Machine ");
    UART0_Print("Billing Machine is Ready\n");

    while (1) {
        char n = keypadScan();
        if (n != '\0') {
            commandOrData(0x01, 0); // Clear LCD
            char *item = items[n - '1'];
            itemPrice = price[n - '1'];
            amount = totalAmount(itemPrice);
            sprintf(buffer, "%s\n", item);
            data(buffer);
            UART0_Print(buffer);
            delay(1000);
        }

        // PRINT button
        if (!(LPC_GPIO2->FIOPIN & PRINT)) {
            while (!(LPC_GPIO2->FIOPIN & PRINT));
            commandOrData(0x01, 0);
            sprintf(buffer, "Total = Rs.%d\n", amount);
            data(buffer);
            UART0_Print(buffer);
            write_file_to_sd();
            amount = 0;
            total = 0;
            commandOrData(0x01, 0);
            data("Ready for new bill");
            delay(500);
        }

        // CANCEL button
        if (!(LPC_GPIO2->FIOPIN & CANCEL)) {
            while (!(LPC_GPIO2->FIOPIN & CANCEL));
            commandOrData(0x01, 0);
            data("Bill Cancelled");
            amount = 0;
            total = 0;
            delay(1000);
            commandOrData(0x01, 0);
            data("Ready for new bill");
            delay(500);
        }
    }
}
