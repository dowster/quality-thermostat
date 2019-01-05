extern "C"{
    #include "mgos.h"
    #include <sys/time.h>
    #include "mgos_gpio.h"
    #include "mgos_mqtt.h"
    #include "mgos_aws_shadow.h"
    #include "string.h"
}

#include "images.cpp"

#include "mgos_arduino_ssd1306.h"

#define OLED_RESET 4
Adafruit_SSD1306 * display;

#include <ctime>

#include "../lib/thermostat/Thermostat.h"
#include "../lib/thermostat/relays/HardwareRelay.h"
#include "../lib/thermostat/sensors/TMP36.h"
#include "../lib/thermostat/Temperature.h"
#include "../lib/thermostat/Schedule.h"

#define TEMP_UP_BUTTON 33
#define TEMP_DOWN_BUTTON 32
#define CYCLE_MODE_BUTTON 2

#define BUTTON_DEBOUNCE 1000


Thermostat * thermostatA;
HardwareRelay * heatingRelay;
HardwareRelay * coolingRelay;
TemperatureSensor * temp;
Schedule * schedule;

mgos_timer_id timerID;

void setup();
void loop(void *arg);

void increaseTemperatureButton(int pin, void* arg);
void decreaseTemperatureButton(int pin, void* arg);
void cycleOperatingMode(int pin, void* arg);
void sendMQTTUpdate(void * arg);
void WriteString(Adafruit_SSD1306 * display, String string);
void WriteString(Adafruit_SSD1306 * display, char *string);

void awsShadowHandler (
    void *arg, enum mgos_aws_shadow_event ev, uint64_t version,
    const struct mg_str reported, const struct mg_str desired,
    const struct mg_str reported_md, const struct mg_str desired_md);

enum mgos_app_init_result mgos_app_init(void) {
    setup();
    return MGOS_APP_INIT_SUCCESS;
}

void setup()
{
    heatingRelay = new HardwareRelay(14);
    coolingRelay = new HardwareRelay(12);

    temp = new TMP36(0);

    thermostatA  = new Thermostat(
        temp,
        heatingRelay,
        coolingRelay
        );
    LOG(LL_INFO, ("Creating Schedule Data"));
    ScheduleData sd = {61.0f, 62.0f, 63.0f, 64.0f, 65.0f, 66.0f, 67.0f, 68.0f, 69.0f, 70.0f, 71.0f, 72.0f, 73.0f, 74.0f, 75.0f, 76.0f, 77.0f, 78.0f, 79.0f, 80.0f, 81.0f, 82.0f, 83.0f, 84.0f, 85.0f, 86.0f, 87.0f, 88.0f};

    LOG(LL_INFO, ("Creating Schedule"));
    schedule = new Schedule(sd);

    LOG(LL_INFO, ("Setting Schedule"));
    thermostatA->setSchedule(schedule);

    LOG(LL_INFO, ("Setting Target"));
    Temperature temp = Temperature(60, Temperature::Unit::FARENHEIT);
    thermostatA->setTarget(temp);

    LOG(LL_INFO, ("Setting Operating Mode "));
    thermostatA->changeOperatingMode(Thermostat::OperatingModes::Heating);

    LOG(LL_INFO, ("Setting Source"));
    thermostatA->setSource(Thermostat::TargetSource::Manual);

    timerID = mgos_set_timer(1000, 1, &loop, NULL);
    mgos_set_timer(10000, 1, &sendMQTTUpdate, thermostatA);
    mgos_gpio_set_mode(TEMP_UP_BUTTON, MGOS_GPIO_MODE_INPUT);
    mgos_gpio_set_mode(TEMP_DOWN_BUTTON, MGOS_GPIO_MODE_INPUT);
    mgos_gpio_set_mode(CYCLE_MODE_BUTTON, MGOS_GPIO_MODE_INPUT);

    mgos_gpio_set_button_handler(TEMP_UP_BUTTON, MGOS_GPIO_PULL_UP, MGOS_GPIO_INT_EDGE_NEG, BUTTON_DEBOUNCE, &increaseTemperatureButton, NULL);
    mgos_gpio_set_button_handler(TEMP_DOWN_BUTTON, MGOS_GPIO_PULL_UP, MGOS_GPIO_INT_EDGE_NEG, BUTTON_DEBOUNCE, &decreaseTemperatureButton, NULL);
    mgos_gpio_set_button_handler(CYCLE_MODE_BUTTON, MGOS_GPIO_PULL_UP, MGOS_GPIO_INT_EDGE_NEG, BUTTON_DEBOUNCE, &cycleOperatingMode, NULL);

    display = mgos_ssd1306_create_i2c(-1, MGOS_SSD1306_RES_128_64);
  display->begin(SSD1306_SWITCHCAPVCC, 0x3C, false); 
      display->clearDisplay();
    display->drawBitmap(0,0,dowsterIcon_128x32, 128, 32, 1, 0);
    display->drawBitmap(28,32,turboIcon_32x22, 32, 22, 1, 0);
    display->drawBitmap(60,32,flameIcon_32x22, 32, 22, 1, 0);
    display->display();
    delay(2000);

  // Clear the buffer.
  display->clearDisplay();
  display->drawBitmap(96,0,turboIcon_32x22, 32, 22, 1, 0);
  display->drawBitmap(96,24,flameIcon_32x22, 32, 22, 1, 0);
  // draw a single pixel
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  display->display();
  delay(2000);

    mgos_aws_shadow_set_state_handler(awsShadowHandler, thermostatA);
}

