#include <SPI.h>
#include <SD.h>

File fd;
char fname[] = "test.txt";
const uint8_t sd_chipsel = 10;

uint16_t testdataout[128];
uint16_t testdatain[128];

void setup() 
{
  // put your setup code here, to run once:

  delay(1500);

  Serial.begin(115200);
  Serial.println("Setup.");

  for(int32_t i = 0; i < 128; i++)
  {
    testdataout[i] = i;
  }

  Serial.print("Testdata sizeof: ");
  Serial.println(sizeof(testdataout));

  // config SPI to alternate pins to avoid I2S
  SPI.setSCK(14);
  SPI.setMOSI(7);
  
  pinMode(sd_chipsel, OUTPUT);
  digitalWrite(sd_chipsel, HIGH);



  Serial.println("Start card init.");

  if(!SD.begin(sd_chipsel))
  {
    Serial.println("** SD.begin failed");
    goto BAIL;
  }

  Serial.println("begin complete");

  if(SD.exists(fname))
  {
    Serial.println("File Exists.");
  }

  Serial.print("testdata:");
  for(uint32_t i = 0; i < 128; i++)
  {
    Serial.print(testdataout[i],HEX);
  }
  Serial.println();

  
  fd = SD.open(fname, FILE_WRITE);

  if(!fd)
  {
    Serial.println("** no FD?");
    goto BAIL;
  }

  fd.seek(0);
  // seek returns something??

  Serial.print("Writing...");

  fd.write((uint8_t*)testdataout, sizeof(testdataout));

  Serial.println();

  fd.flush();
  fd.close();
  
  

BAIL:

  Serial.println("Setup complete.");
}

void loop() 
{
  static bool didit = false;
  char c;

  int ic;

  if(!didit)
  {
    Serial.println("Made it to Loop.");
    didit = true;

    fd = SD.open(fname, FILE_READ);

    if(!fd)
    {
      Serial.println("Couldn't open in read mode...");
      return;
    }

    fd.seek(0);

    Serial.println("Reading file");

    if(fd.available() == sizeof(testdataout))
    {
      uint8_t* dataptr = (uint8_t*)testdatain;
      
      for(uint32_t i = 0; i < sizeof(testdataout); i++)
      {
        dataptr[i] = fd.read();
      }
    }
    else
    {
      Serial.println("** read size mismatch");
    }

    Serial.print("readback :");
    for(uint32_t i = 0; i < 128; i++)
    {
      Serial.print(testdatain[i],HEX);
    }
      
    
    Serial.println();

    fd.close();

    Serial.println("first loop complete...");
  }
  
}
