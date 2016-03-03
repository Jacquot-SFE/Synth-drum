static const uint8_t NUM_POTS = 16;

uint16_t last_seen_val[NUM_POTS];

static const uint32_t analog_pin_lut[NUM_POTS] = {A1, A2, A3, A6, A7, A10, A11, A12, A13, A14, A16, A17, A18, A19, A20, A15};

uint32_t next;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  for(uint8_t i = 0; i < NUM_POTS; i++)
  {
    last_seen_val[i] = analogRead(analog_pin_lut[i]);
  }

  //pinMode(13, OUTPUT); // LED pin
  //pinMode(15, INPUT); // Volume pot pin?

  next = millis()+1000;
  
  analogReadResolution(8);

  Serial.println("Setup complete.");
}

void loop() {
  // put your main code here, to run repeatedly:

    uint16_t current;

  if(millis() > next)
  {
    
    next = millis()+10;

    for(uint8_t i = 0; i < NUM_POTS; i++)
    {
      current = analogRead(analog_pin_lut[i]);
      
      if(current != last_seen_val[i])
      {
        last_seen_val[i] = current;

        Serial.print(i);
        Serial.print(" changed to 0x");
        Serial.println(current, HEX);
      }
    }
  }
  

}
