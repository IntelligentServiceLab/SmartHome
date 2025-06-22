------

### ✅ 获取指定房间的设备列表

```http
GET /api/devices/room/{roomId}
```

------

### ✅ 获取设备详情

```http
GET /api/devices/{deviceId}
```

------

### ✅ 向房间添加设备

```http
POST /api/devices/{roomId}
Content-Type: application/json

{
  "deviceId": "fan-001",
  "deviceName": "空调",
  "deviceType": "AC",
  "deviceStatus": "off"
}
```

------

### ✅ 更新设备信息

```http
PUT /api/devices/{roomId}/{deviceId}
Content-Type: application/json

{
  "deviceName": "空调",
  "deviceType": "AC",
  "deviceStatus": "on"
}
```

------

### ✅ 删除设备

```http
DELETE /api/devices/{deviceId}
```

------

### ✅ 切换设备状态

```http
POST /api/devices/{deviceId}/toggle
```

------

