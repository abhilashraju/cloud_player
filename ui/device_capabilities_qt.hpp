#pragma once

#include "core/sdevice_capabilities.hpp"

#include <chrono>
#include <memory>
#include "core/boost_utilities.h"
#include "core/sbeast.hpp"
#include <iostream>
#include "qcasualvm.h"
#include <QEvent>
#include <QCoreApplication>
#include <QQuickItem>
#include <QQuickWindow>
#include "qcomponents.hpp"
#include <QMetaObject>
#include <QVariant>
struct DebugLogger
{
   const DebugLogger& operator<<(auto i) const{
       std::cout <<i;
       return *this;
   }
};
using namespace std::literals;

class QAsyncEvent:public QEvent
{
public:
    std::function<void()> fun;
    QAsyncEvent(std::function<void()> f):QEvent(QEvent::User),fun(std::move(f)){}
    void excecute(){
        fun();
    }
};

class QAsyncReciever:public QObject{
    bool event(QEvent* evt)override{
        auto asyncevt=static_cast<QAsyncEvent*>(evt);
        if(asyncevt){
            asyncevt->excecute();
            return true;
        }
        return false;
    }
    public:
    static QAsyncReciever* globalReceiver()
    {
        static QAsyncReciever* r= new QAsyncReciever();
        return r;
    }
};

template<typename T>
inline void postandExcuteHandler(T&& fn){
   
    qApp->postEvent(QAsyncReciever::globalReceiver(),new QAsyncEvent(std::forward<T>(fn)));
}
enum Countires{
        COUNTRY_ANGOLA,
        COUNTRY_ARGENTINA,
        COUNTRY_AUSTRALIA,
        COUNTRY_AUSTRIA,
        COUNTRY_BAHRAIN,
        COUNTRY_BANGLADESH,
        COUNTRY_BELARUS,
        COUNTRY_BELGIUM,
        COUNTRY_BRAZIL,
        COUNTRY_BRUNEI,
        COUNTRY_BULGARIA,
        COUNTRY_CANADA,
        COUNTRY_CHILE,
        COUNTRY_CHINA,
        COUNTRY_COLUMBIA,
        COUNTRY_COSTA_RICA,
        COUNTRY_CROATIA,
        COUNTRY_CZECH,
        COUNTRY_DENMARK,
        COUNTRY_ECUADOR,
        COUNTRY_EGYPT,
        COUNTRY_ESTONIA,
        COUNTRY_FINLAND,
        COUNTRY_FRANCE,
        COUNTRY_GERMANY,
        COUNTRY_GREECE,
        COUNTRY_GUATAMALA,
        COUNTRY_HONG_KONG,
        COUNTRY_HUNGARY,
        COUNTRY_INDIA,
        COUNTRY_INDONESIA,
        COUNTRY_IRAN,
        COUNTRY_IRELAND,
        COUNTRY_ISRAEL,
        COUNTRY_ITALY,
        COUNTRY_JAPAN,
        COUNTRY_JORDAN,
        COUNTRY_KAZAKHSTAN,
        COUNTRY_KOREA,
        COUNTRY_KUWAIT,
        COUNTRY_LATVIA,
        COUNTRY_LITHUANIA,
        COUNTRY_LEBANON,
        COUNTRY_MALAYSIA,
        COUNTRY_MEXICO,
        COUNTRY_MOROCCO,
        COUNTRY_MOZAMBIQUE,
        COUNTRY_NETHERLANDS,
        COUNTRY_NEW_ZEALAND,
        COUNTRY_NORWAY,
        COUNTRY_PAKISTAN,
        COUNTRY_PANAMA,
        COUNTRY_PARAGUAY,
        COUNTRY_PERU,
        COUNTRY_PHILIPPINES,
        COUNTRY_POLAND,
        COUNTRY_PORTUGAL,
        COUNTRY_QATAR,
        COUNTRY_ROMANIA,
        COUNTRY_RUSSIA,
        COUNTRY_SAUDI_ARABIA,
        COUNTRY_SERBIA_MONTENEGRO,
        COUNTRY_SINGAPORE,
        COUNTRY_SLOVAKIA,
        COUNTRY_SLOVENIA,
        COUNTRY_SOUTH_AFRICA,
        COUNTRY_SPAIN,
        COUNTRY_SRI_LANKA,
        COUNTRY_SWEDEN,
        COUNTRY_SWISS_FRENCH,
        COUNTRY_SWISS_ITALIAN,
        COUNTRY_SWITZERLAND,
        COUNTRY_TAIWAN,
        COUNTRY_THAILAND,
        COUNTRY_TUNISIA,
        COUNTRY_TURKEY,
        COUNTRY_UKRAINE,
        COUNTRY_UAE,
        COUNTRY_UK,
        COUNTRY_USA,
        COUNTRY_URUGUAY,
        COUNTRY_VENEZUELA,
        COUNTRY_VIETNAM,
        COUNTRY_SOUTHERN_AFRICA,  
        COUNTRY_NORTHWEST_AFRICA,
        COUNTRY_LATIN_AMERICA,
        COUNTRY_EUROPE,    
        COUNTRY_MIDDLE_EAST,
        COUNTRY_ASIA_PACIFIC,
        COUNTRY_ICELAND,
        COUNTRY_NONE
        };
