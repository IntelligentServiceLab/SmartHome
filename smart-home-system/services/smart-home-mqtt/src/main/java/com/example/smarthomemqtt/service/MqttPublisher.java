package com.example.smarthomemqtt.service;
import org.eclipse.paho.client.mqttv3.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Component
public class MqttPublisher {

    @Autowired
    private MqttClient mqttClient;

    // 发送 MQTT 消息
    public void publishMessage(String topic, String message) throws MqttException {
        MqttMessage mqttMessage = new MqttMessage(message.getBytes());
        mqttMessage.setQos(1);  // QOS 级别 1：至少一次
        mqttClient.publish(topic, mqttMessage);
        System.out.println("📡 发送 MQTT 消息: 主题[" + topic + "] 内容[" + message + "]");
    }
}