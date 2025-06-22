package com.example.smarthomemanage.controller;

import com.example.bean.entity.Device;
import com.example.smarthomemanage.service.DeviceService;
import com.example.smarthomemanage.service.MyMqttService;
import com.example.smarthomemanage.utils.JsonUtil;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/devices")
public class DeviceController {

    @Autowired
    private DeviceService deviceService;

    @Autowired
    private MyMqttService myMqttService;

    /**
     * 获取指定房间的所有设备
     * GET /api/devices/room/{roomId}
     */
    @GetMapping("/room/{roomId}")
    public ResponseEntity<List<Device>> getDevicesByRoom(@PathVariable String roomId) {
        List<Device> devices = deviceService.getDevicesByRoomId(roomId);
        return ResponseEntity.ok(devices);
    }

    /**
     * 获取单个设备信息
     * GET /api/devices/{deviceId}
     */
    @GetMapping("/{deviceId}")
    public ResponseEntity<Device> getDeviceById(@PathVariable String deviceId) {
        Device device = deviceService.getDeviceById(deviceId);
        return device != null ? ResponseEntity.ok(device) : ResponseEntity.notFound().build();
    }

    /**
     * 添加设备到指定房间
     * POST /api/devices/{roomId}
     * 请求体示例：
     * {
     *   "deviceId": "fan-001",
     *   "deviceName": "空调",
     *   "deviceType": "AC",
     *   "deviceStatus": "off"
     * }
     */
    @PostMapping("/{roomId}")
    public ResponseEntity<Device> addDeviceToRoom(@PathVariable String roomId,
                                                  @RequestBody Device device) {
        Device savedDevice = deviceService.addDeviceToRoom(roomId, device);
        return ResponseEntity.ok(savedDevice);
    }

    /**
     * 更新指定设备信息
     * PUT /api/devices/{roomId}/{deviceId}
     * 请求体示例：
     * {
     *   "deviceName": "空调",
     *   "deviceType": "AC",
     *   "deviceStatus": "on"
     * }
     */
    @PutMapping("/{roomId}/{deviceId}")
    public ResponseEntity<Device> updateDevice(@PathVariable String roomId,
                                               @PathVariable String deviceId,
                                               @RequestBody Device Device) {
        Device updatedDevice = deviceService.updateDevice(roomId, deviceId, Device);
        return updatedDevice != null ? ResponseEntity.ok(updatedDevice) : ResponseEntity.notFound().build();
    }

    /**
     * 删除设备
     * DELETE /api/devices/{deviceId}
     */
    @DeleteMapping("/{deviceId}")
    public ResponseEntity<String> deleteDevice(@PathVariable String deviceId) {
        boolean deleted = deviceService.deleteDevice(deviceId);
        return deleted ? ResponseEntity.ok("✅ 设备已删除") : ResponseEntity.status(404).body("❌ 设备不存在");
    }

    /**
     * 切换设备状态（开/关）
     * POST /api/devices/{deviceId}/toggle
     */
    @PostMapping("/{deviceId}/toggle")
    public ResponseEntity<String> toggleDevice(@PathVariable String deviceId) {
        String result = deviceService.toggleDevice(deviceId);
        //发布MQTT消息
        if(result != null){
            // 构建 deviceId 和 deviceStatus 的 JSON
            String json = JsonUtil.buildDeviceStatus(deviceId, result);
            myMqttService.publishMessage("devices", json);
        }
        return result != null ? ResponseEntity.ok(result) : ResponseEntity.status(404).body("❌ 设备未找到");
    }

    /**
     * 控制设备状态（指定开/关）
     * POST /api/devices/{deviceId}/{status}
     */
    @PostMapping("/controller/{deviceId}/{status}")
    public ResponseEntity<String> controlDevice(
            @PathVariable String deviceId,
            @PathVariable String status) {
        String result = deviceService.setDeviceStatus(deviceId, status);
        // 发布 MQTT 消息
        if (result != null) {
            // 构建 deviceId 和 deviceStatus 的 JSON
            String json = JsonUtil.buildDeviceStatus(deviceId, result);
            myMqttService.publishMessage("devices", json);
        }
        return result != null ? ResponseEntity.ok(result) : ResponseEntity.status(404).body("❌ 设备未找到");
    }


}
