package com.example.smarthomemqtt.controller;

import com.example.smarthomemqtt.service.MqttPublisher;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/api/mqtt")
public class MqttPubController {
    @Autowired
    private MqttPublisher mqttPublisher;

    // 发送 MQTT 消息
    @PostMapping("/publish")
    public String sendMqttMessage(@RequestParam String topic, @RequestParam String message) {
        try {
            mqttPublisher.publishMessage(topic, message);
            return "✅ MQTT 消息已发送: " + "主题[" + topic + "] 内容[" + message + "]";
        } catch (MqttException e) {
            return "❌ 发送失败: " + e.getMessage();
        }
    }
}