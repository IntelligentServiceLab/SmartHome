package com.example.smarthomemanage.service;

import com.example.bean.entity.Sensor;
import com.example.bean.repository.SensorRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class SensorService {
    @Autowired
    private SensorRepository sensorRepository;
    /**
     * 获取指定房间指定类型的传感器数据
     *
     * @param roomId 房间ID
     * @param sensorType 房间ID
     * @return 房间内指定类型的Top100条传感器数据
     */
    public List<Sensor> getSensorsByRoomId(String roomId, String sensorType)
    {
        return sensorRepository.findTop100ByRoom_RoomIdAndSensorTypeOrderByCreatedAtDesc(roomId, sensorType);
    }

}
