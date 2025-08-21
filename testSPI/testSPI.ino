#include <SPI.h>

// NEW pin definition (CS changed explicitly to GPIO15)
#define CC2500_CS   4  
#define CC2500_SCK  18
#define CC2500_MISO 19
#define CC2500_MOSI 23

uint8_t readReg(uint8_t addr) {
  digitalWrite(CC2500_CS, LOW);
  SPI.transfer(addr | 0x80);  // Read command
  uint8_t val = SPI.transfer(0x00);
  digitalWrite(CC2500_CS, HIGH);
  return val;
}

void setup() {
  Serial.begin(115200);

  // Set up new Chip Select pin explicitly
  pinMode(CC2500_CS, OUTPUT);
  digitalWrite(CC2500_CS, HIGH);

  // SPI Initialization with new CS pin at 1MHz explicitly
  SPI.begin(CC2500_SCK, CC2500_MISO, CC2500_MOSI, CC2500_CS);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  
  delay(500);

  // Read from known register (FREQ0: 0x0F)
  uint8_t val = readReg(0x0F);

  Serial.print("SPI TEST RESULT - FREQ0 = 0x");
  Serial.println(val, HEX);

  SPI.endTransaction();
}

void loop() {}