package com.example.bean.entity;

import com.fasterxml.jackson.annotation.JsonBackReference;
import jakarta.persistence.*;
import lombok.Data;
import java.util.Date;

@Entity
@Table(name = "thresholds") // 映射到 thresholds 表
@Data
public class Threshold {

    @Id
    @Column(name = "threshold_id", nullable = false, unique = true)
    private String thresholdId;
    private String thresholdType;  // 阈值类型(eg. "temperature", "humidity", "light")
    private String thresholdName;  // 阈值名称（例如 "温度阈值、湿度阈值、光照阈值"）
    private Double lowThresholdValue;
    private Double highThresholdValue;

    @ManyToOne
    @JoinColumn(name = "room_id")
    @JsonBackReference // 阻止无限循环：Sensor -> Room -> SensorList（回头不走）
    private Room room; // 阈值所属房间

    @Temporal(TemporalType.TIMESTAMP) // 创建时间戳
    @Column(name = "created_at", nullable = false, updatable = false)
    private Date createdAt;

    @Temporal(TemporalType.TIMESTAMP) // 更新时间戳
    @Column(name = "updated_at", nullable = false)
    private Date updatedAt;

    @PrePersist
    protected void onCreate() {
        this.createdAt = new Date();
        this.updatedAt = this.createdAt;
    }

    @PreUpdate
    protected void onUpdate() {
        this.updatedAt = new Date();
    }
}
