package com.example.smarthomemanage.service;

import com.example.bean.entity.Room;
import com.example.bean.entity.Threshold;
import com.example.bean.repository.RoomRepository;
import com.example.bean.repository.ThresholdRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;

@Service
public class ThresholdService {

    @Autowired
    private ThresholdRepository  thresholdRepository;

    @Autowired
    private RoomRepository roomRepository;

    /**
     * 获取指定房间的所有阈值
     *
     * @param roomId 房间ID
     * @return 房间内的所有阈值设置
     */
    public List<Threshold> getThresholdsByRoomId(String roomId) {
        return thresholdRepository.findByRoom_RoomId(roomId);
    }

    /**
     * 获取指定阈值的详细信息
     *
     * @param thresholdId 阈值ID
     * @return 对应的阈值对象；如果阈值对象不存在，返回null
     */
    public Threshold getThresholdById(String thresholdId) {
        return thresholdRepository.findById(thresholdId).orElse(null);
    }

    /**
     * 添加阈值到指定房间
     *
     * @param roomId 房间ID
     * @param threshold 要添加的阈值设置
     * @return 保存后的阈值设置
     */
    public Threshold addThresholdToRoom(String roomId, Threshold threshold) {
        // 查找对应房间
        Optional<Room> roomOptional = roomRepository.findById(roomId);
        if (roomOptional.isPresent()) {
            threshold.setRoom(roomOptional.get());
            return thresholdRepository.save(threshold);
        }
        throw new IllegalArgumentException("❌ 房间不存在");
    }

    /**
     * 更新阈值设置
     *
     * @param roomId       房间ID
     * @param preferenceId 阈值ID
     * @param updateThreshold 更新后的阈值设置
     * @return 更新后的阈值设置
     */
    public Threshold updateThreshold(String roomId, String preferenceId, Threshold updateThreshold) {
        Optional<Threshold> thresholdOpt = thresholdRepository.findById(preferenceId);
        if (thresholdOpt.isPresent()) {
            Threshold threshold = thresholdOpt.get();
            threshold.setThresholdName(updateThreshold.getThresholdName());
            threshold.setThresholdType(updateThreshold.getThresholdType());
            threshold.setHighThresholdValue(updateThreshold.getHighThresholdValue());
            threshold.setLowThresholdValue(updateThreshold.getLowThresholdValue());
            return thresholdRepository.save(threshold);
        }
        throw new IllegalArgumentException("❌ 阈值不存在");
    }

    /**
     * 删除阈值设置
     * @param thresholdId 阈值ID
     * @return 是否成功删除阈值
     */
    public Boolean deleteThreshold(String thresholdId) {
        if (thresholdRepository.existsById(thresholdId)) {
            thresholdRepository.deleteById(thresholdId);
            return true;
        }
        return false;
    }
}
