#include <SPI.h>

static const int32_t CHIPSEL_BTNS = 8;
static const int32_t CHIPSEL_LEDS = 5;

static const SPISettings registersettings(500000, MSBFIRST, SPI_MODE0 );

uint32_t next;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  SPI.setSCK(14);
  SPI.begin();
  
  pinMode(CHIPSEL_BTNS, OUTPUT);
  digitalWrite(CHIPSEL_BTNS, LOW);
  pinMode(CHIPSEL_LEDS, OUTPUT);
  digitalWrite(CHIPSEL_LEDS, LOW);

  next = millis() + 1000;
  
  Serial.println("Setup complete.");
}

uint16_t current = 0;

void loop() {
  // put your main code here, to run repeatedly:

  static uint32_t dataout;
  //int8_t datain[3];

  if(millis() > next)
  {

    next = millis()+10;

    //Serial.println(current);

    //dataout = 1 << (current % 8);

    SPI.beginTransaction(registersettings);

    digitalWrite(CHIPSEL_BTNS, HIGH);

    SPI.transfer(&dataout, 3);

    digitalWrite(CHIPSEL_LEDS, HIGH);
    digitalWrite(CHIPSEL_BTNS, LOW);
    digitalWrite(CHIPSEL_LEDS, LOW);

    SPI.endTransaction();

    current++;

    dataout = ~dataout;

    //Serial.print(dataout[0], HEX);
    //Serial.print(dataout[1], HEX);
    Serial.println(dataout, HEX);

  }
  

}
