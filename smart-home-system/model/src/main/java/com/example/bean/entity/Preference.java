package com.example.bean.entity;

import com.fasterxml.jackson.annotation.JsonBackReference;
import jakarta.persistence.*;
import lombok.Data;
import java.util.Date;

@Entity
@Table(name = "preferences") // 映射到 preferences 表
@Data
public class Preference {

    @Id
    @Column(name = "preference_id", nullable = false, unique = true)
    private String preferenceId;    // 偏好ID

    private String preferenceType;  // 偏好类型（eg. "temperature", "humidity", "light"）

    private String preferenceName;  // 偏好名称（例如 "温度偏好、湿度偏好、光照偏好"）

    private Double preferenceValue; // 偏好值（例如 22、23、24）

    @ManyToOne
    @JoinColumn(name = "room_id")
    @JsonBackReference // 阻止无限循环：Sensor -> Room -> SensorList（回头不走）
    private Room room; // 偏好设置所在房间

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
