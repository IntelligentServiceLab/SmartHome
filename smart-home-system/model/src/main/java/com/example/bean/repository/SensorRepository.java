package com.example.bean.repository;

import com.example.bean.entity.Sensor;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

public interface SensorRepository extends JpaRepository<Sensor, Long> {
    List<Sensor> findTop100ByRoom_RoomIdAndSensorTypeOrderByCreatedAtDesc(String roomId, String sensorType);

}
