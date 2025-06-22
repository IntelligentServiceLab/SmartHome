------

### ✅ 获取房间列表

```http
GET /api/rooms
```

------

### ✅ 获取单个房间

```http
GET /api/rooms/{roomId}
```

------

### ✅ 创建房间

```http
POST /api/rooms
Content-Type: application/json

{
  "roomId": "room-001",
  "roomType": "bedroom",
  "roomName": "主卧"
}
```

------

### ✅ 更新房间

```http
PUT /api/rooms/{roomId}
Content-Type: application/json

{
  "roomType": "bedroom",
  "roomName": "主卧"
}
```

------

### ✅ 删除房间

```http
DELETE /api/rooms/{roomId}
```

------

