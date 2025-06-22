package com.example.smarthomebpmn.service;

import com.example.smarthomebpmn.feign.ManageFeignClient;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class MyManageService {

    @Autowired
    private ManageFeignClient manageFeignClient;

    /**
     * 管理消息
     *
     * @param deviceId 设备ID
     * @param status 设备状态
     * @return 管理结果
     */

    public void manageMessage(String deviceId, String status) {
        String result = manageFeignClient.sendManageMessage(deviceId, status);
        System.out.println("管理结果：" + "设备" + deviceId + "状态" + result);
    }
}

