#include <SPI.h>

static const int32_t CHIPSEL = 8;

static const SPISettings registersettings(1000000, LSBFIRST, SPI_MODE0 );

uint32_t next;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  SPI.setSCK(14);
  SPI.begin();
  
  pinMode(CHIPSEL, OUTPUT);
  digitalWrite(CHIPSEL, LOW);

  next = millis() + 1000;
  
  Serial.println("Setup complete.");
}

uint16_t current = 0;

void loop() {
  // put your main code here, to run repeatedly:

  uint8_t dataout[3];
  //int8_t datain[3];

  if(millis() > next)
  {

    next = millis()+500;

    Serial.println(current);

    if(!(current & 0x8))
    {
      dataout[0] = 1 << (current & 0x07);
      dataout[1] = 0;
    }
    else
    {
      dataout[0] = 0;
      dataout[1] = 1 << (current & 0x07);
    }

    SPI.beginTransaction(registersettings);


    SPI.transfer(dataout, 2);

    digitalWrite(CHIPSEL, HIGH);
    digitalWrite(CHIPSEL, LOW);

    SPI.endTransaction();

    current++;

//    Serial.print(dataout[0], HEX);
//    Serial.print(dataout[1], HEX);
//    Serial.println(dataout[0], HEX);

  }
  

}
