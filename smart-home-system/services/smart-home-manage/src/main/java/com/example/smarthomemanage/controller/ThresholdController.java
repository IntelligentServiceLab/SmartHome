package com.example.smarthomemanage.controller;

import com.example.bean.entity.Threshold;
import com.example.smarthomemanage.service.MyMqttService;
import com.example.smarthomemanage.service.ThresholdService;
import com.example.smarthomemanage.utils.JsonUtil;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import java.util.List;

@RestController
@RequestMapping("/api/thresholds")
public class ThresholdController {

    @Autowired
    private ThresholdService thresholdService;

    @Autowired
    private MyMqttService myMqttService;

    /**
     * 获取指定房间的所有阈值
     * GET /api/thresholds/room/{roomId}
     */
    @GetMapping("/room/{roomId}")
    public ResponseEntity<List<Threshold>> getThresholdsByRoomId(@PathVariable String roomId) {
        List<Threshold> preferences = thresholdService.getThresholdsByRoomId(roomId);
        return ResponseEntity.ok(preferences);
    }

    /**
     * 获取单个阈值信息
     * GET /api/thresholds/{thresholdId}
     */
    @GetMapping("/{thresholdId}")
    public ResponseEntity<Threshold> getThresholdById(@PathVariable String thresholdId) {
        Threshold threshold = thresholdService.getThresholdById(thresholdId);
        return threshold != null ? ResponseEntity.ok(threshold) : ResponseEntity.notFound().build();
    }

    /**
     * 添加阈值到指定房间
     * POST /api/thresholds/{roomId}
     * 请求体示例：
     * {
     *   "thresholdID": "temperature-001",
     *   "thresholdName": "温度阈值",
     *   "thresholdType": "temperature",
     *   "highThresholdValue": 22.3,
     *   "lowThresholdValue": 10,
     * }
     */
    @PostMapping("/{roomId}")
    public ResponseEntity<Threshold> addThresholdToRoom(@PathVariable String roomId,
                                                          @RequestBody Threshold threshold) {
        Threshold savedThreshold = thresholdService.addThresholdToRoom(roomId, threshold);
        return ResponseEntity.ok(savedThreshold);
    }

    /**
     * 更新指定阈值设置
     * PUT /api/thresholds/{roomId}/{thresholdId}
     *  请求体示例：
     * {
     *   "thresholdName": "温度阈值",
     *   "thresholdType": "temperature",
     *   "highThresholdValue": 22.3,
     *   "lowThresholdValue": 10,
     * }
     */
    @PutMapping("/{roomId}/{thresholdId}")
    public ResponseEntity<Threshold> updateThreshold(@PathVariable String roomId,
                                                       @PathVariable String thresholdId,
                                                       @RequestBody Threshold threshold) {
        Threshold updatedThreshold = thresholdService.updateThreshold(roomId, thresholdId, threshold);
        //发布MQTT消息
        if(updatedThreshold != null)
        {
            // 构建 thresholdId 和 lowThreshold + highThreshold 的 JSON
            String json = JsonUtil.buildThresholdData(thresholdId,
                        updatedThreshold.getLowThresholdValue(),
                        updatedThreshold.getHighThresholdValue());
            myMqttService.publishMessage("thresholds", json);
        }
        return updatedThreshold != null ? ResponseEntity.ok(updatedThreshold) : ResponseEntity.notFound().build();
    }

    /**
     * 删除指定阈值设置
     * DELETE /api/thresholds/{thresholdId}
     */
    @DeleteMapping("/{thresholdId}")
    public ResponseEntity<String> deleteThreshold(@PathVariable String thresholdId) {
        boolean deleted = thresholdService.deleteThreshold(thresholdId);
        return deleted ? ResponseEntity.ok("✅ 阈值已删除") : ResponseEntity.status(404).body("❌ 阈值不存在");
    }
}

