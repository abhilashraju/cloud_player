#ifndef QCASUALVM_H
#define QCASUALVM_H

#include "core/sobject_cloud_vm_base.h"
#include<QQmlEngine>
class QCasualVM:public QObjectVM
{
public:
    QCasualVM(const std::string& url);
    QCasualVM();
    static QQmlEngine* qvmEngine();
    void set_param_notify(boost::string_view name,boost::string_view value){
        d->param_handler(name,value);
        // notifier->setProperty("param",name.data());
    }
};
#endif // QCASUALVM_H
