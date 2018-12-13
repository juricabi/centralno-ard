
//Pametna kuca, Jurica Bilicic

#define DHT_PIN 10
#include <dht.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

dht DHT;

OneWire DS1(9);
OneWire DS2(8);

DallasTemperature sensor1(&DS1);
DallasTemperature sensor2(&DS2);

DeviceAddress DS18B20n1 = {0x28, 0x22, 0xAB, 0xB4, 0x06, 0x00, 0x00, 0xCB};//adresa DS18B20
DeviceAddress DS18B20n2 = {0x28, 0xFF, 0x46, 0x3F, 0x2D, 0x04, 0x00, 0x50};//adresa DS18B20 drugog

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);//definiraj pinove lcd-a


float namtemp = 55, tmpnamtemp = 0, tempkot = 0, tempkotold = 25 , tempboj = 0, tempbojold = 25;
unsigned long previousMillis = 0, greske = 0;
short pumpa = 0, brojacispis = 1, brojacpumpa = 0;


void setup()
{

	Serial.begin(115200);//Brzina prijenosa bluetooth modula
	pinMode(3, INPUT);
	pinMode(4, INPUT);
	pinMode(2, OUTPUT);
	digitalWrite(2, HIGH);
	sensor1.begin();
  sensor2.begin();
	sensor1.setResolution(DS18B20n1, 10);
  sensor2.setResolution(DS18B20n2, 10);
  
	EEPROM.get(0, tmpnamtemp);

	if (tmpnamtemp >= 20 && tmpnamtemp <= 120)
		namtemp = tmpnamtemp;

	lcd.begin(16, 2);
	lcd.off();
	lcd.begin(16, 2);

}


