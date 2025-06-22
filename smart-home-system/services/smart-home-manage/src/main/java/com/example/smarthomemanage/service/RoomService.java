package com.example.smarthomemanage.service;

import com.example.bean.entity.Room;
import com.example.bean.repository.RoomRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;

@Service
public class RoomService {

    @Autowired
    private RoomRepository roomRepository;

    /**
     * 获取所有房间列表
     *
     * @return 包含所有房间的列表
     */
    public List<Room> getAllRooms() {
        return roomRepository.findAll();
    }

    /**
     * 根据房间ID获取房间信息
     *
     * @param roomId 房间ID（主键）
     * @return 对应的房间对象；如果不存在则返回 null
     */
    public Room getRoomById(String roomId) {
        return roomRepository.findById(roomId).orElse(null);
    }

    /**
     * 创建一个新的房间
     *
     * @param room 房间实体对象（包含房间ID、房间名、类型等）
     * @return 创建成功的房间对象；如果房间不存在则返回 null
     */
    public Room createRoom(Room room) {
        if (roomRepository.existsById(room.getRoomId())) {
            return null;
        }

        // 初始化关联列表为 null，防止前端误传旧数据
        room.setDevices(null);
        room.setSensorList(null);
        room.setPreferences(null);
        room.setThresholds(null);
        return roomRepository.save(room);
    }

    /**
     * 更新指定房间的信息（目前支持更新房间名与类型）
     *
     * @param roomId       要更新的房间ID
     * @param updatedRoom  包含更新字段的房间对象
     * @return 更新成功后的房间对象；如果房间不存在则返回 null
     */
    public Room updateRoom(String roomId, Room updatedRoom) {
        Optional<Room> optionalRoom = roomRepository.findById(roomId);
        if (!optionalRoom.isPresent()) {
            return null;
        }

        Room existingRoom = optionalRoom.get();

        // 只更新允许字段
        if (updatedRoom.getRoomName() != null) {
            existingRoom.setRoomName(updatedRoom.getRoomName());
        }
        if (updatedRoom.getRoomType() != null) {
            existingRoom.setRoomType(updatedRoom.getRoomType());
        }

        return roomRepository.save(existingRoom);
    }

    /**
     * 删除指定房间（及其关联的设备、传感器等）
     *
     * @param roomId 房间ID
     * @return 删除成功返回 true；若房间不存在返回 false
     */
    public boolean deleteRoom(String roomId) {
        if (!roomRepository.existsById(roomId)) {
            return false;
        }
        roomRepository.deleteById(roomId);
        return true;
    }
}
