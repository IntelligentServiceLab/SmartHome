package com.example.smarthomebpmn.service;

import com.example.bean.entity.Device;
import com.example.bean.repository.DeviceRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class ControllerService {

    @Autowired
    private DeviceRepository deviceRepository;

    @Autowired
    private MyManageService myManageService;

    /**
     * 遍历所有设备并发送管理消息
     *
     * @param status 要设置的设备状态（on/off）
     */
    public void manageAllDevices(String status) {
        List<Device> devices = deviceRepository.findAll();
        for (Device device : devices) {
            String deviceId = device.getDeviceId();
            myManageService.manageMessage(deviceId, status);
        }
    }

    /**
     * 给指定设备发送管理消息
     *
     * @param deviceId 设备ID
     * @param status   要设置的状态（on/off）
     */
    public void manageSingleDevice(String deviceId, String status) {
        boolean exists = deviceRepository.existsById(deviceId);
        if (exists) {
            myManageService.manageMessage(deviceId, status);
        } else {
            System.out.println("❌ 设备未找到: " + deviceId);
        }
    }


}