struct device_capabilities_qt
 {
    static QQmlEngine* qvmEngine()
    {
        static QQmlEngine* view = new QQmlEngine();
        return view;
    }
    struct SCloudViewManager
    {
        static QQuickItem* createStackView(){
            static QQuickWindow view;
            
            auto stack =createItem(device_capabilities_qt::qvmEngine(),"stack");
            auto backbutton =QItemWrapper(createItem(device_capabilities_qt::qvmEngine(),"link"));

            backbutton->setParentItem(view.contentItem());
            QItemWrapper screen(view.contentItem());
            Hp::anchors(backbutton)["top"]= screen["top"];
            Hp::anchors(backbutton)["left"]= screen["left"];
            Hp::anchors(backbutton)["right"]= screen["right"];
            backbutton["height"]=50;
            backbutton["text"]="B";
            backbutton["horizontalAlignment"]=Qt::AlignLeft;
           
            Hp::connect(backbutton.data(), SIGNAL(clicked()), [=]() {
                qobject_cast<StackNotifier*>(stack)->pop();
            });
            stack->setParentItem(view.contentItem());
            view.show();
            view.resize(600,400);
            QAsyncReciever::globalReceiver();//create the event reciever in main thread.  
            return stack;
        }
        static QItemWrapper& makeStackView(){
             static QItemWrapper stackView{createStackView()};
             return stackView;
        }
        QItemWrapper& stackView{makeStackView()};
        SCloudViewManager(){
           
        }
        void addScreen(QQuickItem* obj){
             qobject_cast<StackNotifier*>(stackView.data())->push(obj);

        }
        QQuickItem* prepareNativeScreen(QQuickItem* obj,boost::string_view screen){
            return obj;
        }
        bool moveBackToScreen(const std::string& toState) {
           QVariant item;
            QMetaObject::invokeMethod(stackView.data(), "pos",
                Q_RETURN_ARG(QVariant, item));
            // item->deleteLater();
        }
        void popScreen(){
          qobject_cast<StackNotifier*>(stackView.data())->pop();
        }
        void removeAllScreens(){
           
        }
        bool bringToFront(const std::string& thescreen){
            
        }
    };

    using VM=QObjectVM;
    using ScreenManager=SCloudViewManager;
    static std::string getLanguage(){
        constexpr std::array <const char*,39> lang{
            "en",
            "en",
            "fr",
            "de",
            "es",
            "it",
            "sv",
            "da",
            "no",
            "nl",
            "fi",
            "ja",
            "pt",
            "pl",
            "tr",
            "zh-tw",
            "zh-cn",
            "ru",
            "cs",
            "hu",
            "ko",
            "th",
            "he",
            "eu",
            "en-eu",
            "fr-ca",
            "el",
            "ar",
            "id",
            "bg",
            "hr",
            "et",
            "lv",
            "lt",
            "ro",
            "sk",
            "sl",
            "uk",
            "ma",
        };
        return lang[0];
    }
    static std::string getCountry(){
    constexpr std::array<std::pair<const char*,int>,91> country={
        std::pair<const char*,int>{"AO",COUNTRY_ANGOLA},
        {"AR",COUNTRY_ARGENTINA},
        {"AU",COUNTRY_AUSTRALIA},
        {"AT",COUNTRY_AUSTRIA},
        {"BH",COUNTRY_BAHRAIN},
        {"BD",COUNTRY_BANGLADESH},
        {"BY",COUNTRY_BELARUS},
        {"BE",COUNTRY_BELGIUM},
        {"BR",COUNTRY_BRAZIL},
        {"BN",COUNTRY_BRUNEI},
        {"BG",COUNTRY_BULGARIA},
        {"CA",COUNTRY_CANADA},
        {"CL",COUNTRY_CHILE},
        {"CN",COUNTRY_CHINA},
        {"CO",COUNTRY_COLUMBIA},
        {"CR",COUNTRY_COSTA_RICA},
        {"HR",COUNTRY_CROATIA},
        {"CZ",COUNTRY_CZECH},
        {"DK",COUNTRY_DENMARK},
        {"EC",COUNTRY_ECUADOR},
        {"EG",COUNTRY_EGYPT},
        {"EE",COUNTRY_ESTONIA},
        {"FI",COUNTRY_FINLAND},
        {"FR",COUNTRY_FRANCE},
        {"DE",COUNTRY_GERMANY},
        {"GR",COUNTRY_GREECE},
        {"GT",COUNTRY_GUATAMALA},
        {"HK",COUNTRY_HONG_KONG},
        {"HU",COUNTRY_HUNGARY},
        {"IN",COUNTRY_INDIA},
        {"ID",COUNTRY_INDONESIA},
        {"IR",COUNTRY_IRAN},
        {"IE",COUNTRY_IRELAND},
        {"IL",COUNTRY_ISRAEL},
        {"IT",COUNTRY_ITALY},
        {"JP",COUNTRY_JAPAN},
        {"JO",COUNTRY_JORDAN},
        {"KZ",COUNTRY_KAZAKHSTAN},
        {"KR",COUNTRY_KOREA},
        {"KW",COUNTRY_KUWAIT},
        {"LV",COUNTRY_LATVIA},
        {"LT",COUNTRY_LITHUANIA},
        {"LB",COUNTRY_LEBANON},
        {"MY",COUNTRY_MALAYSIA},
        {"MX",COUNTRY_MEXICO},
        {"MA",COUNTRY_MOROCCO},
        {"MZ",COUNTRY_MOZAMBIQUE},
        {"NL",COUNTRY_NETHERLANDS},
        {"NZ",COUNTRY_NEW_ZEALAND},
        {"NO",COUNTRY_NORWAY},
        {"PK",COUNTRY_PAKISTAN},
        {"PA",COUNTRY_PANAMA},
        {"PY",COUNTRY_PARAGUAY},
        {"PE",COUNTRY_PERU},
        {"PH",COUNTRY_PHILIPPINES},
        {"PL",COUNTRY_POLAND},
        {"PT",COUNTRY_PORTUGAL},
        {"QA",COUNTRY_QATAR},
        {"RO",COUNTRY_ROMANIA},
        {"RU",COUNTRY_RUSSIA},
        {"SA",COUNTRY_SAUDI_ARABIA},
        {"RS",COUNTRY_SERBIA_MONTENEGRO},
        {"SG",COUNTRY_SINGAPORE},
        {"SK",COUNTRY_SLOVAKIA},
        {"SI",COUNTRY_SLOVENIA},
        {"ZA",COUNTRY_SOUTH_AFRICA},
        {"ES",COUNTRY_SPAIN},
        {"LK",COUNTRY_SRI_LANKA},
        {"SE",COUNTRY_SWEDEN},
        {"CH",COUNTRY_SWISS_FRENCH}, /*ISO-Name missing*/
        {"CH",COUNTRY_SWISS_ITALIAN},/*ISO-Name missing*/
        {"CH",COUNTRY_SWITZERLAND},
        {"TW",COUNTRY_TAIWAN},
        {"TH",COUNTRY_THAILAND},
        {"TN",COUNTRY_TUNISIA},
        {"TR",COUNTRY_TURKEY},
        {"UA",COUNTRY_UKRAINE},
        {"AE",COUNTRY_UAE},
        {"GB",COUNTRY_UK},
        {"US",COUNTRY_USA},
        {"UY",COUNTRY_URUGUAY},
        {"VE",COUNTRY_VENEZUELA},
        {"VN",COUNTRY_VIETNAM},
        {"rAF", COUNTRY_SOUTHERN_AFRICA},  /* 84 - Not clear in spec */
        {"rAF", COUNTRY_NORTHWEST_AFRICA}, /* 85 - Not clear in spec */
        {"rLA", COUNTRY_LATIN_AMERICA}, /* 88 */
        {"rEU", COUNTRY_EUROPE},        /* 89 */
        {"rME", COUNTRY_MIDDLE_EAST},   /* 90 */
        {"rAP", COUNTRY_ASIA_PACIFIC},  /* 91 */
        {"IS", COUNTRY_ICELAND},
        {"rNO",COUNTRY_NONE}, /*No Country*/
    };
        auto iter = std::find_if(begin(country),end(country),[curcountry=0](auto&v){ return curcountry==v.second;});// country["ui_f_local_attr_country"_ia()];
        if(iter != end(country)){
            return iter->first;
        }
        return "rNO";
    }
    static std::string getModel(){
      
        return "HP%20OfficeJet%20Pro%208020%20series";
    }
    static std::tuple<std::string,std::string,std::string> get_auth_data()
    {
        
        return std::make_tuple(std::string(),std::string(),std::string());
    }
    static std::string getAuthToken(std::string name)
    {
        
        return std::string("Basic QVFBQUFBRjQ5R0NabmdBQUFBRU0xRjU4OnIyZ0N2SUZpRnlGODc0NTFMMWZNUWJ0KzBhTHpCZk8zbkxkK1Y5WGpNTms9");
        
    }
    static std::string getCloudId(){
     
        std::string cloudid;
        return cloudid;
    }
    static std::string getCloudVersion(){
        return "3.0";
    }
    static std::string get_proxy()
    {

       std::string proxyString;
       return proxyString;

    }
    static std::string expType(){return "casual";}
    static bool getSmartTasksStatus(){
        return true;
    }
    static std::string getRootUrl(){
        return "http://sipserver.psr.rd.hpicorp.net/main/xmltestCases_v3.1/conformance_v3.1.xml";
    }
    static std::chrono::milliseconds timeout(){return 10s;}
    static std::unique_ptr<QObjectVM> createVM(){ return std::make_unique<QCasualVM>();}
    
   
    
    static std::unique_ptr<ScreenManager> createScreenManager(){
        return std::make_unique<ScreenManager>();
    }
    template<typename Handler>
    static auto make_app_handler(Handler&& handler)
    {
        return [handler=std::move(handler)](beast::error_code ec, std::string data){
           postandExcuteHandler([handler=std::move(handler),ec=std::move(ec),data=std::move(data)](){
                handler(std::move(ec),std::move(data));
           });
        };
    }
 };
 using qt_device_capabilities = device_capabilities_impl<device_capabilities_qt,DebugLogger>;
