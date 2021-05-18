#include "qcasualvm.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <QQmlComponent>
#include <QQuickItem>
#include <QUrl>
#include <QString>
#include <QStringList>
#include "qcomponents.hpp"
#include "layouts.h"
#include "device_capabilities_qt.hpp"
#include "sipslistmodel.h"
#include "datasource.h"
#include "genericmodel.h"
template <typename T = std::string>
T attribute(const ptree &node, const std::string &id,T def=T{})
{
    return node.get("<xmlattr>." + id, def);
};
void update_xml_role(auto& widgetMap, QItemWrapper obj){
    auto role = obj["xml_role"]();
    if(role.isValid()){
         widgetMap[role.toString().toStdString()] = obj.data();
    }
    for(auto child:obj->childItems()){
        update_xml_role(widgetMap,QItemWrapper(child));
    }
}
QQmlEngine* QCasualVM::qvmEngine()
{
   return device_capabilities_qt::qvmEngine();
}
QCasualVM::QCasualVM()
{
      auto find_childitem=[](auto p, auto name)->QQuickItem*{
            auto items = p->childItems();
            auto iter =std::find_if(begin(items),end(items),[&](auto c){ return c->objectName()== name;});
            if(iter != end(items)){
                return *iter;
            }
            return nullptr;

      };
      auto tobeimplemented=[=](){
        QQuickItem *item = createItem(qvmEngine(),"text");
        auto obj=QItemWrapper(item);
        obj["text"] = "To be implemented...";
        obj["xml_role"] = "$T";
        obj["objectName"] = "text";
        return obj.data();
      };
      setInterPretter("screen", [=](const ptree &tree, QQuickItem *p) {
       
        auto view=QItemWrapper(createItem(qvmEngine(),"screen"));
        CloudAttachedType *attached =
          qobject_cast<CloudAttachedType*>(qmlAttachedPropertiesObject<CloudAttachedProperties>(view.data()));

        std::string id = tree.get("<xmlattr>.id", std::string());
        view["objectName"] = QString(id.data());
        view["cached"]=attribute(tree,"allowCache",true);
        view["name"] = QString(id.data());
       
        std::map<std::string, QQuickItem *> widgetMap;
        for (auto &v : tree.get_child(""))
        {
            if (v.first == "<xmlattr>" || v.first == "<xmlcomment>")
            {
                //    SDEBUG() << v.second.get();
                continue;
            }
            QItemWrapper obj = getObject(v, view.data());
            if(!obj){
                throw std::runtime_error("Elemenet "+v.first+" not supported");
            }
             
            update_xml_role(widgetMap,obj);
            obj->setParentItem(view.data());
            

        }
        std::vector<std::string> sortednames;
        std::transform(begin(widgetMap), end(widgetMap), std::back_inserter(sortednames), [](auto &v) {
            return v.first;
        });
        std::sort(begin(sortednames), end(sortednames));
        std::string layoutKey = std::accumulate(begin(sortednames), end(sortednames), std::string(), [&](auto sofar, auto current) {
            return sofar + current;
        });
        qDebug()<< "layout: " << layoutKey.data();
        if(!Hp::getLayoutMap()[layoutKey]){
            throw std::runtime_error("Layout "+layoutKey+" Not found");
        }
        Hp::getLayoutMap()[layoutKey](view, widgetMap);
        Hp::connect(view.data(), SIGNAL(clicked()), [=]() mutable{
               Hp::displayTree(view.data());
                
            });
        return view.data();
    });

    setInterPretter("static", [=](const ptree &tree, QQuickItem *p) -> QQuickItem * {
        
        auto location = tree.get("<xmlattr>.location", std::string());
        auto text = tree.get("<xmlattr>.labelText", std::string());
        if (location == "heading" || location == "subHeading")
        {
            auto header =QItemWrapper(createItem(qvmEngine(),"static"));
            header["text"] = QString::fromStdString(d->substituteParams(text));
            header["xml_role"] = "$H";
            header["objectName"] = location.c_str();
            return header.data();
        }
        if (location == "info")
        {
            auto body = QItemWrapper(createItem(qvmEngine(),"static"));
            body["text"] = QString::fromStdString(d->substituteParams(text));
            body["xml_role"] = "$B";
            body["objectName"] = "text";
            body["alignment"] = QVariant(Qt::AlignLeft | Qt::AlignVCenter);
            return body.data();
        }

        return nullptr;
    });

    setInterPretter("link", [=](const ptree &tree, QQuickItem *p) -> QQuickItem * {
      
        auto location = attribute(tree, "logical");
        location = location.empty()?std::string("ok"):location;
        auto text = d->substituteParams(attribute(tree, "labelText"));
        auto href = attribute(tree, "href");
        auto on_error_href = attribute(tree, "on_error_href");
        auto saveSelection=attribute(tree,"saveSelection",true);

        if (href.empty())
        {
            href = "control:prev(1)";
        }
        auto httpMethod = attribute(tree, "httpMethod");
        std::array<const char *, 5> okbuttons = {"ok", "altOk", "context", "cancel"};
       
        if (std::find_if(begin(okbuttons), end(okbuttons), [&](auto v){ return v? v == location:false;})!=end(okbuttons))
        {
            auto obj=QItemWrapper(createItem(qvmEngine(),"link"));
            obj["text"] = QString::fromStdString(d->substituteParams(text));
            obj["xml_role"] = QString::fromStdString(std::string("$") + location);
            obj["objectName"] = location.c_str();
            Hp::connect(obj.data(), SIGNAL(clicked()), [=]() {
                if(saveSelection){
                    d->href_handler_check(href,on_error_href,httpMethod.empty() ? "GET" : httpMethod,p);
                }else{
                    d->href_handler(href, httpMethod.empty() ? "GET" : httpMethod);
                }
                
            });
            return obj.data();
        }
        
        return nullptr;
      
    });

    setInterPretter("list", [=](const ptree &tree, QQuickItem *p) -> QQuickItem * {
        auto view = attribute(tree, "view");
        auto kind = attribute(tree, "kind");
        auto param = attribute(tree, "param");
        kind = kind.empty() ? "single" : kind;
        using ModelData=SValuePairs<QString,QString,QString,std::function<void(const QVariant&)>>;
        std::vector<ModelData> modelData;
        for (auto &v : tree.get_child(""))
        {
            if (v.first == "item")
            {
                auto text =v.second.get("<xmlattr>.labelText", std::string());
                auto iter =std::find(begin(text),end(text),':');
                auto sectext=std::string{};
                if(iter != end(text)){
                    sectext = text.substr(distance(begin(text),iter)+1,distance(iter,end(text)));
                    text = text.substr(0,distance(begin(text),iter));
                }
                
                auto handler = [=](const QVariant&) {
                            auto httpMethod = v.second.get("<xmlattr>.httpMethod", std::string());
                            auto href = v.second.get("<xmlattr>.href", std::string());
                            auto value = v.second.get("<xmlattr>.value", std::string());
                            auto src = v.second.get("<xmlattr>.src", std::string());
                            if (!value.empty())
                            {
                                set_param_notify(param, value);
                            }
                            if (!href.empty())
                            {
                                d->href_handler(href, httpMethod.empty() ? "GET" : httpMethod);
                            }else if(kind == "single"){
                                auto okbutton =find_childitem(p,"ok");
                                assert(okbutton);
                                QMetaObject::invokeMethod(okbutton,"click");
                            }
                        };
                auto src = v.second.get("<xmlattr>.src", std::string());
                src=d->relativetoAbsolute(src);
                modelData.emplace_back(ModelData{{"text",QString::fromStdString(text)},{"secondarytext",""},{"image",QString::fromStdString(src)},{"handler",handler}});
            }
        }
        QQmlContext *context = new QQmlContext(qvmEngine()->rootContext());
        auto modelItems=make_modelItems<ModelItems>(modelData);
        context->setContextProperty("myModel", modelItems);
        auto obj=QItemWrapper(createItem(qvmEngine(),"list",context));
        context->setParent(obj.data());
        modelItems->setParent(obj.data());
        obj["xml_role"] = "$L";
        return obj.data();
    });

    setInterPretter("autoLink", [=](const ptree &tree, QQuickItem *p) -> QQuickItem * {
      return nullptr;
    });
    setInterPretter("preFetchImage", [=](const ptree &tree, QQuickItem *p) -> QQuickItem * {
        auto obj=QItemWrapper(createItem(qvmEngine(),"preFetchImage"));
        obj["source"] = "qrc:/cat.png";
        obj["xml_role"] = "$I";
        obj["objectName"] = "image";
        return obj.data();
    });
    setInterPretter("text", [=](const ptree &tree, QQuickItem *p) -> QQuickItem * {
        QQuickItem *item = createItem(qvmEngine(),"text");
        auto obj=QItemWrapper(item);
        obj["text"] = QString::fromStdString(attribute(tree, "labelText"));
        obj["xml_role"] = "$T";
        obj["objectName"] = "text";
        return obj.data();
    });
    setInterPretter("inputBox", [=](const ptree &tree, QQuickItem *p) -> QQuickItem * {
        return tobeimplemented();
    });
    setInterPretter("time", [=](const ptree &tree, QQuickItem *p) -> QQuickItem * {
       return tobeimplemented();
    });
    setInterPretter("date", [=](const ptree &tree, QQuickItem *p) -> QQuickItem * {
      return tobeimplemented();
    });
    auto notsupported =[=](const ptree &tree, QQuickItem *p)->QQuickItem* {
      return tobeimplemented();
    };
    setInterPretter("datetime", [=](const ptree &tree, QQuickItem *p) -> QQuickItem * {
       return notsupported(tree,p);
    });
    setInterPretter("genspinner", [=](const ptree &tree, QQuickItem *p) -> QQuickItem * {
       return notsupported(tree,p);
    });
    setInterPretter("chkBox", [=](const ptree &tree, QQuickItem *p) -> QQuickItem * {
       return notsupported(tree,p);
    });
    setInterPretter("dropdown", [=](const ptree &tree, QQuickItem *p) -> QQuickItem * {
       return notsupported(tree,p);
    });
    setInterPretter("toggle", [=](const ptree &tree, QQuickItem *p) -> QQuickItem * {
       return notsupported(tree,p);
    });
    
      
    setInterPretter("form", [=](const ptree &tree, QQuickItem *p) -> QQuickItem * {
       return tobeimplemented();
    });

    d->validate_handler=[](QQuickItem* state){
        // auto validator = state->property("validator");
        // if(validator.isValid()){
        //     SAbstractItemView* view = qobject_cast<SAbstractItemView*>(validator.toObject());
        //     if(view){
        //         SItemSelection& selection=view->itemSelection();
        //         return selection.indices().count()>0;
        //     }
        // }
        return true;
    };
}
QCasualVM::QCasualVM(const std::string &url)
    : QCasualVM()
{
    setUrl(url);
}
