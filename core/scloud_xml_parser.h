#pragma once
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/utility/string_view.hpp>
#include "shttpparameters.h"
using boost::property_tree::ptree;
class SCloudParserPrivate;
class SCloudParser
{
public:
  SCloudParser(const std::string& str =std::string());
  SCloudParser(std::stringstream&& stream);
  ~SCloudParser();
  void load(const std::string& name);
  void load(std::stringstream& stream);
  ptree::value_type element(boost::string_view elem, boost::string_view id =boost::string_view());
  std::string defaultScreen() const;
  std::string name() const;
  std::string xml_base() const;
  std::vector<Ui::Parameter> getParameters();
  void PrintXml();
  void PrintXml(const ptree& elem);
  private:
  std::unique_ptr<SCloudParserPrivate> d;
};