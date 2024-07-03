#include "RMaker.h"
#include <WiFi.h>
#include "WiFiProv.h"
#include "AppInsights.h"
#include <EEPROM.h>
// #include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>

#define EEPROM_SIZE 4
#define DEFAULT_POWER_MODE false


const char *service_name = "PROV_1234";
const char *pop = "abcd1234";

// ir receive pin
const int RECV_PIN = 35;
IRrecv irrecv(RECV_PIN);
decode_results results;

// const uint16_t IrLed = 21;  // ESP8266 GPIO pin to use.
// IRsend irsend(IrLed);  // Set the GPIO to be used to sending the message.

const int power = 0x40040100BCBD;
const int irbtn1 = 0x400401000809;
const int irbtn2 = 0x400401008889;
const int irbtn3 = 0x400401004849;
const int irbtn4 = 0x40040100C8C9;

//light led
static int LED = 2;

// reset switch
static int gpio_0 = 0;

//device names
const char device1[] = "Tube Light 1";
const char device2[] = "Small Light";
const char device3[] = "Fan 1";
const char device4[] = "Fan 2";

// physical switchs
static int button1 = 27;
static int button2 = 26;
static int button3 = 25;
static int button4 = 33;


// relays
static int relay1 = 16;
static int relay2 = 17;
static int relay3 = 18;
static int relay4 = 19;

// relay states
bool relaystate1;
bool relaystate2;
bool relaystate3;
bool relaystate4;

// Declare fur switch objects
static LightBulb *switch1 = NULL;
static LightBulb *switch2 = NULL;
static Device *switch3 = NULL;
static Switch *switch4 = NULL;

//physical switch states flags
bool button1_flg = false;
bool button2_flg = false;
bool button3_flg = false;
bool button4_flg = false;

// debounce timings
unsigned long long int debounce1 = 0;
unsigned long long int debounce2 = 0;
unsigned long long int debounce3 = 0;
unsigned long long int debounce4 = 0;
int debounce_interval = 500;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void sysProvEvent(arduino_event_t *sys_event)
{
    switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
#if CONFIG_IDF_TARGET_ESP32S2
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n",
                      service_name, pop);
        printQR(service_name, pop, "softap");
#else
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n",
                      service_name, pop);
        printQR(service_name, pop, "ble");
#endif
        break;
    case ARDUINO_EVENT_PROV_INIT:
        wifi_prov_mgr_disable_auto_stop(10000);
        break;
    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
        wifi_prov_mgr_stop_provisioning();
        break;
    default:;
    }
}

