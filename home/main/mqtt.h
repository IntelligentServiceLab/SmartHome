#ifndef _MQTT_H_
#define _MQTT_H_

void mqtt_init(void);
void send_device_status(esp_mqtt_client_handle_t mqtt_client, const char *device_id, const char *device_status);

#endif
