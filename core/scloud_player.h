#pragma once
#include <memory>
#include "urilite.h"
#include "boost_utilities.h"
#pragma GCC visibility push(default)
template<typename T>
class SCloudPlayerPrivate;
template<typename DeviceCapability>
class SCloudPlayer
{
    public:
    using dev_cap=DeviceCapability;
    SCloudPlayer();
    void start(const std::string& url,std::function<void(SCloudPlayer*,bool)> callback = std::function<void(SCloudPlayer*,bool)>());
    bool load(std::stringstream& data);
    void load_default_screen();
    ~SCloudPlayer();
    private:
    std::unique_ptr<SCloudPlayerPrivate<DeviceCapability>> d;

};
#include "scloud_player.cpp"
#pragma GCC visibility pop