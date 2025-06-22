package com.example.smarthomebpmn.feign;

import org.springframework.cloud.openfeign.FeignClient;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;


@FeignClient(name = "smart-home-manage")
public interface ManageFeignClient {
    @PostMapping("/api/devices/{deviceId}/{status}")
    String sendManageMessage( @PathVariable String deviceId, @PathVariable String status);
}

