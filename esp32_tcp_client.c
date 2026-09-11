/*
 * EE 250L Fall 2026 - Lab 2, Part 3
 * ESP32-S3 TCP Client Starter
 *
 * This starter reuses the Wi-Fi setup pattern from Lab 1 and leaves the
 * important TCP-client socket operations as TODOs.
 *
 * Configure:
 *   WIFI_SSID
 *   WIFI_PASS
 *   SERVER_IP
 *   SERVER_PORT
 *
 * Then complete the TCP TODOs in tcp_client_task().
 *
 * Credential safety:
 * Do not submit or commit a real Wi-Fi password.
 */

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#define WIFI_SSID      "USC Secure Wireless"
#define WIFI_PASS      ""  // Leave blank for USC Secure Wireless

/*
 * Use the Ubuntu VM's reachable IPv4 address here, NOT 127.0.0.1.
 * Example only:
 *   #define SERVER_IP "192.168.1.50"
 */
#define SERVER_IP      "10.23.46.87"
#define SERVER_PORT    10000

#define CLIENT_MESSAGE "Hello from ESP32-S3"

static const char *TAG = "lab2_tcp_client";

static EventGroupHandle_t wifi_event_group;
static const int WIFI_CONNECTED_BIT = BIT0;

static void wifi_event_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG, "Wi-Fi disconnected; retrying...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT &&
               event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Wi-Fi connected");
        ESP_LOGI(TAG, "ESP32-S3 IP address: " IPSTR,
                 IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init_sta(void)
{
    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &wifi_event_handler,
            NULL,
            &instance_any_id));

    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &wifi_event_handler,
            NULL,
            &instance_got_ip));

    wifi_config_t wifi_config = {0};
    strlcpy((char *)wifi_config.sta.ssid,
            WIFI_SSID,
            sizeof(wifi_config.sta.ssid));
    strlcpy((char *)wifi_config.sta.password,
            WIFI_PASS,
            sizeof(wifi_config.sta.password));

    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Connecting to Wi-Fi SSID: %s", WIFI_SSID);

    xEventGroupWaitBits(wifi_event_group,
                        WIFI_CONNECTED_BIT,
                        pdFALSE,
                        pdTRUE,
                        portMAX_DELAY);
}

static void tcp_client_task(void *pvParameters)
{
    char rx_buffer[256];

    ESP_LOGI(TAG, "TCP server target: %s:%d", SERVER_IP, SERVER_PORT);

    // creating TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (sock < 0) {
        ESP_LOGE(TAG, "TODO: socket has not been created yet");
        vTaskDelete(NULL);
        return;
    }

    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(SERVER_PORT);

    /*
        Convert SERVER_IP from dotted-decimal text into the binary
     * IPv4 address stored in dest_addr.sin_addr
     */

     int ip_conv = inet_pton(AF_INET, SERVER_IP, &dest_addr.sin_addr);
        // check if conversion was successful? 
        if (ip_conv <= 0) {
            ESP_LOGE(TAG, "Invalid IP address format: %s", SERVER_IP);
            close(sock);
            // delete the task if IP conversion fails to avoid further errors 
            vTaskDelete(NULL);
            return;
        }

    // connect socket to python server on ubuntu VM 

    int connect_status = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    // send CLIENT_MESSAGE to server, print # of bytes sent to console 

     int bytes_sent = send(sock, CLIENT_MESSAGE, strlen(CLIENT_MESSAGE), 0);

     if (bytes_sent < 0) // check if send() was successful
     {
         ESP_LOGE(TAG, "Failed to send message");
     } else {
         ESP_LOGI(TAG, "Sent message: %s", CLIENT_MESSAGE);
     }

    // receive server acknowledgment message and print it to the console

     int bytes_received = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
     if (bytes_received < 0) {
         ESP_LOGE(TAG, "Failed to receive message");
     } else {
        /* recv() returns a byte count; if space remains in rx_buffer,
         add '\0' terminator before printing as C string */
         rx_buffer[bytes_received] = '\0';
         ESP_LOGI(TAG, "Received message: %s", rx_buffer);
     }

    
     // close TCP socket after comms are done 
     close(sock);

    ESP_LOGI(TAG, "TCP client task finished");
    vTaskDelete(NULL);
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init_sta();

    xTaskCreate(tcp_client_task,
                "tcp_client",
                4096,
                NULL,
                5,
                NULL);
}
