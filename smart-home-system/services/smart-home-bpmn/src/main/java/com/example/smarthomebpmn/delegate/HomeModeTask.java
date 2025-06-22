package com.example.smarthomebpmn.delegate;

import com.example.smarthomebpmn.service.ControllerService;
import org.flowable.engine.delegate.DelegateExecution;
import org.flowable.engine.delegate.JavaDelegate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import org.springframework.web.context.support.SpringBeanAutowiringSupport;

@Component
public class HomeModeTask implements JavaDelegate {

    @Autowired
    private ControllerService controllerService;

    @Override
    public void execute(DelegateExecution execution) {
        SpringBeanAutowiringSupport.processInjectionBasedOnCurrentContext(this);
        System.out.println("🏠 回家模式开启：");
        System.out.println(" - 打开客厅灯");
        controllerService.manageSingleDevice("light-001", "on");
        System.out.println(" - 打开空调");
        controllerService.manageSingleDevice("adc-001", "on");
        System.out.println(" - 播放欢迎语音");
    }
}

