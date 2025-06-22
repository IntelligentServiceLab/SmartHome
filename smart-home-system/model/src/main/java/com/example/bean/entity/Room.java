package com.example.bean.entity;

import com.fasterxml.jackson.annotation.JsonManagedReference;
import jakarta.persistence.*;
import lombok.Data;
import java.util.Date;
import java.util.List;

@Entity
@Table(name = "rooms")
@Data
public class Room {

    @Id
    @Column(name = "room_id", nullable = false, unique = true)
    private String roomId;      // 房间ID

    private String roomType;    // 房间类型（如：living-room、bedroom、kitchen-room、bathroom、study-room、balcony-room、other）

    private String roomName;    // 房间名称（如：客厅、卧室、厨房、浴室、书房、阳台、其他）

    // 设备列表
    @OneToMany(mappedBy = "room", cascade = CascadeType.ALL, orphanRemoval = true)
    @JsonManagedReference // 阻止无限循环：Room -> SensorList -> Room（停止在 SensorList）
    private List<Device> devices;

    // 阈值列表
    @OneToMany(mappedBy = "room", cascade = CascadeType.ALL, orphanRemoval = true)
    @JsonManagedReference // 阻止无限循环：Room -> SensorList -> Room（停止在 SensorList）
    private List<Threshold> thresholds;

    // 偏好设置列表
    @OneToMany(mappedBy = "room", cascade = CascadeType.ALL, orphanRemoval = true)
    @JsonManagedReference // 阻止无限循环：Room -> SensorList -> Room（停止在 SensorList）
    private List<Preference> preferences;

    // 传感器数据列表
    @OneToMany(mappedBy = "room", cascade = CascadeType.ALL, orphanRemoval = true)
    @JsonManagedReference // 阻止无限循环：Room -> SensorList -> Room（停止在 SensorList）
    private List<Sensor> sensorList;

    @Temporal(TemporalType.TIMESTAMP)
    @Column(name = "created_at", nullable = false, updatable = false)
    private Date createdAt;

    @Temporal(TemporalType.TIMESTAMP)
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