// Callback function for handling write events for switches//////////////////////////////////////////////
void write_callback_switch(Device *device, Param *param, const param_val_t val,
                    void *priv_data, write_ctx_t *ctx)
{
    const char *device_name = device->getDeviceName();
    const char *param_name = param->getParamName();

    if(strcmp(device_name, device1) == 0)
    {
      if (strcmp(param_name, "Power") == 0) {
          Serial.printf("tube light Received value = %s for %s - %s\n", 
                        val.val.b ? "true" : "false", device_name, param_name);
          relaystate1 = val.val.b;
          (relaystate1 == false) ? digitalWrite(relay1, LOW)
          : digitalWrite(relay1, HIGH);
          param->updateAndReport(val);

          EEPROM.write(0, relaystate1);
          EEPROM.commit();
      }
    }

    if(strcmp(device_name, device2) == 0)
    {
      if (strcmp(param_name, "Power") == 0) {
          Serial.printf("small light Received value = %s for %s - %s\n",
                        val.val.b ? "true" : "false", device_name, param_name);
          relaystate2 = val.val.b;
          (relaystate2 == false) ? digitalWrite(relay2, LOW)
          : digitalWrite(relay2, HIGH);
          param->updateAndReport(val);

          EEPROM.write(1, relaystate2);
          EEPROM.commit();
      }
    }

    if(strcmp(device_name, device3) == 0)
    {
      if (strcmp(param_name, "Power") == 0) {
          Serial.printf("fan 1 Received value = %s for %s - %s\n",
                        val.val.b ? "true" : "false", device_name, param_name);
          relaystate3 = val.val.b;
          (relaystate3 == false) ? digitalWrite(relay3, LOW)
          : digitalWrite(relay3, HIGH);
          param->updateAndReport(val);

          EEPROM.write(2, relaystate3);
          EEPROM.commit();
      }

      else if(strcmp(param_name, "Level") == 1)
      {
        int value = val.val.i;

        if(value == 1)
        {
          Serial.printf("\nReceived value = %d for %s - %s\n", val.val.i, device_name, param_name);
          // irsend.sendNEC(0xCFD12E);
          param->updateAndReport(val);
        }

        else if(value == 2)
        {
          Serial.printf("\nReceived value = %d for %s - %s\n", val.val.i, device_name, param_name);
          // irsend.sendNEC(0xCF09F6);
          param->updateAndReport(val);
        }

        else if(value == 3)
        {
          Serial.printf("\nReceived value = %d for %s - %s\n", val.val.i, device_name, param_name);
          // irsend.sendNEC(0xCF51AE);
          param->updateAndReport(val);
        }

        else if(value == 4)
        {
          Serial.printf("\nReceived value = %d for %s - %s\n", val.val.i, device_name, param_name);
          // irsend.sendNEC(0xCFC936);
          param->updateAndReport(val);
        }

        else if(value == 5)
        {
          Serial.printf("\nReceived value = %d for %s - %s\n", val.val.i, device_name, param_name);
          // irsend.sendNEC(0xCF11EE);
          param->updateAndReport(val);
        }

        else if(value == 6)
        {
          Serial.printf("\nReceived value = %d for %s - %s\n", val.val.i, device_name, param_name);
          // irsend.sendNEC(0xCFF10E);
          param->updateAndReport(val);
        }
      }
    }

    if(strcmp(device_name, device4) == 0)
    {
      if (strcmp(param_name, "Power") == 0) {
          Serial.printf("fan 2 Received value = %s for %s - %s\n",
                        val.val.b ? "true" : "false", device_name, param_name);
          relaystate4 = val.val.b;
          (relaystate4 == false) ? digitalWrite(relay4, LOW)
          : digitalWrite(relay4, HIGH);
          param->updateAndReport(val);

          EEPROM.write(3, relaystate4);
          EEPROM.commit();
      }
    }
}

// INTERRUPTS FOR 5 BUTTONS!!////////////////////////////////////////////////////////////////////////
void IRAM_ATTR button1_interrupt()
{
  if(millis() - debounce1 > debounce_interval)
  {
    button1_flg = true;
    Serial.println("Interrupt 1");
    debounce1 = millis();
  }
}

void IRAM_ATTR button2_interrupt()
{
  if(millis() - debounce2 > debounce_interval)
  {
    button2_flg = true;
    Serial.println("Interrupt 2");
    debounce2 = millis();
  }
}

void IRAM_ATTR button3_interrupt()
{
  if(millis() - debounce3 > debounce_interval)
  {
    button3_flg = true;
    Serial.println("Interrupt 3");
    debounce3 = millis();
  }

}

void IRAM_ATTR button4_interrupt()
{
  if(millis() - debounce4 > debounce_interval)
  {
    button4_flg = true;
    Serial.println("Interrupt 4");
    debounce4 = millis();
  }
}


