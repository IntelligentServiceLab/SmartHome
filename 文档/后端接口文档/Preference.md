------

### ✅ 获取指定房间的所有偏好

```http
GET /api/preferences/room/{roomId}
```

------

### ✅ 获取单个偏好信息

```http
GET /api/preferences/{preferenceId}
```

------

### ✅ 向房间添加偏好

```http
POST /api/preferences/{roomId}
Content-Type: application/json

{
  "preferenceID": "temperature-001",
  "preferenceName": "温度偏好",
  "preferenceType": "temperature",
  "preferenceValue": 22.3
}
```

------

### ✅ 更新指定偏好设置

```http
PUT /api/preferences/{roomId}/{preferenceId}
Content-Type: application/json

{
  "preferenceName": "温度偏好",
  "preferenceType": "temperature",
  "preferenceValue": 23.7
}
```

------

### ✅ 删除指定偏好设置

```http
DELETE /api/preferences/{preferenceId}
```

------
