package com.example.bean.repository;

import com.example.bean.entity.Preference;
import org.springframework.data.jpa.repository.JpaRepository;
import java.util.List;

public interface PreferenceRepository extends JpaRepository<Preference, String> {
    List<Preference> findByRoom_RoomId(String roomId);
}