// IR REMOTE RECEIVER ////////////////////////////////////////////////////////////////////////////
void ir_receiver_tsop1838()
{
  if (irrecv.decode(&results))
  {
    int received;
    received = results.value;

    if(received == power) // turn it all off
    {
      relaystate1 = false;
      relaystate2 = false;
      relaystate3 = false;
      relaystate4 = false;

      digitalWrite(relay1, relaystate1);
      digitalWrite(relay2, relaystate2);
      digitalWrite(relay3, relaystate3);
      digitalWrite(relay4, relaystate4);

      // updating the values
      switch1->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate1);
      switch2->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate2);
      switch3->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate3);
      switch4->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate4);

      //EEPROM memory
      EEPROM.write(0, relaystate1);
      EEPROM.write(1, relaystate2);
      EEPROM.write(2, relaystate3);
      EEPROM.write(3, relaystate4);
      EEPROM.commit();

      Serial.println("Turned all devices off");

    }

    else if(received == irbtn1)
    {
      relaystate1 = !relaystate1;
      digitalWrite(relay1, relaystate1);
      switch1->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate1);
      Serial.println("tube light state: " + String(relaystate1));
      EEPROM.write(0, relaystate1);
      EEPROM.commit();
    }

    else if(received == irbtn2)
    {
      relaystate2 = !relaystate2;
      digitalWrite(relay2, relaystate2);
      switch2->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate2);
      Serial.println("small state: " + String(relaystate2));
      EEPROM.write(1, relaystate2);
      EEPROM.commit();
    }

    else if(received == irbtn3)
    {
      relaystate3 = !relaystate3;
      digitalWrite(relay3, relaystate3);
      switch3->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate3);
      Serial.println("fan 1 state: " + String(relaystate3));
      EEPROM.write(2, relaystate3);
      EEPROM.commit();
    }

    else if(received == irbtn4)
    {
      relaystate4 = !relaystate4;
      digitalWrite(relay4, relaystate4);
      switch4->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate4);
      Serial.println("fan 2 state: " + String(relaystate4));
      EEPROM.write(3, relaystate4);
      EEPROM.commit();
    }

    irrecv.resume();

    delay(500);

  }
}


