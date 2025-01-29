#ifndef _YAML_DATA_H
#define _YAML_DATA_H

#include <string>
#include <map>
#include "yaml-cpp/yaml.h"

/*
 InputData:
      description: Information within Determine Location Request.
      type: object
      not:
        required: [ ecgi, ncgi ]
      properties:
        externalClientType:
          $ref: '#/components/schemas/ExternalClientType'
        correlationID:
          $ref: '#/components/schemas/CorrelationID'
        amfId:
          $ref: 'TS29571_CommonData.yaml#/components/schemas/NfInstanceId'
        locationQoS:
          $ref: '#/components/schemas/LocationQoS'
        supportedGADShapes:
          type: array
          items:
            $ref: '#/components/schemas/SupportedGADShapes'
          minItems: 1
*/

class Property
{
public:
    std::string m_name;
    std::string m_type;
    std::shared_ptr<std::string> m_itemType;
    std::shared_ptr<std::string> m_ref;
    std::shared_ptr<std::string> m_map_key;
    std::shared_ptr<std::string> m_map_value;
};

class Schema
{
public:
    bool isRequired(std::string property);
    void addRequiredProperty(std::string property);
    bool isBasicType();
    
    std::string m_name;
    std::string m_type; // integer, string, bool, number(float/double), object, array, map
    std::shared_ptr<std::string> m_itemType;
    std::vector<std::string> m_requiredProperties;
    std::vector<Property> m_properties;
    std::shared_ptr<std::string> m_map_key;
    std::shared_ptr<std::string> m_map_value;
};

class Path
{
public:
    void addOperation(std::string operation);
    void addResponse(std::string operation, std::string response);
    std::vector<std::string> getAllResponses(std::string operation);
    
    std::vector<std::string> m_operations; // put, post, delete, get, patch
    std::map<std::string, std::vector<std::string>> m_responses;
};

class YamlData
{
public:
    YamlData() {};
    YamlData(std::string fullYamlFileName, std::string yamlFileName);

    void loadData();
    void genSchemas();
    void loadProperty(std::string schemaName, std::string propName);
    Property loadProperty(std::string name, YAML::Node node);
    std::string loadSchema(std::string refType);
    Schema loadSchema(std::string name, YAML::Node node);
    
    std::string schemaLoaded(std::string schemaName);
    
    YAML::Node m_yamlNode;

    static std::map<std::string, YamlData*> yamlDataDict;

private:
    void loadPaths(YAML::Node node);
    Path loadPath(YAML::Node node);
    void loadSchemas(YAML::Node node);
    void loadRefSpec(std::string refSpec);
    void genSchemaDef(std::string schemaName);
    void genFromJsonToJsonFunc(std::string schemaName);
    std::string getType(std::string type);
    std::string getRefType(std::string ref);
    
private:
    std::string m_yamlFileName;
    std::map<std::string, Schema> m_schemaDict;
    std::map<std::string, Path> m_pathDict;
};

#endif
