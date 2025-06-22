package com.example.smarthomemanage.controller;

import com.example.bean.entity.Device;
import com.example.bean.entity.Sensor;
import com.example.bean.repository.SensorRepository;
import com.example.smarthomemanage.service.SensorService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/sensors")
public class SensorController {
    @Autowired
    private SensorService sensorService;
    /**
     * 获取指定房间指定类型的传感器数据
     * GET /api/sensors/room/{roomId}/{sensorType}
     */
    @GetMapping("/room/{roomId}/{sensorType}")
    public ResponseEntity<List<Sensor>> getSensorsByRoomId(@PathVariable String roomId,
                                                           @PathVariable String sensorType) {
        List<Sensor> sensors = sensorService.getSensorsByRoomId(roomId, sensorType);
        return ResponseEntity.ok(sensors);
    }
}
