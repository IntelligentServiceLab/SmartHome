package com.example.smarthomemanage.service;

import com.example.smarthomemanage.feign.MqttFeignClient;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class MyMqttService {

    @Autowired
    private MqttFeignClient mqttFeignClient;

    /**
     * 发布消息
     *
     * @param topic MQTT主题
     * @param msg MQTT消息
     * @return 发布的结果
     */

    public void publishMessage(String topic, String msg) {
        String result = mqttFeignClient.sendMqttMessage(topic, msg);
        System.out.println("发布结果：" + result);
    }
}

