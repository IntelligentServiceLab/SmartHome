package com.example.bean.entity;

import com.fasterxml.jackson.annotation.JsonBackReference;
import jakarta.persistence.*;
import lombok.Data;
import java.util.Date;

@Entity
@Table(name = "devices") // 映射到 devices 表
@Data
public class Device {

    @Id
    @Column(name = "device_id", nullable = false, unique = true)
    private String deviceId;    // 设备ID

    private String deviceType;  // 设备类型（light、fan、curtain、ac、others）

    private String deviceName;  // 设备名称

    private String deviceStatus; // 设备状态

    //设备模式   自动模式和手动模式

    @ManyToOne
    @JoinColumn(name = "room_id")
    @JsonBackReference // 阻止无限循环：Sensor -> Room -> SensorList（回头不走）
    private Room room; // 设备所属房间

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