//VOID SETUP!!! //////////////////////////////////////////////////////////////////////////////////
void setup()
{
  delay(5000);
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  // Configure GPIO pins for switches

  pinMode(gpio_0, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);

  //showcase the esp is on
  digitalWrite(2, HIGH);
  delay(200);
  digitalWrite(2,LOW);
  delay(200);
  digitalWrite(2, HIGH);
  delay(200);
  digitalWrite(2,LOW);
  delay(200);

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);

  relaystate1 = EEPROM.read(0);
  relaystate2 = EEPROM.read(1);
  relaystate3 = EEPROM.read(2);
  relaystate4 = EEPROM.read(3);

  digitalWrite(relay1, relaystate1);
  digitalWrite(relay2, relaystate2);
  digitalWrite(relay3, relaystate3);
  digitalWrite(relay4, relaystate4);

  //interrupts for switches
  attachInterrupt(button1, button1_interrupt, CHANGE);
  attachInterrupt(button2, button2_interrupt, CHANGE);
  attachInterrupt(button3, button3_interrupt, CHANGE);
  attachInterrupt(button4, button4_interrupt, CHANGE);

  // node declaration and rainmaker stuff
  Node my_node;
  my_node = RMaker.initNode("Hall Node");


  // adding all the 4 switches
  // Initialize four switch devices
  switch1 = new LightBulb(device1, &relay1);
    if (!switch1) {
        return;
    }
  // Standard switch device
  switch1->addCb(write_callback_switch);
  // Add switch device to the node
  my_node.addDevice(*switch1);

  // Initialize five switch devices
  switch2 = new LightBulb(device2, &relay2);
    if (!switch2) {
        return;
    }
  // Standard switch device
  switch2->addCb(write_callback_switch);
  // Add switch device to the node
  my_node.addDevice(*switch2);

  switch3 = new Device(device3, "custom.device.fanspeed", &relay3);
    if (!switch3) {
        return;
    }
  // Standard switch device
  switch3->addNameParam();
  switch3->addPowerParam(DEFAULT_POWER_MODE);
  switch3->assignPrimaryParam(switch3->getParamByName(ESP_RMAKER_DEF_POWER_NAME));

  Param level_param("Level", "custom.param.level", value(1), PROP_FLAG_READ | PROP_FLAG_WRITE);
  level_param.addBounds(value(1), value(6), value(1));
  level_param.addUIType(ESP_RMAKER_UI_SLIDER);
  switch3->addParam(level_param); 

  switch3->addCb(write_callback_switch);
  // Add switch device to the node
  my_node.addDevice(*switch3);

  switch4 = new Switch(device4, &relay4);
    if (!switch4) {
        return;
    }
  // Standard switch device
  switch4->addCb(write_callback_switch);
  // Add switch device to the node
  my_node.addDevice(*switch4);




  // Enable necessary services
  RMaker.enableOTA(OTA_USING_TOPICS);
  RMaker.enableTZService();
  RMaker.enableSchedule();
  RMaker.enableScenes();
  initAppInsights();
  RMaker.enableSystemService(SYSTEM_SERV_FLAGS_ALL, 2, 2, 2);

  RMaker.start();

  WiFi.onEvent(sysProvEvent);

  // Begin Wi-Fi provisioning
  #if CONFIG_IDF_TARGET_ESP32S2
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE,
                          WIFI_PROV_SECURITY_1, pop, service_name);
  #else
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM,
                          WIFI_PROV_SECURITY_1, pop, service_name);
  #endif

  // updating the values
  switch1->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate1);
  switch2->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate2);
  switch3->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate3);
  switch4->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate4);

  //beginning of ir sensor
  irrecv.enableIRIn();  // Start the receiver

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{

  //wifi connection
  if(WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(LED, HIGH);
  }
  else 
  {
    digitalWrite(LED, LOW);
    // WiFi.begin();
  }


  // reset the esp gpio button 0
  if (digitalRead(gpio_0) == LOW) 
  {  // Push button pressed

    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(gpio_0) == LOW) 
    {
        delay(50);
    }
    int endTime = millis();

    if ((endTime - startTime) > 10000) 
    {
        // If key pressed for more than 10secs, reset all
        Serial.printf("Reset to factory.\n");
        RMakerFactoryReset(2);
    } 
    else if ((endTime - startTime) > 3000) 
    {
        Serial.printf("Reset Wi-Fi.\n");
        // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
        RMakerWiFiReset(2);
    }
  }

  //switch 1
  if(button1_flg)
  {
    relaystate1 = !relaystate1;
    Serial.printf("tube light state %s.\n", relaystate1 ? "true" : "false");
    if(switch1)
    {
      switch1->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate1);
    }

    (relaystate1 == false) ? digitalWrite(relay1, LOW) : digitalWrite(relay1, HIGH);
    EEPROM.write(0, relaystate1);
    EEPROM.commit();
    button1_flg = false;
  }

  //switch 2
  if(button2_flg)
  {
    relaystate2 = !relaystate2;
    Serial.printf("small light state %s.\n", relaystate2 ? "true" : "false");
    if(switch2)
    {
      switch2->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate2);
    }

    (relaystate2 == false) ? digitalWrite(relay2, LOW) : digitalWrite(relay2, HIGH);
    EEPROM.write(1, relaystate2);
    EEPROM.commit();
    button2_flg = false;
  }

  //switch 3
  if(button3_flg)
  {
    relaystate3 = !relaystate3;
    Serial.printf("fan 1 state %s.\n", relaystate3 ? "true" : "false");
    if(switch3)
    {
      switch3->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate3);
    }

    (relaystate3 == false) ? digitalWrite(relay3, LOW) : digitalWrite(relay3, HIGH);
    EEPROM.write(2, relaystate3);
    EEPROM.commit();
    button3_flg = false;
  }


  //switch 4
  if(button4_flg)
  {
    relaystate4 = !relaystate4;
    Serial.printf("fan 2 state %s.\n", relaystate4 ? "true" : "false");
    if(switch4)
    {
      switch4->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relaystate4);
    }

    (relaystate4 == false) ? digitalWrite(relay4, LOW) : digitalWrite(relay4, HIGH);
    EEPROM.write(3, relaystate4);
    EEPROM.commit();
    button4_flg = false;
  }


  ir_receiver_tsop1838();

  delay(100);
}
