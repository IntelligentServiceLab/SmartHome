package com.example.smarthomemqtt.broker;

import io.moquette.broker.Server;
import io.moquette.broker.config.MemoryConfig;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Component;

import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import java.io.IOException;
import java.util.Properties;

@Slf4j
@Component
public class MqttBrokerConfig {
    private Server mqttBroker;

    @PostConstruct
    public void startBroker() {
        mqttBroker = new Server();
        Properties configProps = new Properties();
        configProps.setProperty("port", "1883"); // MQTT 端口
        configProps.setProperty("host", "0.0.0.0"); // 监听所有 IP
        configProps.setProperty("allow_anonymous", "true"); // 允许匿名连接

        try {
            mqttBroker.startServer(new MemoryConfig(configProps));
            log.info("✅ MQTT Broker 启动成功，监听端口 1883");
        } catch (IOException e) {
            log.error("❌ MQTT Broker 启动失败", e);
        }
    }

    @PreDestroy
    public void stopBroker() {
        if (mqttBroker != null) {
            mqttBroker.stopServer();
            log.info("🔴 MQTT Broker 已关闭");
        }
    }
}