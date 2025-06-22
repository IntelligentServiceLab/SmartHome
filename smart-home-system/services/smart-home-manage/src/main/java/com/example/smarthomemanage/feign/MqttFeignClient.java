package com.example.smarthomemanage.feign;

import org.springframework.cloud.openfeign.FeignClient;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;


@FeignClient(name = "smart-home-mqtt")
public interface MqttFeignClient {
    @PostMapping("/api/mqtt/publish")
    String sendMqttMessage(@RequestParam String topic, @RequestParam String message);
}

