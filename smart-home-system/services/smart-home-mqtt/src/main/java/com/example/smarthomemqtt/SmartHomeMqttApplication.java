package com.example.smarthomemqtt;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.autoconfigure.domain.EntityScan;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.data.jpa.repository.config.EnableJpaRepositories;

@SpringBootApplication
@EnableDiscoveryClient  // 启用 Nacos 发现
@EnableJpaRepositories(basePackages = "com.example.bean.repository")
@EntityScan(basePackages = "com.example.bean.entity")
public class SmartHomeMqttApplication {
    public static void main(String[] args) {
        SpringApplication.run(SmartHomeMqttApplication.class, args);
    }

}
