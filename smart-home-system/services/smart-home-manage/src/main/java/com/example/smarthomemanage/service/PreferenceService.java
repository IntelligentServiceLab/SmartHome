package com.example.smarthomemanage.service;

import com.example.bean.entity.Preference;
import com.example.bean.entity.Room;
import com.example.bean.repository.PreferenceRepository;
import com.example.bean.repository.RoomRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;

@Service
public class PreferenceService {

    @Autowired
    private PreferenceRepository preferenceRepository;

    @Autowired
    private RoomRepository roomRepository;

    /**
     * 获取指定房间的所有偏好
     *
     * @param roomId 房间ID
     * @return 房间内的所有偏好设置
     */
    public List<Preference> getPreferencesByRoomId(String roomId) {
        return preferenceRepository.findByRoom_RoomId(roomId);
    }

    /**
     * 获取指定偏好的详细信息
     *
     * @param preferenceId 偏好设置 ID
     * @return 对应的偏好设置；如果偏好不存在，返回null
     */
    public Preference getPreferenceById(String preferenceId) {
        return preferenceRepository.findById(preferenceId).orElse(null);
    }

    /**
     * 添加偏好到指定房间
     *
     * @param roomId 房间ID
     * @param Preference 要添加的偏好设置
     * @return 保存后的偏好设置
     */
    public Preference addPreferenceToRoom(String roomId, Preference Preference) {
        // 查找对应房间
        Optional<Room> roomOptional = roomRepository.findById(roomId);
        if (roomOptional.isPresent()) {
            Preference.setRoom(roomOptional.get());
            return preferenceRepository.save(Preference);
        }
        throw new IllegalArgumentException("❌ 房间不存在");
    }

    /**
     * 更新偏好设置
     *
     * @param roomId       房间ID
     * @param preferenceId 偏好设置 ID
     * @param updatedPreference 更新后的偏好设置
     * @return 更新后的偏好设置
     */
    public Preference updatePreference(String roomId, String preferenceId, Preference updatedPreference) {
        Optional<Preference> preferenceOpt = preferenceRepository.findById(preferenceId);
        if (preferenceOpt.isPresent()) {
            Preference preference = preferenceOpt.get();
            preference.setPreferenceName(updatedPreference.getPreferenceName());
            preference.setPreferenceType(updatedPreference.getPreferenceType());
            preference.setPreferenceValue(updatedPreference.getPreferenceValue());
            return preferenceRepository.save(preference);
        }
        throw new IllegalArgumentException("❌ 偏好不存在");
    }

    /**
     * 删除偏好设置
     * @param preferenceId 偏好设置 ID
     * @return 是否成功删除偏好设置
     */
    public Boolean deletePreference(String preferenceId) {
        if (preferenceRepository.existsById(preferenceId)) {
            preferenceRepository.deleteById(preferenceId);
            return true;
        }
        return false;
    }
}
