/******************************
 *          Libraries         *
 ******************************/
#include <DHT.h>     // DHT22 library
#include <Nextion.h> // NextionDisplay library

/*****************************
 *        Constants          *
 *****************************/
#define DHTPIN 2          // DHT22 PIN
#define DHTTYPE DHT22     // DHT 22  (AM2302)
#define RED_LED 6         // RED_LED pin 6
#define GREEN_LED 7       // GREEN_LED pin 7
#define BLUE_LED 5        // BLUE_LED pin 5
#define FAN 4             // FAN pin 4
#define SOIL 1            // SOIL MOISTURE pin 1
#define PUMP 8            // PUMP pin 8
#define LDR_PIN 0         // PHOTORESISTOR pin 0
#define HEATER 9          // HEATER pin 9
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor for normal 16mhz Arduino

/*****************************
 *        Variables          *
 *****************************/

int luminTreshold = 60; //Variable luminosity treshold
int soilTreshold = 31;  //Variable soil moisture treshold
int temp_max = 30;       //Variable min temperature
int temp_min = 22;       //Variable min temperature
int humTreshold = 62;    //Variable humidity threshold
float hum;               //Variable humidity value
float temp;              //Variable temperature value
float soil_mst;          //Variable soil moisture value
float light;             //Variable photoresistor value

/*************************************************************************
 *          Declaration of objects that will be read from the display    *
 *************************************************************************/

NexButton b0 = NexButton(0, 8, "b0");
NexButton b1 = NexButton(0, 9, "b1");
NexButton b2 = NexButton(0, 10, "b2");
NexButton b3 = NexButton(0, 11, "b3");
NexButton b4 = NexButton(0, 13, "b4");
NexButton b5 = NexButton(0, 14, "b5");

/*************************************************************************
 *                           Touch event list                            *
 *      Declared each touch event object name to the touch event list    *
 *************************************************************************/

NexTouch *nex_listen_list[] =
    {
        &b0,
        &b1,
        &b2,
        &b3,
        &b4,
        &b5};

/********************************************************
 *                 Touch events                         *
 ********************************************************/

//Function for incrementing temp_max variable
void b0PushCallback(void *ptr)
{
    temp_max++;
};

//Function for decrementing temp_max variable
void b1PushCallback(void *ptr)
{
    temp_max--;
};

//Function for incrementing temp_min variable
void b2PushCallback(void *ptr)
{
    temp_min++;
};

//Function for decrementing temp_min variable
void b3PushCallback(void *ptr)
{
    temp_min--;
};

//Function for incrementing humThreshold variable
void b4PushCallback(void *ptr)
{
    humTreshold++;
};

//Function for decrementing humThreshold variable
void b5PushCallback(void *ptr)
{
    humTreshold--;
};

/********************************************************
 *                      SETUP                           *
 ********************************************************/

void setup()
{
    Serial.begin(9600); // Start serial comunication at baud=9600

    pinMode(RED_LED, OUTPUT);   // Red LED on pin 6 output
    pinMode(GREEN_LED, OUTPUT); // Green LED on pin 7 output
    pinMode(BLUE_LED, OUTPUT);  // Blue LED on pin 5 output
    pinMode(FAN, OUTPUT);       // FAN on pin 4 output
    pinMode(PUMP, OUTPUT);      // PUMP on pin 8 output
    pinMode(HEATER, OUTPUT);    // PUMP on pin 9 output

    nexInit();
    dht.begin();

    // Register the event callback functions of each touch event
    b0.attachPush(b0PushCallback);
    b1.attachPush(b1PushCallback);
    b2.attachPush(b2PushCallback);
    b3.attachPush(b3PushCallback);
    b4.attachPush(b4PushCallback);
    b5.attachPush(b5PushCallback);
}

/****************************************************************************************
 *  Function which send this three lines after each command sent to the nextion display *
 ****************************************************************************************/
void endNextionCommand()
{
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);
};

/********************************************************************
 * Function which map values of soil moisture sensor from 0 to 100% *
 ********************************************************************/
int gSoilMoist(int anaPin)
{
    int analogValue1 = 0;

    analogValue1 = analogRead(anaPin);
    analogValue1 = map(analogValue1, 1000, 300, 0, 100);
    return analogValue1;
};

/********************************************************************
 *       Function which map values LDR sensor from 0 to 100%        *
 ********************************************************************/
int gLuminosity(int anaPin)
{
    int analogValue2 = 0;

    for (int i = 0; i < 10; i++)
    {
        analogValue2 += analogRead(anaPin);
        delay(60);
    }

    analogValue2 = analogValue2 / 10;
    analogValue2 = map(analogValue2, 1000, 0, 0, 100);
    return analogValue2;
};

/********************************************************
 *       Function used for sending values to display    * 
 ********************************************************/
void sendMeasurementsToNextion()
{
    String temperature = "temp.txt=\"" + String(temp) + "\"";
    Serial.print(temperature);
    endNextionCommand();

    String humidity = "hum.txt=\"" + String(hum) + "\"";
    Serial.print(humidity);
    endNextionCommand();

    String soilmoist = "soilmoist.txt=\"" + String(soil_mst) + "\"";
    Serial.print(soilmoist);
    endNextionCommand();

    String lumin = "lumin.txt=\"" + String(light) + "\"";
    Serial.print(lumin);
    endNextionCommand();

    String maxtemp = "maxtemp.txt=\"" + String(temp_max) + "\"";
    Serial.print(maxtemp);
    endNextionCommand();

    String mintemp = "mintemp.txt=\"" + String(temp_min) + "\"";
    Serial.print(mintemp);
    endNextionCommand();

    String humiThreshold = "humThreshold.txt=\"" + String(humTreshold) + "\"";
    Serial.print(humiThreshold);
    endNextionCommand();
};

/************************************************************ 
 *               Reading values from each sensor            *
 ************************************************************/
void readSensors()
{
    temp = dht.readTemperature();
    hum = dht.readHumidity();
    soil_mst = gSoilMoist(SOIL);
    light = gLuminosity(LDR_PIN);
};

/**************************************************************
 *                 GreenHouseAuto                             *
 **************************************************************/
void greenHouseAuto()
{
    //FAN on/off
    if (temp > temp_max || hum > humTreshold)
    {
        digitalWrite(FAN, HIGH);
    }
    else
    {
        digitalWrite(FAN, LOW);
    };

    //HEATER on/off
    if (temp < temp_min)
    {
        digitalWrite(HEATER, HIGH);
    }
    else
    {
        digitalWrite(HEATER, LOW);
    };

    
    //LED on/off 
    if (light > luminTreshold)
    {
      digitalWrite(GREEN_LED, LOW);
    }
    else
    {
      digitalWrite(GREEN_LED, HIGH); 
    };

    //PUMP on/off
    if (soil_mst < soilTreshold)
    {
      digitalWrite(PUMP, HIGH);
    }
    else
    {
      digitalWrite(PUMP, LOW); 
    };    
    
};

/**************************************************************
 *                    VOID LOOP                               *
 *************************************************************/
void loop()
{

    readSensors();
    greenHouseAuto();
    sendMeasurementsToNextion();
    

    nexLoop(nex_listen_list);
};
