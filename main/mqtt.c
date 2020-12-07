//
// Created by derk on 13-9-20.
//

#include "mqtt.h"

/* MQTT (over TCP) Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include <driver/gpio.h>

#include "esp_log.h"
#include "mqtt_client.h"
#include "base.h"

static const char *TAG = "MQTT";

static EventGroupHandle_t mqtt_event_group;
static esp_mqtt_client_handle_t client;

//Subscribe to socket/1/state
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    switch (event->event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Mqtt connected!");
        esp_mqtt_client_publish(client, "socket/1/status", "\"connected\"", 0, 1, 1);
        esp_mqtt_client_subscribe(client, "socket/1/state", 1);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        break;
    case MQTT_EVENT_PUBLISHED:
        break;
    case MQTT_EVENT_DATA:
        if(strcmp(event->topic, "socket/1/state") == 0)
        {
            //Payload is either "on" or "off"
            if(event->data[0] != '"' && event->data[event->data_len - 1] != '"') break;
            event->data[event->data_len - 1] = '\0';
            memmove(event->data, event->data+1, event->data_len);
            if(strcmp(event->data, "on") == 0)
            {
                gpio_set_level(RELAY_GPIO, 1);
                ESP_LOGI(TAG, "Relay -> 1");
            }
            else if(strcmp(event->data, "off") == 0)
            {
                gpio_set_level(RELAY_GPIO, 0);
                ESP_LOGI(TAG, "Relay -> 0");
            }
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
    return ESP_OK;
}

void start_mqtt_client(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://142.93.224.106",
        .username = "derk",
        .password = "sopkut",
        .event_handle = mqtt_event_handler_cb,
        .lwt_topic = "socket/1/status",
        .lwt_msg = "\"disconnected\"",
        .lwt_qos = 1,
        .lwt_retain = 1
    };

    if(!mqtt_event_group)
        mqtt_event_group = xEventGroupCreate();

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}

void stop_mqtt_client(void)
{
    if(client)
    {
        esp_mqtt_client_stop(client);
        esp_mqtt_client_destroy(client);
    }
}