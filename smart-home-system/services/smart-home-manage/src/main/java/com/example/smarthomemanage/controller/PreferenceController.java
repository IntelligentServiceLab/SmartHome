package com.example.smarthomemanage.controller;

import com.example.bean.entity.Preference;
import com.example.smarthomemanage.service.MyMqttService;
import com.example.smarthomemanage.service.PreferenceService;
import com.example.smarthomemanage.utils.JsonUtil;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import java.util.List;

@RestController
@RequestMapping("/api/preferences")
public class PreferenceController {

    @Autowired
    private PreferenceService preferenceService;

    @Autowired
    private MyMqttService myMqttService;

    /**
     * 获取指定房间的所有偏好
     * GET /api/preferences/room/{roomId}
     */
    @GetMapping("/room/{roomId}")
    public ResponseEntity<List<Preference>> getPreferencesByRoomId(@PathVariable String roomId) {
        List<Preference> preferences = preferenceService.getPreferencesByRoomId(roomId);
        return ResponseEntity.ok(preferences);
    }

    /**
     * 获取单个偏好信息
     * GET /api/preferences/{preferenceId}
     */
    @GetMapping("/{preferenceId}")
    public ResponseEntity<Preference> getPreferenceById(@PathVariable String preferenceId) {
        Preference preference = preferenceService.getPreferenceById(preferenceId);
        return preference != null ? ResponseEntity.ok(preference) : ResponseEntity.notFound().build();
    }

    /**
     * 添加偏好到指定房间
     * POST /api/preferences/{roomId}
     * 请求体示例：
     * {
     *   "preferenceID": "temperature-001",
     *   "preferenceName": "温度偏好",
     *   "preferenceType": "temperature",
     *   "preferenceValue": 22.3
     * }
     */
    @PostMapping("/{roomId}")
    public ResponseEntity<Preference> addPreferenceToRoom(@PathVariable String roomId,
                                                       @RequestBody Preference preference) {
        Preference savedPreference = preferenceService.addPreferenceToRoom(roomId, preference);
        return ResponseEntity.ok(savedPreference);
    }

    /**
     * 更新指定偏好设置
     * PUT /api/preferences/{roomId}/{preferenceId}
     *  请求体示例：
     * {
     *   "preferenceName": "温度偏好",
     *   "preferenceType": "temperature",
     *   "preferenceValue": 23.7
     * }
     */
    @PutMapping("/{roomId}/{preferenceId}")
    public ResponseEntity<Preference> updatePreference(@PathVariable String roomId,
                                                       @PathVariable String preferenceId,
                                                       @RequestBody Preference preference) {
        Preference updatedPreference = preferenceService.updatePreference(roomId, preferenceId, preference);
        //发布MQTT消息
        if(updatedPreference != null)
        {
            // 构建 preferenceId 和 preferenceValue 的 JSON
            String json = JsonUtil.buildPreferenceValue(preferenceId, updatedPreference.getPreferenceValue());
            myMqttService.publishMessage("preferences", json);
        }
        return updatedPreference != null ? ResponseEntity.ok(updatedPreference) : ResponseEntity.notFound().build();
    }

    /**
     * 删除指定偏好设置
     * DELETE /api/preferences/{preferencesId}
     */
    @DeleteMapping("/{preferenceId}")
    public ResponseEntity<String> deletePreference(@PathVariable String preferenceId) {
        boolean deleted = preferenceService.deletePreference(preferenceId);
        return deleted ? ResponseEntity.ok("✅ 偏好已删除") : ResponseEntity.status(404).body("❌ 偏好不存在");
    }
}

