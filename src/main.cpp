#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

#define NUMBER_OF_BYTE 23
char line1[16], line2[16];
unsigned char buffer [NUMBER_OF_BYTE];
int PM25 = 0, PM10 = 0;
int PM1, PM1a, PM25a, PM10a;

SoftwareSerial pmsSerial(2, 3);
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define relay 5

int pm_high = 100;
int pm_low = 80;

void read_data(void);
bool checkValue(unsigned char *buf, int length);
void compare(void);

void setup() {
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  Serial.begin(9600);
  pmsSerial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.print("     Welcome    ");
  delay(5000);
  lcd.clear();
}

void loop() {

  read_data();
  //delay(500);

} 

void compare() {
  if (PM25 > pm_high) {
    digitalWrite(relay, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  if (PM25 < pm_low) {
    digitalWrite(relay, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
  }
}

bool checkValue(unsigned char *buf, int length)
{
  bool flag = 0;
  int sum = 0;

  for (int i = 0; i < (length - 2); i++)
  {
    sum += buf[i];
  }
  sum = sum + 0x42;

  if (sum == ((buf[length - 2] << 8) + buf[length - 1]))
  {
    sum = 0;
    flag = 1;
  }
  return flag;
}

void read_data() {
  char fel = 0x42;
  if (pmsSerial.find(&fel, 1)) {
    pmsSerial.readBytes(buffer, NUMBER_OF_BYTE);
  }

  if (buffer[0] == 0x4d)
  {
    if (checkValue(buffer, NUMBER_OF_BYTE))
    {
      PM25 = ((buffer[5] << 8) + buffer[6]);
      PM10 = ((buffer[7] << 8) + buffer[8]);

      // rest of values (if you want to use it)
      PM1 = ((buffer[3] << 8) + buffer[4]);
      PM1a = ((buffer[9] << 8) + buffer[10]);
      PM25a = ((buffer[11] << 8) + buffer[12]);
      PM10a = ((buffer[13] << 8) + buffer[14]);

    /*  Serial.print("pm1 atmosphere: "); Serial.println(PM1a);
      Serial.print("pm2.5 atmosphere: "); Serial.println(PM25a);
      Serial.print("pm10 atmosphere: "); Serial.println(PM10a);
      Serial.println();
      Serial.print("pm1: "); Serial.println(PM1);
      Serial.print("pm2.5: "); Serial.println(PM25);
      Serial.print("pm10: "); Serial.println(PM10);
      Serial.println();
      Serial.println();*/

      Serial.print("PM 2.5 : ");
      Serial.print(PM25);
      Serial.println(" ug/m^3");

      lcd.setCursor(0,0);
      lcd.print("     PM 2.5     ");
      lcd.setCursor(0,1);
      lcd.print("  ");
      if(PM25 < 100) {
        lcd.print(" ");
      }
      lcd.print(PM25);
      lcd.print(" ug/m^3 ");

      compare();
    }
  }
}