#pragma once
#include <array>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <sstream>
#include <QQmlProperty>
#include <deque>
template <typename T>
struct QObjectWrapperGeneric
{
    T* obj{nullptr};
    struct Binder{
        T* obj;
        std::string prop;
        void operator=(const QVariant& value){
            obj->setProperty(prop.data(),value);
        }
        QVariant operator()(){return obj->property(prop.data());}
        operator QVariant()const {return obj->property(prop.data());}
    };
    Binder operator[](const char* name){
        return Binder{obj,name};
    }
    operator bool(){
        return obj != nullptr;
    }
    T* operator->(){return data();}
    QObjectWrapperGeneric(T* o):obj(o){if(obj==nullptr)throw std::runtime_error("null object");}
    T* data(){return obj;}
};
using QObjectWrapper= QObjectWrapperGeneric<QObject>;
using QItemWrapper= QObjectWrapperGeneric<QQuickItem>;
namespace Hp{

    struct slothandler:QObject{
        Q_OBJECT
    public:
        using Handler = std::function<void()>;
        std::function<void()> slotfunc;
        slothandler(Handler f):slotfunc(std::move(f)){}
        public slots:
        void invoke(){ slotfunc();}
    };
    inline auto connect(QObject* send,const char* sig,auto slt){

        auto handler = new slothandler(std::move(slt));
        handler->setParent(send);
        return QObject::connect(send,sig,handler,SLOT(invoke()));
    }
    inline QObjectWrapper anchors(QQuickItem* item){
        return QObjectWrapper(qvariant_cast<QObject*>(item->property("anchors")));
    }
    inline QObjectWrapper anchors(QItemWrapper item){
        return QObjectWrapper(qvariant_cast<QObject*>(item->property("anchors")));
    }
    inline QObjectWrapper anchorLines(QQuickItem* item){
        return QObjectWrapper(qvariant_cast<QObject*>(item->property("anchorslines")));
    }
    inline QObjectWrapper anchorLines(QItemWrapper item){
        return QObjectWrapper(qvariant_cast<QObject*>(item->property("anchorslines")));
    }
    static QString displayTree(QQuickItem *item){
        struct  Visitor
        {
            std::vector<QString> widgetLog;
            QString spacestr=" ";
            void visit( QQuickItem* obj){
                auto geometry=[](auto w){
                    return QString("{%1 x %2 %3 x %4 }").arg(w->x()).arg(w->y()).arg(w->width()).arg(w->height());
                };
                QQuickItem* wid = obj;
                if(!wid) return ;
                spacestr.append("  ");
                std::ostringstream steam (std::ostringstream::ate);
                steam << geometry(wid).toStdString();
                steam << " ";

                QString vis = " visible ";
                if(!wid->isVisible())
                    vis=" invisible ";
                widgetLog.push_back(spacestr + " x "+wid->objectName()+" "
                                    +steam.str().data()+vis +QString("opacity %1").arg(wid->property("opacity").toReal()));
                for(auto ch: wid->childItems()){
                    visit(ch);
                }
                spacestr.remove(spacestr.length()-2);
            }
        };
        Visitor vis;
        vis.visit(item);
        for(auto lg:vis.widgetLog)
        {
            qDebug()<<lg;
        }
    }
    static QString properties(QQuickItem *item, bool linebreak)
    {
        const QMetaObject *meta = item->metaObject();

        QHash<QString, QVariant> list;
        for (int i = 0; i < meta->propertyCount(); i++)
        {
            QMetaProperty property = meta->property(i);
            const char* name = property.name();
            QVariant value = item->property(name);
            list[name] = value;
        }

        QString out;
        QHashIterator<QString, QVariant> i(list);
        while (i.hasNext()) {
            i.next();
            if (!out.isEmpty())
            {
                out += ", ";
                if (linebreak) out += "\n";
            }
            out.append(i.key());
            out.append(": ");
            out.append(i.value().toString());
        }
        for(auto c:item->childItems()){
            out +=properties(c,linebreak);
        }
        return out;
    }
}
inline QQuickItem* createItem(QQmlEngine* eng,const std::string& name,QQmlContext* context=nullptr)
{
    constexpr std::array<std::pair<const char*,const char*>,7> components={std::pair<const char*,const char*>
        {"link",R"(import QtQuick 2.5

         Rectangle {
             id: button
             signal clicked
             property alias text: text.text
             border.width: 1
             border.color: "white"
             color:"black"
             property real textHeight: height - 2
             property real fontHeight: 0.4
             property bool pressed: mouse.pressed
             property real implicitMargin: (width - text.implicitWidth) / 2
             function click(){
                    clicked()
             }
             Text {
                 id: text
                 anchors.left: parent.left
                 anchors.right: parent.right
                 anchors.top: parent.top
                 height: parent.textHeight
                 horizontalAlignment: Text.AlignHCenter
                 verticalAlignment: Text.AlignVCenter
                 font.pixelSize: height * fontHeight
                 color: "white"
                 font.family: "Open Sans Regular"
                 text:"hello"
             }

             MouseArea {
                 id: mouse
                 anchors.fill: parent
                 onClicked: button.clicked()
             }
         })"},

        {
            "list",R"(
                   import QtQuick 2.0
                    import SipsControls 1.0
                    ListView {
                        id:list
                        model:CloudItemModel{
                                            listData:myModel
                                            }
                        Component {
                            id: contactsDelegate
                            Rectangle {
                                id: wrapper
                                width: parent.width
                                height: 50
                                color:"black"
                                Image {
                                    id: image
                                    width: 64
                                    anchors.left: parent.left
                                    anchors.leftMargin: 3
                                    anchors.bottom: parent.bottom
                                    anchors.bottomMargin: 1
                                    anchors.top: parent.top
                                    anchors.topMargin: 1
                                    fillMode: Image.PreserveAspectFit
                                    source: model.image
                                }
                                Text {
                                    id:primary
                                    anchors.rightMargin: 100
                                    anchors.leftMargin:2

                                    verticalAlignment: Text.AlignCenter
                                    text:model.text
                                    anchors.left: image.right
                                    anchors.right: parent.right
                                    anchors.bottom: parent.bottom
                                    anchors.top: parent.top
                                    font.family: "Open Sans Regular"
                                    font.pointSize: 22
                                    fontSizeMode: Text.Fit
                                    color:"white"

                                }
                                Text {
                                anchors.left:primary.right
                                anchors.right: parent.right
                                anchors.top: parent.top
                                anchors.bottom: parent.bottom
                                anchors.leftMargin:10
                                anchors.rightMargin:10
                                verticalAlignment: Text.AlignCenter
                                id: secondary
                                text:model.secondarytext
                                font.family: "Open Sans Regular"
                                font.pointSize: 22
                                fontSizeMode: Text.Fit
                                color:"white"

                                }
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: {model.handler=list.currentIndex}
                                }
                            }
                        }


                        delegate: contactsDelegate
                        focus: true
                    }



                )"
        },
        {
            "static",R"(
                       import QtQuick 2.0
                       Text {
                           id:primary
                           anchors.fill:parent
                           anchors.rightMargin: 70
                           anchors.leftMargin:10

                           verticalAlignment: Text.AlignCenter
                           text:"Header"
                           font.family: "Open Sans Regular"
                           font.pointSize: 22
                           fontSizeMode: Text.Fit
                           color:"white"

                       }
             )"
        },
        {
            "text",R"(
                       import QtQuick 2.0
                       Text {
                           id:primary
                           anchors.fill:parent
                           anchors.rightMargin: 70
                           anchors.leftMargin:10

                           verticalAlignment: Text.AlignCenter
                           text:"Body"
                           font.family: "Open Sans Regular"
                           font.pointSize: 22
                           fontSizeMode: Text.Fit
                           color:"white"

                       }
             )"
        },
        {
            "preFetchImage",R"(import QtQuick 2.0
                        Image {

                        })"
        },
        {
            "screen",R"(
                import QtQuick 2.0
                Rectangle {
                    id:screen
                    anchors.fill:parent
                    anchors.topMargin: 50
                    color:"black"
                    PropertyAnimation {
                        loops: 1
                        running: true
                        id: animation1;
                        target: screen;
                        property: "opacity";
                        from: 0;
                        to: 1;
                        duration: 500
                    }

                }
            )"
        },
        {
            "stack",R"(import QtQuick 2.0
           import QtQuick.Controls 2.5
           import QtQuick.Controls 1.4
           import SipsControls 1.0

           StackNotifier{
               anchors.fill:parent
               id:notifier
               objectName: "notifier"
           }
           )"
        }
    };
    if(auto iter =std::find_if(begin(components),end(components),[&](auto& v){return name == v.first;});iter != end(components)){
        QQmlComponent component(eng);
        component.setData( QByteArray::fromStdString(iter->second),QUrl());
        QObject *myObject = component.create(context);
        QQuickItem *item = qobject_cast<QQuickItem*>(myObject);
        return item;
    }
    return nullptr;
}
class StackNotifier : public QQuickItem
{
    Q_OBJECT
public:
    StackNotifier(QQuickItem *parent = 0):QQuickItem(parent){}
    void push(QQuickItem* item)
    {
        if(!items.empty()){
            items.back()->setParentItem(nullptr);
            if(!items.back()->property("cached").toBool()){
                auto item =items.back();
                items.pop_back();
                item->deleteLater();
            }
           
        }
        items.push_back(item);
        item->setParentItem(this);
        emit pushed(QVariant::fromValue(item));
    }
    QQuickItem* pop(){
        if(!items.empty()){
            auto item =items.back();
            items.pop_back();
            item->setParentItem(nullptr);
            emit poped();
            item->deleteLater();
            if(!items.empty()){
                 items.back()->setParentItem(this);
            }
        }

    }
signals:
    void pushed(QVariant id);
    void poped();
private:
    std::deque<QQuickItem*> items;
};
class CloudAttachedType : public QObject
  {
      Q_OBJECT
      Q_PROPERTY(bool cached READ cached WRITE setCached)
  public:
      CloudAttachedType(QObject *parent);
      bool cached() const{return cash;}
      void setCached(bool c){ cash=c;}
  signals:
      void published();
      private:
      bool cash{false};

    
  };
class CloudAttachedProperties : public QObject
  {
      Q_OBJECT
  public:
      static CloudAttachedType *qmlAttachedProperties(QObject *object)
      {
          return new CloudAttachedType(object);
      }
  };
  QML_DECLARE_TYPEINFO(CloudAttachedProperties, QML_HAS_ATTACHED_PROPERTIES)
