#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <rom/gpio.h>

#include "wifi.h"
#include "base.h"
#include "led.h"
#include "mqtt.h"

void initialize_relay(void)
{
    gpio_pad_select_gpio(RELAY_GPIO);
    gpio_set_direction(RELAY_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(RELAY_GPIO, 0);
}

static void on_wifi_connect(void *data)
{
    set_led_status(LED_MODE_ON);
    start_mqtt_client();
}

static void on_wifi_disconnect(void *data)
{
    set_led_status(LED_MODE_OFF);
    stop_mqtt_client();
}

static void on_wifi_connection_reset(void* data)
{
    set_led_status(LED_MODE_FASTER_BLINK);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

static void on_wifi_receive_credentials(void* data)
{
    set_led_status(LED_MODE_FAST_BLINK);
}

static void on_wifi_sta_start(void* data)
{
    set_led_status(LED_MODE_BLINK);
}

void app_main()
{
    //Initialize components
    initialize_nvs();
    initialize_status_led();

    //Register the wifi callbacks before wifi initialization
    register_on_wifi_connect_cb(&on_wifi_connect);
    register_on_wifi_disconnect_cb(&on_wifi_disconnect);
    register_on_wifi_connection_reset(&on_wifi_connection_reset);
    register_on_wifi_receive_credentials_cb(&on_wifi_receive_credentials);
    register_on_wifi_sta_start_cb(&on_wifi_sta_start);

    initialize_wifi();
    initialize_relay();
}