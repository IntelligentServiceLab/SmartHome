package com.example.smarthomemqtt.service;

import com.example.bean.entity.*;
import com.example.bean.repository.*;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.eclipse.paho.client.mqttv3.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import javax.annotation.PostConstruct;
import java.nio.charset.StandardCharsets;
import java.util.HashSet;
import java.util.Set;

@Component
public class MqttSubscriber implements MqttCallback {

    @Autowired
    private MqttClient mqttClient;

    @Autowired
    private RoomRepository roomRepository;

    @Autowired
    private SensorRepository sensorRepository;

    @Autowired
    private DeviceRepository deviceRepository;

    // 存储已订阅的主题，防止重复订阅
    private final Set<String> subscribedTopics = new HashSet<>();

    // **初始化时默认订阅的主题**
    private static final String[] INITIAL_TOPICS = {
            "sensor/data",  //传感器数据
            "device/status" //设备状态
     };

    @PostConstruct
    public void init() {
        try {
            mqttClient.setCallback(this);
            System.out.println("✅ 订阅者已初始化，正在订阅默认主题...");

            // 订阅初始化主题
            for (String topic : INITIAL_TOPICS) {
                subscribeTopic(topic);
            }
        } catch (Exception e) {
            System.err.println("❌ MQTT 初始化失败: " + e.getMessage());
        }
    }

    // **订阅新主题**
    public boolean subscribeTopic(String topic) {
        try {
            if (!subscribedTopics.contains(topic)) {
                mqttClient.subscribe(topic);
                subscribedTopics.add(topic);
                System.out.println("✅ 成功订阅主题: " + topic);
                return true;
            } else {
                System.out.println("⚠️ 已订阅过该主题: " + topic);
                return false;
            }
        } catch (MqttException e) {
            System.err.println("❌ 订阅失败: " + e.getMessage());
        }
        return false;
    }

    // **取消订阅主题**
    public boolean unsubscribeTopic(String topic) {
        try {
            if (subscribedTopics.contains(topic)) {
                mqttClient.unsubscribe(topic);
                subscribedTopics.remove(topic);
                System.out.println("✅ 成功取消订阅: " + topic);
                return true;
            } else {
                System.out.println("⚠️ 当前未订阅该主题: " + topic);
                return false;
            }
        } catch (MqttException e) {
            System.err.println("❌ 取消订阅失败: " + e.getMessage());
        }
        return false;
    }

    // **查询当前已订阅的所有主题**
    public Set<String> getSubscribedTopics() {
        return new HashSet<>(subscribedTopics);
    }

    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        String payload = new String(message.getPayload(), StandardCharsets.UTF_8);
        System.out.println("📥 收到消息 [主题: " + topic + "]: " + payload);

        ObjectMapper objectMapper = new ObjectMapper();

        try {
            switch (topic) {
//                 topic: sensor/data
//                {
//                     "sensorType": "temperature/humidity/light",
//                     "sensorValue": "25.3",
//                     "roomId": "room-001"
//                }
                case "sensor/data": {
                    JsonNode sensorNode = objectMapper.readTree(payload);
                    String roomId = sensorNode.get("roomId").asText();
                    Room room = roomRepository.findById(roomId).orElse(null);
                    if (room == null) {
                        System.out.println("❌ 房间不存在，无法保存传感器数据。roomId=" + roomId);
                        return;
                    }

                    Sensor sensor = new Sensor();
                    sensor.setSensorType(sensorNode.get("sensorType").asText());
                    sensor.setSensorValue(sensorNode.get("sensorValue").asDouble());
                    sensor.setRoom(room);
                    sensorRepository.save(sensor);
                    System.out.println("✅ 传感器数据已保存");
                    break;
                }
//                topic: sensor/data              //设备状态
//                {
//                     "deviceId": "light-001",
//                     "deviceStatus": "on"
//                }
                case"device/status":
                    JsonNode deviceNode = objectMapper.readTree(payload);
                    String deviceId = deviceNode.get("deviceId").asText();
                    Device device = deviceRepository.findById(deviceId).orElse(null);
                    device.setDeviceStatus(deviceNode.get("deviceStatus").asText());
                    deviceRepository.save(device);
                    System.out.println("✅ 设备状态已保存");
                    break;
                default:
                    System.out.println("⚠️ 未处理的主题：" + topic);
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("❌ 处理 MQTT 消息失败: " + e.getMessage());
        }
    }

    @Override
    public void connectionLost(Throwable cause) {
        System.err.println("⚠️ 连接断开: " + cause.getMessage());
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {
        // 订阅者不需要处理这个
    }
}