void loop(void *arg)
{
    thermostatA->loop();

  display->clearDisplay();
      display->setTextSize(2);
  display->setTextColor(WHITE);
  display->setCursor(0,0);
  
  WriteString(display, "Thermy");
  display->println(); 
  switch(thermostatA->getOperatingMode()) {
              case Thermostat::OperatingModes::Heating:
        WriteString(display, "Heat");
        break;
        case Thermostat::OperatingModes::Cooling:
        WriteString(display, "Cool");
        break;
        case Thermostat::OperatingModes::Off:
        WriteString(display, "Off");
        break;
        default:
        WriteString(display, "Rekt");
        break;
  }
  display->println(); 
  char buffer[5];
  sprintf(buffer, "%3.1f", thermostatA->getTemperature().getTemperature(Temperature::Unit::FARENHEIT));
  WriteString(display, buffer);
  WriteString(display, "F");

  display->drawBitmap(96,0,turboIcon_32x22, 32, 22, 1, 0);
  display->drawBitmap(96,24,flameIcon_32x22, 32, 22, 1, 0);
  // draw a single pixel
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  display->display();
  display->display();

    LOG(LL_INFO, ("----------------Update Message----------------"));
    thermostatA->getStatus();
    LOG(LL_INFO, ("----------------------------------------------"));   

    (void)arg;
}

void increaseTemperatureButton(int pin, void* arg)
{
    LOG(LL_INFO, ("Increase Temperature Button Pressed"));
    
    Temperature newTemp = thermostatA->getTarget() + Temperature(1.0f, Temperature::Unit::FARENHEIT);
    thermostatA->setTarget(newTemp);
    loop(NULL);
    (void) pin;
}

void decreaseTemperatureButton(int pin, void* arg)
{
    LOG(LL_INFO, ("Decrease Temperature Button Pressed"));
    
    Temperature newTemp = thermostatA->getTarget() - Temperature(1.0f, Temperature::Unit::FARENHEIT);
    thermostatA->setTarget(newTemp);
    loop(NULL);

    (void) pin;
}

void cycleOperatingMode(int pin, void* arg)
{
    LOG(LL_INFO, ("Cycle Operation Mode Pressed"));

    switch(thermostatA->getOperatingMode()) 
    {
        case Thermostat::OperatingModes::Cooling:
            thermostatA->changeOperatingMode(Thermostat::OperatingModes::Heating);
            break;
        case Thermostat::OperatingModes::Heating:
            thermostatA->changeOperatingMode(Thermostat::OperatingModes::Off);
            break;
        case Thermostat::OperatingModes::Off:
            thermostatA->changeOperatingMode(Thermostat::OperatingModes::Cooling);
            break;
        default:
            thermostatA->changeOperatingMode(Thermostat::OperatingModes::Cooling);
    }

    loop(NULL);
    (void) pin;
    (void*)arg;
}

void sendMQTTUpdate(void * arg) {
    Thermostat * thermostat = (Thermostat*)arg;

    mgos_aws_shadow_updatef(
        0, 
        "{reported:{mode: %d, temp: %f, target: %f, heating: %s, cooling: %s, waitTime: %d, debounce: %d}}", 
        thermostat->getOperatingMode(), thermostat->getTemperature().getTemperature(), thermostat->getTarget().getTemperature(),
        thermostat->getHeatingRelay()->getActivated() ? "true" : "false",thermostat->getCoolingRelay()->getActivated() ? "true" : "false", thermostat->WaitPeriod, thermostat->getDebounce());
}


void awsShadowHandler (
    void *arg, enum mgos_aws_shadow_event ev, uint64_t version,
    const struct mg_str reported, const struct mg_str desired,
    const struct mg_str reported_md, const struct mg_str desired_md) {

    LOG(LL_INFO, ("Processing shadow event: %d", ev));

    if(desired.len > 0 && ev == MGOS_AWS_SHADOW_UPDATE_DELTA) {
        int mode = -1;
        float target = -1;

        LOG(LL_INFO, ("Reported=%.*s", desired.len, desired.p));
        
        LOG(LL_INFO, ("Got to 1"));
        json_scanf(desired.p, desired.len, 
                                "{mode: %d, target: %f}",
                                &mode,
                                &target);
                                
            LOG(LL_INFO, ("Parsed parameters: [mode=%d, target=%f]", mode, target));

        if(target > 0.0f) {
            Thermostat * thermostat = (Thermostat*)arg;
            thermostat->getStatus();
            thermostat->setTarget(Temperature(target, Temperature::Unit::FARENHEIT));
            mgos_aws_shadow_update_simple(0, "{desired: target: null}");
        }

        if(mode != -1) {
            Thermostat * thermostat = (Thermostat*)arg;
            thermostat->changeOperatingMode((Thermostat::OperatingModes)mode);
        }

    }
}

void WriteString(Adafruit_SSD1306 * display, String string) {
  int len = string.length();
  for(int i = 0; i < len; i++) {
    display->write(string.charAt(i));
  }
}

void WriteString(Adafruit_SSD1306 * display, char *string) {
  for(int i = 0; string[i] != 0; i++) {
    display->write(string[i]);
  }
}