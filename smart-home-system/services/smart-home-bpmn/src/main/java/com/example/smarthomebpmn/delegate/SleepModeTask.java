package com.example.smarthomebpmn.delegate;

import com.example.smarthomebpmn.service.ControllerService;
import org.flowable.engine.delegate.DelegateExecution;
import org.flowable.engine.delegate.JavaDelegate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import org.springframework.web.context.support.SpringBeanAutowiringSupport;

@Component
public class SleepModeTask implements JavaDelegate {

    @Autowired
    private ControllerService controllerService;

    @Override
    public void execute(DelegateExecution execution) {
        SpringBeanAutowiringSupport.processInjectionBasedOnCurrentContext(this);
        System.out.println("🛏️ 睡觉模式开启：");
        System.out.println(" - 关闭客厅灯");
        controllerService.manageSingleDevice("light-001", "off");
        System.out.println(" - 打开卧室小夜灯");
        controllerService.manageSingleDevice("light-002", "on");
        System.out.println(" - 设置空调为睡眠模式");
        controllerService.manageSingleDevice("adc-001", "sleep");
    }
}

