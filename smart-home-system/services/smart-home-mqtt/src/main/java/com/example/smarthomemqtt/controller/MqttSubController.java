package com.example.smarthomemqtt.controller;

import com.example.smarthomemqtt.service.MqttSubscriber;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.Set;

@RestController
@RequestMapping("/api/mqtt")
public class MqttSubController {

    @Autowired
    private MqttSubscriber mqttSubscriber;

    // **订阅新主题**
    @PostMapping("/subscribe")
    public String subscribe(@RequestParam String topic) {
        if(mqttSubscriber.subscribeTopic(topic))return "✅ 成功订阅主题: " + topic;
        return "⚠️ 已订阅过该主题: " + topic;
    }

    // **取消订阅主题**
    @PostMapping("/unsubscribe")
    public String unsubscribe(@RequestParam String topic) {
        if(mqttSubscriber.unsubscribeTopic(topic)) return "✅ 成功取消订阅: " + topic;
        return "⚠️ 当前未订阅该主题: " + topic;
    }

    // **查询当前已订阅的所有主题**
    @GetMapping("/subscriptions")
    public Set<String> getSubscriptions() {
        return mqttSubscriber.getSubscribedTopics();
    }
}
