### ✅ 获取指定房间的所有阈值

```http
GET /api/thresholds/room/{roomId}
```

------

### ✅ 获取单个阈值信息

```http
GET /api/thresholds/{thresholdId}
```

------

### ✅ 向房间添加阈值

```http
POST /api/thresholds/{roomId}
Content-Type: application/json

{
  "thresholdID": "temperature-001",
  "thresholdName": "温度阈值",
  "thresholdType": "temperature",
  "highThresholdValue": 22.3,
  "lowThresholdValue": 10
}
```

------

### ✅ 更新指定阈值设置

```http
PUT /api/thresholds/{roomId}/{thresholdId}
Content-Type: application/json

{
  "thresholdName": "温度阈值",
  "thresholdType": "temperature",
  "highThresholdValue": 22.3,
  "lowThresholdValue": 10
}
```

------

### ✅ 删除指定阈值设置

```http
DELETE /api/thresholds/{thresholdId}
```

------