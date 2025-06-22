package com.example.smarthomemanage.utils;

import com.fasterxml.jackson.databind.ObjectMapper;
import java.util.HashMap;
import java.util.Map;

public class JsonUtil {

    private static final ObjectMapper objectMapper = new ObjectMapper();

    /**
     * 构建：deviceId + deviceStatus
     *
     * @param deviceId 设备的唯一标识符（如 "fan-001"）
     * @param deviceStatus 设备的状态（如 "on", "off", "auto" 等）
     * @return 返回一个包含 deviceId 和 deviceStatus 的 JSON 字符串
     */
    public static String buildDeviceStatus(String deviceId, String deviceStatus) {
        try {
            Map<String, Object> map = new HashMap<>();
            map.put("deviceId", deviceId);  // 将 deviceId 添加到 map 中
            map.put("deviceStatus", deviceStatus);  // 将 deviceStatus 添加到 map 中
            return objectMapper.writeValueAsString(map);  // 将 map 转换为 JSON 字符串并返回
        } catch (Exception e) {
            e.printStackTrace();
            return "{}";  // 如果发生异常，返回一个空的 JSON 对象
        }
    }

    /**
     * 构建：thresholdId + lowThreshold + highThreshold
     *
     * @param thresholdId 阈值标识符（如 "pref-001"）
     * @param lowThresholdValue 低阈值（如 28.5）
     * @param highThresholdValue 高阈值（如 30.0）
     * @return 返回一个包含 thresholdId、lowThreshold 和 highThreshold 的 JSON 字符串
     */
    public static String buildThresholdData(String thresholdId, Double lowThresholdValue, Double highThresholdValue) {
        try {
            Map<String, Object> map = new HashMap<>();
            map.put("thresholdId", thresholdId);  // 将 thresholdId 添加到 map 中
            map.put("lowThreshold", lowThresholdValue);  // 将低阈值添加到 map 中
            map.put("highThreshold", highThresholdValue);  // 将高阈值添加到 map 中
            return objectMapper.writeValueAsString(map);  // 将 map 转换为 JSON 字符串并返回
        } catch (Exception e) {
            e.printStackTrace();
            return "{}";  // 如果发生异常，返回一个空的 JSON 对象
        }
    }

    /**
     * 构建：preferenceId（字符串） + preferenceValue
     *
     * @param preferenceId 偏好设置标识符（字符串类型，如 "pref-001"）
     * @param preferenceValue 设备的当前值（如温度偏好值、目标值等）
     * @return 返回一个包含 preferenceId 和 preferenceValue 的 JSON 字符串
     */
    public static String buildPreferenceValue(String preferenceId, Double preferenceValue) {
        try {
            Map<String, Object> map = new HashMap<>();
            map.put("preferenceId", preferenceId);  // 将 preferenceId 添加到 map 中
            map.put("preferenceValue", preferenceValue);  // 将 preferenceValue 添加到 map 中
            return objectMapper.writeValueAsString(map);  // 将 map 转换为 JSON 字符串并返回
        } catch (Exception e) {
            e.printStackTrace();
            return "{}";  // 如果发生异常，返回一个空的 JSON 对象
        }
    }
}
