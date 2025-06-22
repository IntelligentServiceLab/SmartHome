package com.example.bean.entity;

import com.fasterxml.jackson.annotation.JsonBackReference;
import jakarta.persistence.*;
import lombok.Data;
import java.util.Date;

@Entity
@Table(name = "sensors") // 映射到 data 表
@Data
public class Sensor {

    @Id
    @Column(name = "sensor_id", nullable = false, unique = true)
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long sensorId;

    private String sensorType;

    private Double sensorValue;

    @Temporal(TemporalType.TIMESTAMP) // 创建时间戳
    @Column(name = "created_at", nullable = false, updatable = false)
    private Date createdAt;

    @ManyToOne
    @JoinColumn(name = "room_id")
    @JsonBackReference // 阻止无限循环：Sensor -> Room -> SensorList（回头不走）
    private Room room; // 数据所在的房间

    @PrePersist
    protected void onCreate() {
        this.createdAt = new Date();
    }
}
