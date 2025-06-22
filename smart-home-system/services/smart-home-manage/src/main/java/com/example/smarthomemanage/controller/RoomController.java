package com.example.smarthomemanage.controller;

import com.example.bean.entity.Room;
import com.example.smarthomemanage.service.RoomService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/rooms")
public class RoomController {

        @Autowired
        private RoomService roomService;

        /**
         * 获取所有房间列表
         * GET /api/rooms
         */
        @GetMapping
        public ResponseEntity<List<Room>> getAllRooms() {
                List<Room> rooms = roomService.getAllRooms();
                return ResponseEntity.ok(rooms);
        }

        /**
         * 获取某个房间的信息
         * GET /api/rooms/{roomId}
         */
        @GetMapping("/{roomId}")
        public ResponseEntity<Room> getRoomById(@PathVariable String roomId) {
                Room room = roomService.getRoomById(roomId);
                return room != null ? ResponseEntity.ok(room) : ResponseEntity.notFound().build();
        }

        /**
         * 创建新房间
         * POST /api/rooms
         * 请求体示例：
         * {
         *   "roomType": "bedroom",
         *   "roomName": "卧室"
         * }
         */
        @PostMapping
        public ResponseEntity<Room> createRoom(@RequestBody Room room) {
                Room savedRoom = roomService.createRoom(room);
                return savedRoom != null ? ResponseEntity.ok(savedRoom) : ResponseEntity.notFound().build();
        }

        /**
         * 更新指定房间的信息
         * PUT /api/rooms/{roomId}
         * 请求体示例：
         * {
         *   "roomId": "room-001",
         *   "roomType": "bedroom",
         *   "roomName": "主卧"
         * }
         */
        @PutMapping("/{roomId}")
        public ResponseEntity<Room> updateRoom(@PathVariable String roomId, @RequestBody Room room) {
                Room updatedRoom = roomService.updateRoom(roomId, room);
                return updatedRoom != null ? ResponseEntity.ok(updatedRoom) : ResponseEntity.notFound().build();
        }

        /**
         * 删除指定房间
         * DELETE /api/rooms/{roomId}
         */
        @DeleteMapping("/{roomId}")
        public ResponseEntity<Void> deleteRoom(@PathVariable String roomId) {
                boolean deleted = roomService.deleteRoom(roomId);
                return deleted ? ResponseEntity.noContent().build() : ResponseEntity.notFound().build();
        }
}
