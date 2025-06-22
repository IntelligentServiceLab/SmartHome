package com.example.smarthomemanage.service;

import com.example.bean.entity.Device;
import com.example.bean.entity.Room;
import com.example.bean.repository.DeviceRepository;
import com.example.bean.repository.RoomRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;

@Service
public class DeviceService {

    @Autowired
    private DeviceRepository deviceRepository;

    @Autowired
    private RoomRepository roomRepository;

    /**
     * 获取指定房间的所有设备
     *
     * @param roomId 房间ID
     * @return 房间内的设备列表
     */
    public List<Device> getDevicesByRoomId(String roomId) {
        return deviceRepository.findByRoom_RoomId(roomId);
    }

    /**
     * 获取指定设备的详细信息
     *
     * @param deviceId 设备ID
     * @return 对应的设备对象；如果设备不存在，返回null
     */
    public Device getDeviceById(String deviceId) {
        return deviceRepository.findById(deviceId).orElse(null);
    }

    /**
     * 添加设备到指定房间
     *
     * @param roomId 房间ID
     * @param device 要添加的设备对象
     * @return 保存后的设备对象
     */
    public Device addDeviceToRoom(String roomId, Device device) {
        Optional<Room> roomOpt = roomRepository.findById(roomId);
        if (roomOpt.isPresent()) {
            device.setRoom(roomOpt.get());
            device.setDeviceStatus("off");  // 默认设备状态为 off
            return deviceRepository.save(device);
        }
        throw new IllegalArgumentException("❌ 房间不存在");
    }

    /**
     * 更新设备信息
     *
     * @param roomId       房间ID
     * @param deviceId     设备ID
     * @param updatedDevice 更新后的设备对象
     * @return 更新后的设备对象
     */
    public Device updateDevice(String roomId, String deviceId, Device updatedDevice) {
        Optional<Device> deviceOpt = deviceRepository.findById(deviceId);
        if (deviceOpt.isPresent()) {
            Device device = deviceOpt.get();
            device.setDeviceName(updatedDevice.getDeviceName());
            device.setDeviceType(updatedDevice.getDeviceType());
            return deviceRepository.save(device);
        }
        throw new IllegalArgumentException("❌ 设备不存在");
    }

    /**
     * 删除设备
     *
     * @param deviceId 设备ID
     * @return 是否成功删除设备
     */
    public boolean deleteDevice(String deviceId) {
        if (deviceRepository.existsById(deviceId)) {
            deviceRepository.deleteById(deviceId);
            return true;
        }
        return false;
    }

    /**
     * 切换设备的开/关状态
     *
     * @param deviceId 设备ID
     * @return 设备的新状态（开/关）
     */
    public String toggleDevice(String deviceId) {
        Optional<Device> deviceOpt = deviceRepository.findById(deviceId);
        if (deviceOpt.isPresent()) {
            Device device = deviceOpt.get();
            String currentStatus = device.getDeviceStatus();
            String newStatus = currentStatus.equalsIgnoreCase("on") ? "off" : "on";
            device.setDeviceStatus(newStatus);
            deviceRepository.save(device);
            return newStatus;
        }
        throw new IllegalArgumentException("❌ 设备未找到");
    }

    /**
     * 设置设备状态为指定值（开/关）
     *
     * @param deviceId 设备ID
     * @param status   目标状态（开/关）
     * @return 更新后的设备状态（开/关）
     */
    public String setDeviceStatus(String deviceId, String status) {
        Optional<Device> deviceOpt = deviceRepository.findById(deviceId);
        if (deviceOpt.isPresent()) {
            Device device = deviceOpt.get();
            String normalizedStatus = status.equalsIgnoreCase("on") ? "on" : "off";
            device.setDeviceStatus(normalizedStatus);
            deviceRepository.save(device);
            return normalizedStatus;
        }
        throw new IllegalArgumentException("❌ 设备未找到");
    }

}