void loop() //Program se izvr�ava u beskona�noj petlji.
{


	if (Serial.available()) //ako se na serjskom portu nalazi podatak udi u switch
	{

		switch (Serial.read())
		{

		case 10:
		{
			if (namtemp >= 120)
			{
				Serial.print("Namjestena temperatura prevelika!\n");
				namtemp = 120;
				EEPROM.put(0, namtemp);
			}

			else
			{
				namtemp += 0.5;
				EEPROM.put(0, namtemp);
				Serial.print("Namjestena temperatura povecana i iznosi: ");
				Serial.print(namtemp, 2);
				Serial.print("C\n");
			}
			break;
		}

		case 11:
		{

			if (namtemp <= 20) {
				Serial.print("Namjestena temperatura preniska!\n");
				namtemp = 20;
				EEPROM.put(0, namtemp);
			}

			else
			{
				namtemp -= 0.5;
				EEPROM.put(0, namtemp);
				Serial.print("Namjestena temperatura snizena i iznosi: ");
				Serial.print(namtemp, 2);
				Serial.print("C\n");
			}
			break;

		}
		case 12:
		{
			Serial.print("Izvjestaj\nNamjestena temperatura: ");
			Serial.print(namtemp, 2);
			Serial.print("C\nTemeratura kotla: ");
			Serial.print(tempkot);
      Serial.print("C\nTemeratura bojlera: ");
      Serial.print(tempboj);
			Serial.print("C\nTemeratura u kuci: ");
			Serial.print(DHT.temperature, 1);
			Serial.print("C\nVlaga u kuci: ");
			Serial.print(DHT.humidity, 1);
			Serial.print("%\n");

			if (pumpa == 1) {
				Serial.print("Pumpa upaljena!\n");
			}

			else if (pumpa == 0) {
				Serial.print("Pumpa ugasena!\n");
			}
			break;

		}
		case 13:
		{
			Serial.print("Greske citanja: ");
			Serial.print(greske);
			Serial.println();
			break;

		}

		default:
		{
			Serial.read();
			Serial.print("Nepoznati unos!\n");
			break;
		}

		}


	}

	if ((unsigned long)(millis() - previousMillis) >= 1200)  //jednostavni timer za svaku sekundu

	{

		//Serial.println(millis()); 
		//Serial.println(previousMillis);

		previousMillis = millis();
		sensor1.requestTemperatures();
		tempkot = sensor1.getTempC(DS18B20n1);

		if (tempkot == -127.00)
		{
			tempkot = tempkotold;
			greske++;
		}

		else
		{
			tempkotold = tempkot;
		}


    sensor2.requestTemperatures();
    tempboj = sensor2.getTempC(DS18B20n2);

    if (tempboj == -127.00)
    {
      tempboj = tempbojold;
      greske++;
    }

    else
    {
      tempbojold = tempboj;
    }
   


		if (brojacispis <= 2)
		{

			lcd.setCursor(0, 1);
			lcd.print("                ");
			lcd.setCursor(0, 1);
			lcd.print("Kotao ");
			lcd.print(tempkot);
			lcd.write(223);
			lcd.print("C ");
			brojacispis++;
		}


     else if (brojacispis > 2 && brojacispis <= 4)
    {

      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("Bojler ");
      lcd.print(tempboj);
      lcd.write(223);
      lcd.print("C ");
      brojacispis++;
    }

		else if (brojacispis > 4 && (DHT.read22(DHT_PIN) == DHTLIB_OK || DHT.read11(DHT_PIN) == DHTLIB_OK))
		{
			
			lcd.setCursor(0, 1);
			lcd.print("                ");
			lcd.setCursor(0, 1);
			lcd.print("Kuca ");
			lcd.print(DHT.temperature, 0);
			lcd.write(223);
			lcd.print("C ");
			lcd.print(DHT.humidity, 0);
      brojacispis++;

			lcd.print("%");
		}


			else 
			{
				lcd.setCursor(0, 1);
				lcd.print("                ");
				lcd.setCursor(0, 1);
				lcd.print("DHT22 greska !");
				brojacispis++;
			}


		if (brojacispis > 6)
		{
			//Serial.println(brojacispis);
			brojacispis = 0;
		}



		if ((digitalRead(3) == HIGH)) {

			if (namtemp >= 120)
			{
				lcd.setCursor(0, 0);
				namtemp = 120;
				EEPROM.put(0, namtemp);
				lcd.print("                ");
				lcd.setCursor(0, 0);
				lcd.print("Postavi nize!");
			}

			else
			{
				namtemp += 0.5;
				EEPROM.put(0, namtemp);
				lcd.print("                ");
				lcd.setCursor(0, 0);
				lcd.print("Nam.Temp ");
				lcd.print(namtemp, 1);
				lcd.write(223);
				lcd.print("C");
			}

		}

		else if ((digitalRead(4) == HIGH)) {

			if (namtemp <= 20)
			{
				lcd.setCursor(0, 0);
				namtemp = 20;
				EEPROM.put(0, namtemp);
				lcd.print("                ");
				lcd.setCursor(0, 0);
				lcd.print("Postavi vise!");
			}

			else
			{
				namtemp -= 0.5;
				EEPROM.put(0, namtemp);
				lcd.print("                ");
				lcd.setCursor(0, 0);
				lcd.print("Nam.Temp ");
				lcd.print(namtemp, 1);
				lcd.write(223);
				lcd.print("C");

			}


		}

		else 
		{
			lcd.setCursor(0, 0);
			lcd.print("Nam.Temp ");
			lcd.print(namtemp, 1);
			lcd.write(223);
			lcd.print("C");
		}



		if (tempkot >= namtemp && brojacpumpa == 0)
		{
			digitalWrite(2, LOW);
			pumpa = 1;
			brojacpumpa = 1;
		}

		else if (tempkot < namtemp && brojacpumpa == 0)
		{
			digitalWrite(2, HIGH);
			pumpa = 0;
			brojacpumpa = 1;
		}


		if (brojacpumpa > 0 && brojacpumpa < 60) {
			brojacpumpa++;
		}

		else {
			brojacpumpa = 0;
		}

	}

}
