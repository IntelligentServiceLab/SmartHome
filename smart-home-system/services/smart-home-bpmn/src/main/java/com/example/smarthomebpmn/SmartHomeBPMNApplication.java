package com.example.smarthomebpmn;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.autoconfigure.domain.EntityScan;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.cloud.openfeign.EnableFeignClients;
import org.springframework.data.jpa.repository.config.EnableJpaRepositories;

@SpringBootApplication
@EnableDiscoveryClient  // 启用 Nacos 发现
@EnableJpaRepositories(basePackages = "com.example.bean.repository")
@EntityScan(basePackages = "com.example.bean.entity")
@EnableFeignClients(basePackages = "com.example.smarthomebpmn.feign") // 指定 Feign 接口包路径
public class SmartHomeBPMNApplication {
    public static void main(String[] args) {
        SpringApplication.run(SmartHomeBPMNApplication.class, args);
    }

}
