#include "scloud_player.h"
#include "shttp_client.hpp"
#include "sflatmap.h"
// #include "nlohmann/json.hpp"
#include <regex>
#include <iterator>
#include "utf8.h"

namespace {
    inline auto replaced_str(auto str,auto first,auto replaced){
        auto ref=std::string{str.data(),str.length()};
        std::regex ampreg(first);
        return  std::regex_replace(ref, ampreg, replaced);
    }
    inline auto validated_utf8(auto str){
        std::string temp;
        utf8::replace_invalid(str.begin(), str.end(), back_inserter(temp));
        return temp;
    }
    inline auto removed_escapes(auto str){
         return validated_utf8(replaced_str(std::forward<decltype(str)>(str),R"(&.*;)","&"));
    }
    template <typename HeadFunc>
    auto chain(HeadFunc head)
    {
        return [=](auto&&... v){
            return head(std::forward<decltype (v)>(v)...);

        };
    }

    template <typename HeadFunc,typename... TailFunc>
    auto chain(HeadFunc&& head, TailFunc&&... tail)
    {
        return [=](auto&&... v){
            auto r = head(std::forward<decltype (v)>(v)...);
            if(r) return r;
            return chain(tail...)(std::forward<decltype (v)>(v)...);
        };
    }
    struct CurrentErrorStatus{
        std::string error;
        std::string status;
        static CurrentErrorStatus& getGlobalErrorStatus(){
            static CurrentErrorStatus gerrsts;
            return gerrsts;
        }
    };
    template <typename Func>
    std::string replace(std::string s,std::regex  reg,Func func){
        std::smatch match;
        bool stop=false;
        while (!stop && std::regex_search(s, match, reg)) {
            std::string param=match.str(); 
            std::string formatter="$`";
            formatter += func(boost::string_view{param.c_str()+1,param.length()-2},stop);
            formatter += "$'";
            s = match.format(
                formatter); 
        }
        return s;
    }
    // using getLanguage=dev_cap::getLanguage;
    // using getLanguage=dev_cap::getCountry;
    // using getLanguage=dev_cap::getModel;
    std::string getStatus(){
        return CurrentErrorStatus::getGlobalErrorStatus().status;
    }
    std::string getError(){
        return CurrentErrorStatus::getGlobalErrorStatus().error;
    }
   
}
template<typename DeviceCapability>
class SCloudPlayerPrivate
{
public:
    using dev_cap=DeviceCapability;
    using Player = SCloudPlayer<dev_cap>;
    std::unique_ptr<typename dev_cap::VM> cVM{dev_cap::createVM()};
    std::unique_ptr<typename dev_cap::ScreenManager> screenManager{dev_cap::createScreenManager()};
    using child_xmls = SFlatMap<std::string,std::unique_ptr<Player>>;
    child_xmls mChildren;
    std::vector<std::unique_ptr<Player>> mPostChildren;
    std::vector<Ui::Parameter> params_;
    void add_parameter(Ui::Parameter param)
    {
        auto iter = std::find_if(begin(params_),end(params_),[&](auto v){ 
            return  v.key== param.key;
            });
        if(iter== end(params_)){
            params_.push_back(std::move(param));
            return;
        }
        *iter=std::move(param);
        // print_params();
    }
    void remove_parameter(const std::string& key)
    {
        params_.erase(std::remove_if(begin(params_),end(params_),[&](auto v){ return v.key== key;}),end(params_));
    }
    std::string get_param(boost::string_view key){
        // print_params();
        auto iter = std::find_if(begin(params_),end(params_),[&](auto v){
             return  v.key== key;
             });
        if(iter!= end(params_)){
            return iter->value;
        }
        //assert(!"parameter not found");
        std::string notfoundError(key.data(),key.length());
        notfoundError.append(": not found");
        show_error(notfoundError);
        return std::string{};
        
    }
    void print_params(){
        dev_cap::logger()<<"****************params****************************";
        for(auto& p: params_){
            dev_cap::logger()<<p.key<<": "<<p.value;
        }
        dev_cap::logger()<<"****************params-end****************************";
    }

