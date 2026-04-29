
#include <Wire.h> // Or your MCU I2C library

#define LIS2DH12_ADDR    0x19 // STEVAL-MKI151v1 default I2C address
#define CTRL_REG1        0x20
#define CTRL_REG4        0x23
#define STATUS_REG    0x27 // Status register address
#define OUT_X_L          0x28

int16_t data,x,y,z;
char dataBuffer[32];

void setup() {
    Serial.begin(115200);  //921600
    delay(1000);  //was  while(!Serial); 
    
    // 2. Init LIS2DJ12 MEMS Accelerometer eval module   
    lis2dh12_init();
}

void loop() {
    // read and print the accelerometer outputs
    read_accelerometer_fsm(&x, &y, &z);
    sprintf(dataBuffer, "%d, %d, %d\n", x, y, z);
    Serial.println(dataBuffer);
}

void lis2dh12_init() {
    Wire.begin();
  
  // 1. Enable all axes, set 100Hz Normal Mode (ODR 0101)
    Wire.beginTransmission(LIS2DH12_ADDR);
    Wire.write(CTRL_REG1);
    Wire.write(0x57); // 0101 0111 (100Hz, normal, XYZ enabled)
    Wire.endTransmission();
  
  // 2. Set +/- 2g range, High-Resolution mode enabled
    Wire.beginTransmission(LIS2DH12_ADDR);
    Wire.write(CTRL_REG4);
    Wire.write(0x08); // 0000 1000
    Wire.endTransmission();
    Serial.println("LIS2DH12 Initialized");
}


void read_accelerometer_fsm(int16_t *x, int16_t *y, int16_t *z) {
    uint8_t status = 0;

    // --- Steps 1 & 2: Read STATUS_REG and poll Bit 3 (ZYXDA) ---
    // ZYXDA = 1 means new set of data is available
    do {
        Wire.beginTransmission(LIS2DH12_ADDR);
        Wire.write(STATUS_REG);
        Wire.endTransmission(false); // Repeated start
        Wire.requestFrom(LIS2DH12_ADDR, 1);
        if (Wire.available()) {
            status = Wire.read();
        }
    } while (!(status & 0x08)); // Loop until Bit 3 is high

    // --- Step 3: Check Bit 7 (ZYXOR) for Data Overrun ---
    if (status & 0x80) {
        Serial.println("Warning: Data Overwritten!");
    }

    // --- Steps 4-9: Read XYZ Data (6 bytes) ---
    // Using 0x80 to enable auto-increment for multi-byte reads
    Wire.beginTransmission(LIS2DH12_ADDR);
    Wire.write(OUT_X_L | 0x80); 
    Wire.endTransmission(false);
    Wire.requestFrom(LIS2DH12_ADDR, 6);

    if (Wire.available() >= 6) {
        uint8_t xl = Wire.read(); uint8_t xh = Wire.read();
        uint8_t yl = Wire.read(); uint8_t yh = Wire.read();
        uint8_t zl = Wire.read(); uint8_t zh = Wire.read();

        // Step 10: Data Processing (Combine bytes)
        *x = (int16_t)((xh << 8) | xl);
        *y = (int16_t)((yh << 8) | yl);
        *z = (int16_t)((zh << 8) | zl);
    } 
}
