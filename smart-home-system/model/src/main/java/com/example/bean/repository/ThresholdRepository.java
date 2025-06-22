package com.example.bean.repository;

import com.example.bean.entity.Threshold;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

public interface ThresholdRepository extends JpaRepository<Threshold, String> {
    //根据房间 ID 获取该房间下所有阈值
    List<Threshold> findByRoom_RoomId(String roomId);

}