    void show_error(boost::string_view detail){
        CurrentErrorStatus::getGlobalErrorStatus()={std::string{detail.data(),detail.length()},std::string()};
        load_native_screen("error_screen");
        // sApp->getApp("cloud_app")->removeState("cloud::busy_screen");
    }
    void movetoInternalScreen(boost::string_view screen){        
        if(screen.starts_with("prev(")){
            int patternlength= "prev("_sv.length();
            auto dest= screen.substr(patternlength,screen.length()-patternlength-1);
            if(dest.starts_with('#')){
                auto target= dest.substr(1,dest.length()-1);
                auto toState = std::string{target.data(),target.length()};
                if(screenManager->moveBackToScreen(toState)){
                    return;
                }
                if(screenManager->moveBackToScreen(cVM->defaultScreen())){
                    return;
                }
                screenManager->popScreen();
                return;
            }
            
            int unwind = atoi(dest.data());
            while(unwind-- > 0){
                screenManager->popScreen();
            }
            return;
        }
        if(screen=="end"){
            screenManager->removeAllScreens();
            return; 
        }
        if(screenManager->bringToFront(std::string{screen.data(),screen.length()}))
        {
            return ;
        }
        screenManager->addScreen(cVM->getObject("screen",screen));    
        
    }
    void postRequest(boost::string_view url)
    {
        auto newurl = substituteParams(removed_escapes(url));
        
        urilite::uri remotepath =urilite::uri::parse(newurl);
        // auto create_body=[&]()
        // {
        //     using json = nlohmann::json;
        //     json js;
        //     for(auto& param: params_){
        //         js[param.key]= param.value;
        //     }
        //     return js.dump();
        // };
        auto extractvalue=[](auto param){
            if(param.type=="nameVal"){
                auto iter = std::find(begin(param.value),end(param.value),'|');
                // assert(iter != end(param.value)&& "name value param should be sperated by |");
                if(iter != end(param.value)){
                    std::string v;
                    std::copy(++iter,end(param.value),std::back_inserter(v));
                    dev_cap::logger()<<v;
                    return v;
                }
            }
            return param.value;
        };
        auto create_body=[&]()
        {
            print_params();
            std::map<std::string,std::string> bodyparams;
            for(auto& param: params_){
               bodyparams[param.key]= urilite::uri::encode(extractvalue(param));
            }
            Ui::Payload payload(begin(bodyparams),end(bodyparams));
            return payload.content;
        };
        dev_cap::logger()<<"Post URL:" <<  R"(curl -i --insecure -X POST --header "Content-Type: text/plain" --header "Authorization: )" <<dev_cap::getAuthToken("svc_sbs")<<R"(" ")"<<newurl<< R"(" --data-raw ")"<<create_body()<<R"(")";
        CurrentErrorStatus::getGlobalErrorStatus()={std::string(),std::string("Downloading")};
        load_native_screen("busy_screen");
        Ui::Post()(remotepath,Ui::Timeout(dev_cap::timeout()),Ui::Body(create_body()),
                            Ui::HttpHeader{{std::string{"Authorization"},dev_cap::getAuthToken("svc_sbs")}},
                            Ui::ContentType{"text/plain"},dev_cap::make_app_handler([=](beast::error_code ec,std::string data){
                    if(ec/* && ec.value() != (int)beast::http::error::partial_message*/){
                    
                        std::string errormsg= Ui::shttp_fail(ec,data.c_str());
                        show_error(boost::string_view{errormsg.c_str(),errormsg.length()});
                        
                        return;
                    }
                    
                    std::stringstream stream;
                    stream<<data;
                    dev_cap::logger()<<data;
                    try{
                        auto player = std::make_unique<Player>();
                        if(!player->load(stream)){
                            show_error(boost::string_view{data.c_str(),data.length()});
                            return;
                        }
                        player->load_default_screen();
                        mPostChildren.emplace_back(std::move(player));
                    }
                    catch(std::exception& e){
                            show_error(boost::string_view{e.what()});
                    }
            }));
        
    }
    void syncParam(SCloudPlayerPrivate* oldp){
        for(auto& p:params_){
            if(p.inherit ){
                auto iter = std::find_if(begin(oldp->params_),end(oldp->params_),[&](auto& v){
                    return v.key == p.key;
                });
                if(iter != end(oldp->params_)){
                    p.value=iter->value;
                }
            }
        }
    }
    std::string substituteParams(std::string urlp){
        
        std::regex pattern(R"((\{([^\}]|\{\})*\}))");
        auto repalced = replace(urlp,pattern,[=](auto truncparam,bool& stop){
                if(truncparam=="$lang"){
                    return dev_cap::getLanguage();
                }
                if(truncparam=="$region"){
                    return dev_cap::getCountry();
                }
                if(truncparam=="$model"){
                    return replaced_str(dev_cap::getModel()," ",R"(%20)");
                }
                if(truncparam=="$status"){
                    return getStatus();
                }
                 if(truncparam=="$error"){
                    stop=true;
                    return getError();
                }
                if(truncparam == "$cloudVer"){
                    return dev_cap::getCloudVersion();
                }
                if(truncparam == "$cloudID"){
                    return dev_cap::getCloudId();
                }
                if(truncparam == "$expType"){
                    return dev_cap::expType();
                }
                if(truncparam[0]=='#'){
                    return get_param(truncparam.substr(1));
                }
                  
                return get_param(truncparam);
                
                // return std::string{truncparam.data(),truncparam.length()};
            });
        // dev_cap::logger()<<"URL: "<< repalced;
        return repalced;
    };

   
    void load_native_screen(boost::string_view screen)
    {
        screenManager->addScreen(screenManager->prepareNativeScreen(cVM->getObject("native_screen",screen),screen));
    }
};
template<typename DeviceCapability>
SCloudPlayer<DeviceCapability>::SCloudPlayer()
:d(std::make_unique<SCloudPlayerPrivate<DeviceCapability>>())
{
    d->cVM->add_error_handler([=](boost::string_view error)
    {
        d->show_error(error);
            
    });

    d->cVM->add_href_handler([=](boost::string_view href,boost::string_view method)
    {
        try{
                
                auto nhrefstr = removed_escapes(href);
                href= boost::string_view(nhrefstr);
            
                auto remote_url = [&](auto href,auto m){
                if(href.starts_with("http")){//remote url 
                    std::string url =d->substituteParams(std::string{href.data(),href.length()});
                    if(m == "GET"){
                        auto iter =d->mChildren.find(url);
                        if(iter != end(d->mChildren)){
                            iter->second->load_default_screen();
                            return true;
                        }
                        auto player = std::make_unique<SCloudPlayer>();
                        auto callback = [=](auto newplayer,bool success)mutable{
                            if(success){
                                newplayer->d->syncParam(this->d.get());
                                return;
                            }
                            d->mChildren.erase_all_if([=](auto& v){
                                return url==v.first;
                            });
                        };
                        player->start(url,callback);
                        this->d->mChildren[url]=std::move(player);
                        return true;
                    }
                    if(m=="POST"){
                        d->postRequest(href);
                        return true;
                    }
                    
                }
                return false ;
                };
                auto inner_screen = [&](auto href,auto ){
                    if(href.starts_with("#")){//remote url 
                        d->movetoInternalScreen(href.substr(1));
                        return true;
                    }
                    return false ; 
                };
                auto control_back = [&](auto href,auto ){
                    if(href.starts_with("control:")){//remote url 
                        d->movetoInternalScreen(href.substr(boost::string_view("control:").length()));
                        return true;
                    }
                    return false ;
                };
                auto relativeuri=[&](auto href,auto ){
                    auto slash=href.starts_with('/')?std::string():std::string("/");
                    std::string uri= d->cVM->xml_base() +slash +std::string{href.data(),href.length()};
                    remote_url(boost::string_view(uri),method);
                    return true ;
                };
                chain(remote_url,
                            inner_screen,
                            control_back,
                            relativeuri
                            )(href,method);
        
            }
            catch(std::exception& e){
                    d->show_error(boost::string_view{e.what()});
            }
              
    });
    d->cVM->add_param_handler([=](boost::string_view param,boost::string_view value)
    {
        auto iter = std::find_if(begin(d->params_),end(d->params_),[&](auto v){
             return  v.key== param;
        });
        if(iter!= end(d->params_)){
            auto param= *iter;
            param.value=std::string{value.data(),value.length()};
            d->add_parameter(param);
            return;
        }
        d->add_parameter(Ui::Parameter{std::string{param.data(),param.length()},std::string{value.data(),value.length()}});
    });
    d->cVM->add_param_get_handler([=](boost::string_view param)
    {
        return d->get_param(param);
    });
    d->cVM->add_param_replacer([=](std::string text){
        return d->substituteParams(text);
    });
    d->cVM->add_absolute_url_maker([=](boost::string_view relurl){
        if(relurl.empty())return std::string();
        if(relurl.starts_with("http")){
            return std::string{relurl.data(),relurl.length()};
        }
        auto slash=relurl.starts_with('/')?std::string():std::string("/");
        std::string url= d->cVM->xml_base() +slash +std::string{relurl.data(),relurl.length()};
        return url;
    });
}

