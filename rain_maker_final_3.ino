#include "RMaker.h"
#include <WiFi.h>
#include "WiFiProv.h"
#include "AppInsights.h"
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <EEPROM.h>

#define EEPROM_SIZE 4
#define DEFAULT_POWER_MODE false
#define DEFAULT_SPEED 3

const char *service_name = "PROV_1234";
const char *pop = "abcd1234";

static int LED = 2;
static int gpio_0 = 0;
const uint16_t IrLed = 21;
int power_state = 0;
const int RECV_PIN = 35;

//ir send
IRsend irsend(IrLed);

// ir receive pin
IRrecv irrecv(RECV_PIN);
decode_results results;

const int power = 16825533;
const int irbtn1 = 16779273;
const int irbtn2 = 16812169;
const int irbtn3 = 16795721;
const int irbtn4 = 16828617;

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
void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
    const char *device_name = device->getDeviceName();
    const char *param_name = param->getParamName();

    if (strcmp(device_name, device3) == 0 && strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
        power_state = val.val.b;
        (power_state == false) ? digitalWrite(relay3, LOW) : digitalWrite(relay3, HIGH);
        param->updateAndReport(val);
    }

    else if (strcmp(device_name, device3) == 0 && strcmp(param_name, "Speed") == 0) {
        Serial.printf("\nReceived value = %d for %s - %s\n", val.val.i, device_name, param_name);
        int speed = val.val.i;

        if (speed == 1)
            irsend.sendNEC(0xCFD12E);

        else if (speed == 2)
            irsend.sendNEC(0xCF09F6);

        else if (speed == 3)
            irsend.sendNEC(0xCF51AE);

        else if (speed == 4)
            irsend.sendNEC(0xCFC936);

        else if (speed == 5)
            irsend.sendNEC(0xCF11EE);

        else if (speed == 6)
            irsend.sendNEC(0xCFF10E);


        param->updateAndReport(val);
    }

}

// IR REMOTE RECEIVER ////////////////////////////////////////////////////////////////////////////
void tsop_receiver()
{
  if (irrecv.decode(&results))
  {
    int received;
    received = results.value;
    // Serial.println(received, HEX);
    delay(500);
    irrecv.resume();
  }
}



void setup() {
  
    delay(5000);
    Serial.begin(115200);
    EEPROM.begin(EEPROM_SIZE);

    
    pinMode(gpio_0, INPUT_PULLUP);
    pinMode(LED, OUTPUT);
    pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(button3, INPUT_PULLUP);
    pinMode(button4, INPUT_PULLUP);

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

    Node my_node;
    my_node = RMaker.initNode("Hall Node");

    // // adding all the 4 switches
    // // Initialize four switch devices
    // switch1 = new LightBulb(device1, &relay1);
    // if (!switch1) {
    //     return;
    // }
    // // Standard switch device
    // switch1->addCb(write_callback);
    // // Add switch device to the node
    // my_node.addDevice(*switch1);
    // 
    // switch2 = new LightBulb(device2, &relay2);
    // if (!switch2) {
    //     return;
    // }
    // // Standard switch device
    // switch2->addCb(write_callback);
    // // Add switch device to the node
    // my_node.addDevice(*switch2);

    switch3 = new Device(device3, "my.device.fan", NULL);


    if (!switch3) {
        return;
    }

    switch3->addNameParam();
    switch3->addPowerParam(DEFAULT_POWER_MODE);
    switch3->assignPrimaryParam(switch3->getParamByName(ESP_RMAKER_DEF_POWER_NAME));

    Param speed("Speed", ESP_RMAKER_PARAM_RANGE, value(DEFAULT_SPEED), PROP_FLAG_READ | PROP_FLAG_WRITE);
    speed.addUIType(ESP_RMAKER_UI_SLIDER);
    speed.addBounds(value(1), value(6), value(1));
    switch3->addParam(speed); 

    switch3->addCb(write_callback);
    my_node.addDevice(*switch3);

    // switch4 = new Switch(device4, &relay4);
    // if (!switch4) {
    //     return;
    // }
    // // Standard switch device
    // switch4->addCb(write_callback);
    // // Add switch device to the node
    // my_node.addDevice(*switch4);


    // Enable necessary services
    RMaker.enableSchedule();

    RMaker.enableScenes();

    RMaker.start();

    WiFi.onEvent(sysProvEvent);


#if CONFIG_IDF_TARGET_ESP32S2
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
#else
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
#endif

    irrecv.enableIRIn();
    delay(100);
    irsend.begin();

    //showcase the esp is on and setup is done
    digitalWrite(2, HIGH);
    delay(200);
    digitalWrite(2,LOW);
    delay(200);
    digitalWrite(2, HIGH);
    delay(200);
    digitalWrite(2,LOW);
    delay(200);

}

void loop()
{
  tsop_receiver();
}














