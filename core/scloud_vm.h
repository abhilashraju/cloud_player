#pragma once
#include "scloud_xml_parser.h"
#include <functional>
#include <map>
#include "native_view_templates.h"
template<typename TreeType>
class SCloudVMPrivate
{
   public:
    using INTERPRETTER_TYPE = std::function<TreeType*(const ptree&,TreeType* parent)>;
    using ERROR_HANDLER=std::function<void(boost::string_view)>;
    using HREF_TYPE = std::function<void(boost::string_view,boost::string_view)>;
    using HREF_CHECK_TYPE = std::function<void(boost::string_view,boost::string_view,boost::string_view,TreeType* p)>;
    using PARAM_TYPE = std::function<void(boost::string_view,boost::string_view)>;
    using PARAM_GET_TYPE = std::function<std::string(boost::string_view)>;
    using PARAM_REPLACER = std::function<std::string(std::string)>;
    using URL_MAKER = std::function<std::string(std::string)>;
    using VALIDATE_HANDLER = std::function<bool(TreeType*)>;
    std::map<std::string, INTERPRETTER_TYPE> map;
    ERROR_HANDLER error_handler;
    HREF_TYPE href_handler;
    HREF_CHECK_TYPE href_handler_check;
    PARAM_TYPE param_handler;
    PARAM_GET_TYPE param_get;
    PARAM_REPLACER substituteParams;
    URL_MAKER relativetoAbsolute;
    VALIDATE_HANDLER validate_handler;
    SCloudParser parser;
    SCloudVMPrivate(const std::string& url):parser(url){
        
    }
    SCloudVMPrivate(){
       
    }
    void setUrl(const std::string& url ){
        parser.load(url);
    }
    void load(std::stringstream& stream ){
        parser.load(stream);
    }
    
};
template<typename TreeType>
class SCloudVM
{
    public:
    using value_type=TreeType;
    using pointer_type=TreeType*;
    using ref_type=TreeType&;
    SCloudVM()
    :d(std::make_unique<SCloudVMPrivate<TreeType> >())
    {
        
    }
    SCloudVM(const std::string& url)
    :d(std::make_unique<SCloudVMPrivate<TreeType> >(url))
    {
       
    }
    void setUrl(const std::string& url){
        d->setUrl(url);
    }
    void load(std::stringstream& stream)
    {
         d->load(stream);
    }
    void setInterPretter(const std::string& elem, typename SCloudVMPrivate<TreeType>::INTERPRETTER_TYPE interpretter)
    {
        d->map.insert({elem,interpretter});
    }
    TreeType* getObject( ptree::value_type root,TreeType* parent=nullptr){
        auto iter =std::find_if(std::begin(d->map),std::end(d->map),[&](auto& v){
            return v.first == root.first;
        });
        if( iter !=std::end(d->map)){
             return iter->second(root.second,parent);
         }
        return nullptr;
    }
    TreeType* getObject(boost::string_view elem,boost::string_view id){
        if(elem.starts_with("native_")){
            return getObject(get_native_parser().element(elem.substr(strlen("native_")),id));
        }
        return getObject(d->parser.element(elem,id));
        
    }
    std::string defaultScreen() const
    {
        return d->parser.defaultScreen();
    }
    std::string name() const
    {
        return d->parser.name();
    }
    std::string xml_base() const
    {
        return d->parser.xml_base();
    }
    void add_href_handler(typename SCloudVMPrivate<TreeType>::HREF_TYPE handler){
        d->href_handler=std::move(handler);
        d->href_handler_check=[=](boost::string_view href,boost::string_view on_error_href,boost::string_view method,TreeType* parent){
            if(d->validate_handler(parent)){
                 d->href_handler(href,method);
                 return;
            }
            if(!on_error_href.empty()){
                 d->href_handler(on_error_href,"GET");
                 return;
            }
               
        };
    }
    void add_error_handler(typename SCloudVMPrivate<TreeType>::ERROR_HANDLER handler){
          d->error_handler=std::move(handler); 
    }
    void add_param_handler(typename SCloudVMPrivate<TreeType>::PARAM_TYPE handler){
        d->param_handler=std::move(handler);
    }
    void add_param_get_handler(typename SCloudVMPrivate<TreeType>::PARAM_GET_TYPE handler){
        d->param_get=std::move(handler);
    }
    void add_param_replacer(typename SCloudVMPrivate<TreeType>::PARAM_REPLACER handler){
        d->substituteParams=std::move(handler);
    }
    void add_absolute_url_maker(typename SCloudVMPrivate<TreeType>::URL_MAKER handler){
        d->relativetoAbsolute=std::move(handler);
    }
    std::vector<Ui::Parameter> getParameters(){
        return d->parser.getParameters();
    }

protected:
    std::unique_ptr<SCloudVMPrivate<TreeType>> d;
};

