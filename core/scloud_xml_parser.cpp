#include "scloud_xml_parser.h"
#include <boost/fusion/include/find_if.hpp>
#include <iostream>   
class SCloudParserPrivate
{
    public:
    ptree tree;
    std::string _defaultScreen;
    std::string _name;
    std::string _xml_base;
    void init(){
        _defaultScreen = tree.get("cloudUiDescription.<xmlattr>.defaultScreen", std::string());
        _defaultScreen =_defaultScreen.empty() ?element("screen",std::string()).second.get("<xmlattr>.id", std::string()):_defaultScreen;
        _name = tree.get("cloudUiDescription.<xmlattr>.name", std::string());
        _xml_base = tree.get("cloudUiDescription.<xmlattr>.xml:base", std::string());
    }
    SCloudParserPrivate(const std::string& file){
        read_xml(file,tree);
        init();
    }   
    SCloudParserPrivate(std::stringstream& stream){
        
        read_xml(stream,tree);
        init();
    }
    ptree::value_type element(boost::string_view elem, boost::string_view id);   
};

SCloudParser::SCloudParser(const std::string& str)
{
    if(!str.empty()){
        load(str);
    }
    
}
SCloudParser::SCloudParser(std::stringstream&& stream)
{
    load(stream);
}
SCloudParser::~SCloudParser()
{

}
void SCloudParser::load(const std::string& name)
{
    d=std::make_unique<SCloudParserPrivate>(name);
}
void SCloudParser::load(std::stringstream& stream)
{
    d=std::make_unique<SCloudParserPrivate>(stream);
}
ptree::value_type SCloudParser::element(boost::string_view elem, boost::string_view id)
{
     return d->element(elem,id);
}
ptree::value_type SCloudParserPrivate::element(boost::string_view elem, boost::string_view id)
{
    auto& children = tree.get_child("cloudUiDescription");
    auto iter = std::find_if(std::begin(children),std::end(children),[&](auto& v){
        auto vid = v.second.get("<xmlattr>.id", std::string());
        return  id.empty() ? (v.first == elem) : ((v.first == elem) && (vid ==  id));
    });
    if(iter != std::end(children)){
        return *iter;
    }
    return ptree::value_type{};
}
std::string SCloudParser::defaultScreen() const
{
    return d->_defaultScreen;
}
std::string SCloudParser::name() const
{
    return d->_name;
}
std::string SCloudParser::xml_base() const
{
    return d->_xml_base;
}
std::vector<Ui::Parameter> SCloudParser::getParameters()
{
    auto& children = d->tree.get_child("cloudUiDescription");
    std::vector<Ui::Parameter> params;
    for(auto p:children){
        if(p.first == "parameter"){
           auto name =  p.second.get("<xmlattr>.name", std::string());
           auto type =  p.second.get("<xmlattr>.type", std::string());
           auto inherit =  p.second.get("<xmlattr>.inherit", false);
           auto deflt =  p.second.get("<xmlattr>.default", std::string());
           params.emplace_back(Ui::Parameter(name,deflt,type,inherit));
        }
    }
   
   
    return params;
}
template<typename T>
void printTree(const T& ptree){
     for(const typename T::value_type &v : ptree)
     {
        std::cout << v.first;
        printTree(v.second);
     }
     
}
void SCloudParser::PrintXml()
{
    if(d){
        printTree(d->tree);
    }
}

void SCloudParser::PrintXml(const ptree& elem)
{
    
    printTree(elem);
    
}
