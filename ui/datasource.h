#ifndef DATASOURCE_H
#define DATASOURCE_H
#include <memory>
#include <QObject>
#include <QHash>
#include <QVariant>
#include <QModelIndex>
#include <type_traits>
#include "spropertyaccessor.h"
template <typename Handler>
std::function<void(const QVariant&)> make_handler(Handler&& h){
    return  std::function<void(const QVariant&)>(std::move(h));
}
struct DataSource
{
    using handler=std::function<void(const QVariant&)>;

    QHash<QByteArray ,QVariant> mdata;
    QHash<int,QByteArray> mRolesNames;
    int start_role{Qt::UserRole+1};
    QVariant data( int role)const{
        if(mRolesNames.contains(role)){
            return mdata[mRolesNames[role]];
        }
        return QVariant();
    }
    virtual bool setData(const QVariant &value,
                 int role = Qt::EditRole)
    {
        if(mRolesNames.contains(role)){
            if(mRolesNames[role] =="handler"){
                handleOnClick(qvariant_cast<QModelIndex>(value));
                return true;
            }
            mdata[mRolesNames[role]]=value;
            return true;
        }
        return  false;
    }
    virtual QHash<int, QByteArray> roleNames() const{
        return mRolesNames;
    }
    void insert(const QByteArray& name, const QVariant& value){
        if(!mdata.contains(name)){
            mRolesNames[start_role++]=name;
        }
        mdata[name]=value;
    }
    void handleOnClick(const QModelIndex& index){
        if(mdata.contains("handler")){
             qvariant_cast<handler>(mdata["handler"])(index);
        }
    }
    class QPropObserver:public QObject
    {

        DataSource* item;
    public:
        QPropObserver(DataSource* item ,QObject* p=0):QObject(p),item(item){}
        bool handleSetProperty(const QByteArray &name, const QVariant &value)
        {

             item->insert(name,value);
             return true;
        }
    };

    QString changingProperty()const;
    template <typename T>
    void setValue(const QByteArray& name , const T& val)
    {
        handleSetValue(name,val);
    }
    template<typename T>
    void handleSetValue(const QByteArray& name , const T& val, typename std::enable_if<std::is_convertible<T,QVariant>::value,int>::type t=0)
    {

        insert(name,val);
    }

    template<typename T>
    void handleSetValue(const QByteArray& name ,  T val, typename std::enable_if<
                        s_is_callable_v<T,const QVariant&>
                        ,int>::type=0)
    {
       insert(name,QVariant::fromValue(handler{std::move(val)}));
    }
//    template<typename T>
//    void handleSetValue(const QString& name , const T& val, typename std::enable_if<
//                        sourcebinderproperties<T>::value
//                        ,int>::type=0)
//    {
//        if(!observer){
//            observer = Ui::make_unique<QPropObserver>(this);
//        }
//        (*observer)[name]=val;
//    }
//    void handleSetValue(const QString& name , const SAny& val)
//    {
//        if(!observer){
//            observer = Ui::make_unique<QPropObserver>(this);
//        }
//        (*observer.get())[name]=val;
//    }
};
Q_DECLARE_METATYPE(DataSource::handler)

class ModelItems : public QObject
{
    Q_OBJECT
public:
    using data_source_type =DataSource;
    using value_type =DataSource*;
    struct ref_iterator{
        size_t index{0};
        using value_type = typename std::vector<std::unique_ptr<DataSource>>::value_type;
        using reference =typename std::vector<std::unique_ptr<DataSource>>::reference;
        std::vector<std::unique_ptr<DataSource>>& data;
        reference operator*(){return data.at(index);}
        friend bool operator <(const ref_iterator& f,const ref_iterator& s){
            return f.index < s.index;
        }
        friend bool operator ==(const ref_iterator& f,const ref_iterator& s){
            return f.index == s.index;
        }
        friend bool operator !=(const ref_iterator& f,const ref_iterator& s){
            return !(f.index == s.index);
        }
        ref_iterator& operator++(){
            index++;
            return *this;
        }

    };

    explicit ModelItems(QObject *parent = nullptr);
    explicit ModelItems(size_t no,QObject *parent = nullptr):mdata{no}{};
    void append(DataSource* data);
    void remove(int index);
    int size(){return mdata.size();}


    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)const;
    virtual bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QHash<int, QByteArray> roleNames() const ;
    ref_iterator begin(){return ref_iterator{0,mdata};}
    ref_iterator end(){return ref_iterator{mdata.size(),mdata};}
    void push_back(DataSource* data){ append(data);}
    void reserve(size_t no){mdata.reserve(no);}
 signals:
    void beginAppendItem();
    void endAppendItem();
    void beginRemoveItem(int index);
    void endRemoveItem();
public:
    Q_INVOKABLE void handleOnclick(QModelIndex index);
private:
    std::vector<std::unique_ptr<DataSource>> mdata;

};

#endif // DATASOURCE_H
