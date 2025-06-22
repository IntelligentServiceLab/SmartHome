package com.example.smarthomebpmn.delegate;

import com.example.smarthomebpmn.service.ControllerService;
import org.flowable.engine.delegate.DelegateExecution;
import org.flowable.engine.delegate.JavaDelegate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import org.springframework.web.context.support.SpringBeanAutowiringSupport;

@Component
public class AwayModeTask implements JavaDelegate {

    @Autowired
    private ControllerService controllerService;

    @Override
    public void execute(DelegateExecution execution) {
        SpringBeanAutowiringSupport.processInjectionBasedOnCurrentContext(this);
        System.out.println("🚪 离家模式开启：");
        System.out.println(" - 关闭所有设备");
        controllerService.manageAllDevices("off");
        System.out.println(" - 设置安防警报");
    }
}