template<typename DeviceCapability>
bool SCloudPlayer<DeviceCapability>::load(std::stringstream& stream)
{
     try{
            d->cVM->load(stream);
            d->params_=d->cVM->getParameters();
            // d->print_params();
        }
        catch(std::exception& e){
            dev_cap::logger()<< stream.str();
            dev_cap::logger()<< e.what();
            return false;
            
        }
        return true;
}
template<typename DeviceCapability>
void SCloudPlayer<DeviceCapability>::start(const std::string& url,std::function<void(SCloudPlayer*,bool)> callback)
{
   
   
        
    boost::string_view href(url);
    if(href.starts_with("http")){
       
        urilite::uri remotepath =urilite::uri::parse(d->substituteParams(removed_escapes(url)));
        dev_cap::logger()<<"GET URL:" <<  R"(curl -i --insecure -X GET --header "Content-Type: text/plain" --header "Authorization: )" <<dev_cap::getAuthToken("svc_sbs")<<R"(" ")"<<d->substituteParams(removed_escapes(url))<<R"(")";
        auto handler = dev_cap::make_app_handler([=,callback=std::move(callback)](beast::error_code ec,std::string data){
                    if(ec){
                        std::string errormsg= Ui::shttp_fail(ec,data.c_str());
                        d->show_error(boost::string_view{errormsg.c_str(),errormsg.length()});
                        return;
                    }
                    
                    std::stringstream stream;
                    dev_cap::logger() << data;
                    stream<<data;
                    auto on_error=[=](auto str){
                        d->show_error(str);//);
                        if(callback){
                                callback(this,false);
                        }
                    };
                    try{
                        if(load(stream)){
                            if(callback){
                                callback(this,true);
                            }
                            load_default_screen();
                            return ;
                        }
                        on_error(boost::string_view{data.c_str(),data.length()});
                    }
                    catch(std::exception& e){
                        dev_cap::logger()<<e.what();
                        on_error(e.what());
                    }

                    

            });
            CurrentErrorStatus::getGlobalErrorStatus()={std::string(),std::string("Downloading")};
            d->load_native_screen("busy_screen");
            Ui::Get()(remotepath,Ui::Timeout(dev_cap::timeout()),Ui::Proxies{{"http", dev_cap::get_proxy()}, {"https", dev_cap::get_proxy()}},Ui::HttpHeader{{std::string{"Authorization"},dev_cap::getAuthToken("svc_sbs")}},handler);
            return;
    }
    
    d->cVM->setUrl(url);
    d->params_=d->cVM->getParameters();
    d->print_params();
    load_default_screen();

}
template<typename DeviceCapability>
void SCloudPlayer<DeviceCapability>::load_default_screen()
{
   
    d->screenManager->addScreen(d->cVM->getObject("screen",d->cVM->defaultScreen()));
     
}

template<typename DeviceCapability>
SCloudPlayer<DeviceCapability>::~SCloudPlayer()
{

}
