# 💾BILLING-MACHINE-BASED-DATA-LOGGING-SYSTEM

This embedded system project demonstrates interfacing of an SD card with a microcontroller using the FAT file system to create folders and text files, log data with real-time timestamps using RTC, and integrate a billing machine using an LCD display and keypad.

## 📌 Key Features

- ✅ *SD Card Initialization* using SPI protocol
- 🗂️ *Folder & Text File Creation* with FAT file system (FATFS)
- ⏰ *RTC (Real Time Clock) Integration* for timestamping file entries
- 🧾 *Billing Machine Functionality* using 4x4 Keypad and 16x2 LCD
- 💬 *Real-time Data Logging* of billing information onto SD card

## 🔧 Hardware Components Used

- LPC1768 Microcontroller
- SD Card Module (SPI Interface)
- 4x4 Matrix Keypad
- 16x2 LCD Display (I2C or Parallel)
- DS1307 RTC Module
- Level Shifter (if required for SD card)

## 🔌 Circuit Connections (Overview)

### SD Card (SPI)
- MOSI – P0.9  
- MISO – P0.8  
- SCK  – P0.7  
- CS   – P0.6  

### RTC DS1307 (I2C)
- SDA – P0.27  
- SCL – P0.28  

### LCD Display (I2C)
- SDA – Connected with RTC SDA (P0.27)  
- SCL – Connected with RTC SCL (P0.28)  

### Keypad
- Connected to GPIOs (e.g., P1.18–P1.25)  
- Rows to inputs, Columns to outputs


## 🧠 How It Works

1. *SD Card Initialization*: Initializes using SPI mode with FATFS library.
2. *Folder/File Management*: Creates a folder for the day (using RTC date) and writes logs in a text file.
3. *RTC Timestamping*: Each entry in the file is tagged with real-time data from RTC.
4. *Billing Interface*: User inputs item prices through keypad; total is calculated and shown on LCD.
5. *Data Logging*: Each item, cost, and total bill is saved into the text file in SD card.
## Demo
<img width="517" height="610" alt="image" src="https://github.com/user-attachments/assets/34f11e82-992a-4b3a-ab83-3e7d096f1a25" />
<img width="1010" height="536" alt="image" src="https://github.com/user-attachments/assets/e140c790-c577-4b9b-bbb1-07b8e07379c2" />

## 📂 Project Structure

📁 SD_Card_Billing_Project ┣ 📁 src/ ┃ ┗ main.c ┣ 📁 inc/ ┃ ┗ header.h ┣ 📁 fatfs/ ┃ ┣ ff.c, ff.h ┃ ┗ diskio.c, diskio.h ┣ 📁 rtc/ ┃ ┗ rtc.c, rtc.h ┣ README.md

## 📈 Applications

- Mini POS (Point of Sale) systems
- Data logging systems for offline embedded devices
- RTC-based file management in embedded storage systems

## 🛠️ Tools Used

- Keil uVision
- FlashMagic for flashing
- Qcom(for UART)
- Git & GitHub for version control

## 🤝 Contributors

- 👩‍💻 *Name*: Mallam Deekshitha – Final Year EEE Student  
  Embedded Systems Enthusiast | Aspiring Automotive Embedded Engineer

---
✨ Contributions, suggestions, and improvements are welcome!
⭐ If you liked this project, *star* the repo and feel free to fork it!
