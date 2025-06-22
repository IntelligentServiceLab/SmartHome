package com.example.smarthomebpmn.controller;

import org.flowable.engine.RuntimeService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.HashMap;
import java.util.Map;

@RestController
@RequestMapping("/api/flow")
public class FlowController {

    @Autowired
    private RuntimeService runtimeService;

    /**
     * 启动指定模式流程
     * POST /api/flow/mode/{mode}
     *
     * @param mode 模式名称（home、away、sleep）
     * @return 启动成功提示
     */
    @PostMapping("/mode/{mode}")
    public ResponseEntity<String> startMode(@PathVariable String mode) {
        Map<String, Object> vars = new HashMap<>();
        vars.put("userStatus", mode);

        runtimeService.startProcessInstanceByKey("homeModeProcess", vars);
        return ResponseEntity.ok("✅ 流程已启动: " + mode);
    }
}
