#pragma once
#include <boost/utility/string_view.hpp>
#include "sobject_cloud_vm_base.h"
#pragma GCC visibility push(default)
template<typename Device,typename DEBUG_LOGGER>
struct device_capabilities_impl{
     //assumes copy constructable logger
     static DEBUG_LOGGER logger() {return DEBUG_LOGGER();}
     using VM=typename Device::VM;
     using ScreenManager=typename Device::ScreenManager;
     static std::string getModel(){return Device::getModel();}
     static std::string getCountry(){return Device::getCountry();}
     static std::string getLanguage(){return Device::getLanguage();}
     static std::string getAuthToken(std::string name){return Device::getAuthToken(name);}
     static std::tuple<std::string,std::string,std::string> get_auth_data(){return Device::get_auth_data();}
     static std::string getCloudId(){ return Device::getCloudId(); }
     static std::string getCloudVersion(){ return Device::getCloudVersion(); }
     static std::string expType(){ return Device::expType(); }
     static std::string get_proxy(){return Device::get_proxy();}
     static bool getSmartTasksStatus(){return Device::getSmartTasksStatus();}
     static std::string getRootUrl(){return  Device::getRootUrl();}
     static std::chrono::milliseconds timeout(){return Device::timeout();}
     static std::unique_ptr<VM> createVM(){ return Device::createVM();}
     static std::unique_ptr<ScreenManager> createScreenManager(){return Device::createScreenManager();}
     template<typename Handler>
     static auto make_app_handler(Handler&& handler)
     {
      return  Device::make_app_handler(std::forward<Handler>(handler));
     }
     
};
#pragma GCC visibility pop