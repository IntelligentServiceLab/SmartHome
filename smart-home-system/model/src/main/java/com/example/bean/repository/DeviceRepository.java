package com.example.bean.repository;

import com.example.bean.entity.Device;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

public interface DeviceRepository extends JpaRepository<Device, String> {
    List<Device>  findByRoom_RoomId(String roomId);
}


