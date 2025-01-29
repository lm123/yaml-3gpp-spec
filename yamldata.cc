#include <yamldata.h>
#include <iostream>
#include "utilities.h"

std::map<std::string, YamlData*> YamlData::yamlDataDict;

void genAllSchemas()
{
    for (auto it : YamlData::yamlDataDict)
    {
        YamlData *pYamlData = it.second;
        pYamlData->genSchemas();
    }
}

void clearYamlDataDict()
{
    for (auto it : YamlData::yamlDataDict)
    {
        delete it.second;
    }

    YamlData::yamlDataDict.clear();
}

YamlData::YamlData(std::string fullYamlFileName, std::string yamlFileName)
{
    m_yamlFileName = yamlFileName;
    
    if (yamlDataDict.find(yamlFileName) != yamlDataDict.end())
    {
        if (verbose)
        {
            std::cout << yamlFileName << " is already loaded.\n";
        }
        
        return;
    }

    if (!fileExists(fullYamlFileName.c_str()))
    {
        if (verbose)
        {
            std::cout << fullYamlFileName << " is not existed.\n";
        }
        
        return;
    }

    try
    {
        m_yamlNode = YAML::LoadFile(fullYamlFileName);
    } catch(YAML::ParserException& e) {
        std::cout << e.what() << "\n";
        return;
    } catch(...) {
        std::cout << "catch exception\n";
        return;
    }

    if (verbose)
    {
        std::cout << "yamlDataDict[" << yamlFileName << "]\n";
    }
    
    yamlDataDict[yamlFileName] = this;
}

void YamlData::loadData()
{
    loadPaths(m_yamlNode["paths"]);
    loadSchemas(m_yamlNode["components"]["schemas"]);
}

void YamlData::genSchemas()
{
    std::cout << "------ " << m_yamlFileName << " ------\n";
    for (auto it : m_schemaDict)
    {
         genSchemaDef(it.first);
    }

    for (auto it : m_schemaDict)
    {
         genFromJsonToJsonFunc(it.first);
    }
}

/*
struct UeSmsContextData {
    std::string supi;
    std::shared_ptr<std::string> pei;
    std::string amfId;
    std::shared_ptr<std::vector<LS_SBI::Guami>> guamis;
    LS_SBI::AccessType accessType;
    std::shared_ptr<std::string> gpsi;
    std::shared_ptr<LS_SBI::UserLocation> ueLocation;
    std::shared_ptr<std::string> ueTimeZone;
    std::shared_ptr<LS_SBI::TraceData> traceData;
    std::shared_ptr<std::vector<LS_SBI::BackupAmfInfo>> backupAmfInfo;
    std::shared_ptr<std::string> udmGroupId;
    std::shared_ptr<std::string> routingIndicator;
};

*/
void YamlData::genSchemaDef(std::string schemaName)
{
    if (verbose)
    {
        std::cout << "genSchemaDef: " << schemaName << "\n";
    }
    
    Schema schema = m_schemaDict[schemaName];

    if (schema.isBasicType())
    {
        return;
    }

    if (schema.m_type == "map" && schema.m_map_key.use_count() && schema.m_map_value.use_count())
    {
        std::cout << "typedef std::map<" << * schema.m_map_key << "," << * schema.m_map_value << "> " << schemaName << ";\n";
        return;
    }
    
    std::cout << "struct " << schemaName << " {\n";
    for (Property prop : schema.m_properties)
    {
        if (schema.isRequired(prop.m_name))
        {
            if (prop.m_type != "array")
            {
                if (prop.m_type != "map")
                {
                    std::cout << "\t" << getType(prop.m_type) << " " << prop.m_name << ";\n"; 
                }
                else if (prop.m_map_key.use_count() && prop.m_map_value.use_count())
                {
                    std::cout << "\tstd::map<" << * prop.m_map_key << "," << * prop.m_map_value << "> " << prop.m_name << ";\n";
                }
            }
            else 
            {   
                if (prop.m_itemType.use_count())
                std::cout << "\tstd::vector<" << getType(* prop.m_itemType) << "> " << prop.m_name << ";\n"; 
            }
        }
        else 
        {
            if (prop.m_type != "array")
            {
                if (prop.m_type != "map")
                {
                     std::cout << "\tstd::shared_ptr<" << getType(prop.m_type) << "> " << prop.m_name << ";\n";
                }
                else 
                {
                     std::cout << "\tstd::shared_ptr<std::map<" << * prop.m_map_key << "," << * prop.m_map_value << ">> " << prop.m_name << ";\n";
                }
            }
            else
            {
                if (prop.m_itemType.use_count())
                std::cout << "\tstd::shared_ptr<std::vector<" << getType(* prop.m_itemType) << ">> " << prop.m_name << ";\n";
            }
        }
    }

    std::cout << "}\n";
}

/*
inline void from_json(const json & j, LS_SBI::LocationData& x) {
    x.accuracyFulfilmentIndicator = LS_SBI::get_optional<std::string>(j, "accuracyFulfilmentIndicator");
    x.ageOfLocationEstimate = LS_SBI::get_optional<int64_t>(j, "ageOfLocationEstimate");
    x.altitude = LS_SBI::get_optional<double>(j, "altitude");
    x.barometricPressure = LS_SBI::get_optional<int64_t>(j, "barometricPressure");
    x.civicAddress = LS_SBI::get_optional<LS_SBI::CivicAddress>(j, "civicAddress");
    x.ecgi = LS_SBI::get_optional<LS_SBI::Ecgi>(j, "ecgi");
    x.gnssPositioningDataList = LS_SBI::get_optional<std::vector<LS_SBI::GnssPositioningMethodAndUsage>>(j, "gnssPositioningDataList");
    x.locationEstimate = j.at("locationEstimate").get<LS_SBI::GeographicArea>();
    x.ncgi = LS_SBI::get_optional<LS_SBI::Ncgi>(j, "ncgi");
    x.positioningDataList = LS_SBI::get_optional<std::vector<LS_SBI::PositioningMethodAndUsage>>(j, "positioningDataList");
    x.velocityEstimate = LS_SBI::get_optional<LS_SBI::VelocityEstimate>(j, "velocityEstimate");
}

inline void to_json(json & j, const LS_SBI::LocationData & x) {
    j = json::object();
    if (x.accuracyFulfilmentIndicator) j["accuracyFulfilmentIndicator"] = x.accuracyFulfilmentIndicator;
    if (x.ageOfLocationEstimate) j["ageOfLocationEstimate"] = x.ageOfLocationEstimate;
    if (x.altitude) j["altitude"] = x.altitude;
    if (x.barometricPressure) j["barometricPressure"] = x.barometricPressure;
    if (x.civicAddress) j["civicAddress"] = x.civicAddress;
    if (x.ecgi) j["ecgi"] = x.ecgi;
    if (x.gnssPositioningDataList) j["gnssPositioningDataList"] = x.gnssPositioningDataList;
    j["locationEstimate"] = x.locationEstimate;
    if (x.ncgi) j["ncgi"] = x.ncgi;
    if (x.positioningDataList) j["positioningDataList"] = x.positioningDataList;
    if (x.velocityEstimate) j["velocityEstimate"] = x.velocityEstimate;
}
*/

void YamlData::genFromJsonToJsonFunc(std::string schemaName)
{
    if (verbose)
    {
        std::cout << "genFromJsonToJsonFunc: " << schemaName << "\n";
    }
    
    Schema schema = m_schemaDict[schemaName];

    if (schema.isBasicType())
    {
        return;
    }

    if (schema.m_type == "map" && schema.m_map_key.use_count() && schema.m_map_value.use_count())
    {
        std::cout << "typedef std::map<" << * schema.m_map_key << "," << * schema.m_map_value << "> " << schemaName << ";\n";
        return;
    }

    // from_json
    // inline void from_json(const json & j, LS_SBI::LocationData& x) {
    std::cout << "inline void from_json(const json & j, LS_SBI::" << schemaName << "& x) {\n";
    for (Property prop : schema.m_properties)
    {
        if (schema.isRequired(prop.m_name))
        {
            // x.locationEstimate = j.at("locationEstimate").get<LS_SBI::GeographicArea>();
            std::cout << "\tx." << prop.m_name << " = j.at(\"" << prop.m_name << "\").get<";
            if (prop.m_type != "array")
            {
                if (prop.m_type != "map")
                {
                    std::cout << getType(prop.m_type); 
                }
                else if (prop.m_map_key.use_count() && prop.m_map_value.use_count())
                {
                    std::cout << "std::map<" << * prop.m_map_key << "," << * prop.m_map_value << ">";
                }
            }
            else 
            {   
                if (prop.m_itemType.use_count())
                {
                    std::cout << "std::vector<" << getType(* prop.m_itemType) << ">"; 
                }
            }
            std::cout << ">();\n";
        }
        else 
        {
            // x.altitude = LS_SBI::get_optional<double>(j, "altitude");
            std::cout << "\tx." << prop.m_name << " = LS_SBI::get_optional<";
            if (prop.m_type != "array")
            {
                if (prop.m_type != "map")
                {
                     std::cout << getType(prop.m_type);
                }
                else 
                {
                     std::cout << "std::map<" << * prop.m_map_key << "," << * prop.m_map_value << ">";
                }
            }
            else
            {
                if (prop.m_itemType.use_count())
                {
                    std::cout << "std::vector<" << getType(* prop.m_itemType) << ">";
                }
            }
            std::cout << ">(j, \"" << prop.m_name << "\");\n";
        }
    }

    std::cout << "}\n";

    // to_json
    // inline void to_json(json & j, const LS_SBI::LocationData & x) {
    // j = json::object();
    std::cout << "inline void to_json(const json & j, LS_SBI::" << schemaName << "& x) {\n";
    std::cout << "\tj = json::object();\n";
    
    for (Property prop : schema.m_properties)
    {
        if (schema.isRequired(prop.m_name))
        {
            // j["locationEstimate"] = x.locationEstimate;
            std::cout << "\tj[\"" << prop.m_name << "\"] = x." << prop.m_name << ";\n";
        }
        else 
        {
            // if (x.ncgi) j["ncgi"] = x.ncgi;
            std::cout << "\tif (x." << prop.m_name << ") j[\"" << prop.m_name << "\"] = x." << prop.m_name << ";\n";
        }
    }

    std::cout << "}\n";    
}


/*
/ue-contexts/{supi}:
  put:
    summary: Activate SMS Service for a given UE
    operationId: SMServiceActivation
    tags:
      - Activation of SMS service
    parameters:
      - name: supi
        in: path
        required: true
        description: Subscriber Permanent Identifier (SUPI)
        schema:
          type: string
    requestBody:
      content:
        application/json:
          schema:
            $ref: '#/components/schemas/UeSmsContextData'
      required: true
    responses:
      '201':
*/

void YamlData::loadPaths(YAML::Node node)
{
    for (auto it=node.begin(); it != node.end(); it++)
    {
        std::string name = it->first.as<std::string>();

        if (verbose)
        {
            std::cout << "Path " << name << "\n";
        }
        
        Path path = loadPath(it->second);

        m_pathDict[name] = path;
    }
}

Path YamlData::loadPath(YAML::Node node)
{
    Path path;
    
    for (auto it=node.begin(); it != node.end(); it++)
    {
        std::string name = it->first.as<std::string>();

        path.addOperation(name);

        if (verbose)
        {
            std::cout << "Operation " << name << "\n";
        }

        YAML::Node oper = node[name];
        if (oper["responses"])
        {
            YAML::Node respNode = oper["responses"];
            for (auto operit = respNode.begin(); operit != respNode.end(); operit++)
            {
                std::string resp = operit->first.as<std::string>();
                path.addResponse(name, resp);

                if (verbose)
                {
                    std::cout << "Response " << resp << "\n";
                }
            }
        }
    }

    return path;
}

/*
schemas:

  UeSmsContextData:
    description: Represents the information used for activating the SMS service for a service user, or updating the parameters of the SMS service.
    type: object
    required:
      - supi
      - amfId
      - accessType
    properties:
      supi:
        $ref: 'TS29571_CommonData.yaml#/components/schemas/Supi'
      pei:
        $ref: 'TS29571_CommonData.yaml#/components/schemas/Pei'
      amfId:
        $ref: 'TS29571_CommonData.yaml#/components/schemas/NfInstanceId'
      guamis:
        type: array
        items:
          $ref: 'TS29571_CommonData.yaml#/components/schemas/Guami'
        minItems: 1
*/
void YamlData::loadSchemas(YAML::Node node)
{
    for (auto it=node.begin(); it != node.end(); it++)
    {
        std::string name = it->first.as<std::string>();

        if (verbose)
        {
            std::cout << "schema name: " << name << "\n";
        }
        
        loadSchema(name, it->second);
    }
}

Schema YamlData::loadSchema(std::string name, YAML::Node node)
{
    if (verbose)
    {
        std::cout << m_yamlFileName << " loadSchema(" << name << ")\n";
        std::cout << "node:" << node << "\n";
    }

    Schema schema;

    if (node["type"])
    {
        schema.m_type = node["type"].as<std::string>();
        if (schema.m_type == "object")
        {
            if (node["additionalProperties"])
            {
                std::string ref = node["additionalProperties"]["$ref"].as<std::string>();
                schema.m_map_value = std::make_shared<std::string>(getRefType(ref));
                
                std::string desc = node["description"].as<std::string>();
                size_t pos_s = desc.find("where");
                size_t pos_e = desc.find("serves as key");
                if (pos_s != std::string::npos && pos_e != std::string::npos)
                {
                    schema.m_map_key = std::make_shared<std::string>(desc.substr(pos_s + 6, pos_e - pos_s - 7));
                }
                
                schema.m_type = "map";
            }
            else 
            {
                schema.m_type = name;
            }
        }
        else if (schema.m_type == "array")
        {
             if (node["items"]["$ref"])
             {
                 std::string itemType = getRefType(node["items"]["$ref"].as<std::string>());
                 schema.m_itemType = std::make_shared<std::string>(itemType);
             }
        }
        else if (schema.m_type == "number")
        {
             if (node["format"])
             {
                 schema.m_type = node["format"].as<std::string>();
             }
        }
    }
    else if (node["anyOf"])
    {
        YAML::Node anyOf = node["anyOf"];
        std::vector<YAML::Node> nodes = anyOf.as<std::vector<YAML::Node>>();

        for (auto it : nodes)
        {
            if (it["type"])
            {
                schema.m_type = it["type"].as<std::string>();
            }
            else if (it["$ref"])
            {
                std::string ref = it["$ref"].as<std::string>();
                schema.m_type = getRefType(ref);            
            }
        }
    }
    else if (node["allOf"]) // TODO
    {
    }
    else if (node["oneof"]) // TODO
    {
    }
    
    if (node["required"])
    {
        schema.m_requiredProperties = node["required"].as<std::vector<std::string>>();
    }

    if (node["properties"])
    {
        YAML::Node props = node["properties"];
        for (auto it = props.begin(); it != props.end(); it++)
        {
            Property prop = loadProperty(it->first.as<std::string>(), it->second);
            schema.m_properties.push_back(prop);
        }
    }

    m_schemaDict[name] = schema;

    if (verbose)
    {
        std::cout << node << "\n";
        std::cout << m_yamlFileName << " loadSchema(" << name << ") return " << schema.m_type << "\n";
    }

    return schema;
}

std::string YamlData::loadSchema(std::string refType)
{
    Schema schema;

    if (verbose)
    {
        std::cout << m_yamlFileName << " loadSchema(" << refType << ")\n";
    }
    
    schema = loadSchema(refType, m_yamlNode["components"]["schemas"][refType]);

    if (verbose)
    {
        std::cout << m_yamlFileName << " loadSchema(" << refType << ") return " << schema.m_type << "\n";
    }
    
    return schema.m_type;
}

Property YamlData::loadProperty(std::string name, YAML::Node node)
{
    if (verbose)
    {
        std::cout << "loadProperty: name(" << name << ")\n";
        std::cout << node << "\n";
    }
    
    Property prop;
    
    prop.m_name = name;
    
    for (auto it = node.begin(); it != node.end(); it++)
    {
        std::string name = it->first.as<std::string>();
        if (name == "$ref")
        {
            prop.m_ref = std::make_shared<std::string>(it->second.as<std::string>());
            prop.m_type = getRefType(* prop.m_ref);
        }
        else if (name == "type")
        {
            prop.m_type = it->second.as<std::string>();
            if (prop.m_type == "array")
            {
                 if (node["items"]["$ref"])
                 {
                     std::string itemType = getRefType(node["items"]["$ref"].as<std::string>());
                     prop.m_itemType = std::make_shared<std::string>(itemType);
                 }
                 else if (node["items"]["type"])
                 {
                     std::string itemType = node["items"]["type"].as<std::string>();
                     prop.m_itemType = std::make_shared<std::string>(itemType);
                 }
            }
            else if (prop.m_type == "number")
            {
                prop.m_type = node["format"].as<std::string>();
            }
        }
        else if (name == "additionalProperties")
        {
            std::string ref = it->second["$ref"].as<std::string>();
            prop.m_map_value = std::make_shared<std::string>(getRefType(ref));

            std::string desc = node["description"].as<std::string>();
            size_t pos_s = desc.find("where");
            size_t pos_e = desc.find("serves as key");
            if (pos_s != std::string::npos && pos_e != std::string::npos)
            {
                prop.m_map_key = std::make_shared<std::string>(desc.substr(pos_s + 6, pos_e - pos_s - 7));
                prop.m_type = "map";
            }
        }
    }
    
    if (verbose)
    {
        std::cout << "Property " << prop.m_name << " type " << prop.m_type << "\n";
          
        if (prop.m_ref)
        {
            std::cout << "ref " << * prop.m_ref << "\n";
        }
    }

    return prop;
}

void YamlData::loadProperty(std::string schemaName, std::string propName)
{
    if (verbose)
    {
        std::cout << "loadProperty:" << schemaName << "," << propName << "\n";
    }
    
    Schema schema;
    if (m_schemaDict.find(schemaName) == m_schemaDict.end())
    {
        schema.m_name = schemaName;
        schema.m_type = schemaName;
        m_schemaDict[schemaName] = schema;

        if (verbose)
        {
            std::cout << "m_schemaDict[" << schemaName << "]\n";
        }
    }
    else
    {
        schema = m_schemaDict[schemaName];
    }
    
    Property prop = loadProperty(propName, m_yamlNode["components"]["schemas"][schemaName]["properties"][propName]);
    schema.m_properties.push_back(prop);

    m_schemaDict[schemaName] = schema;
    if (verbose)
    {
        std::cout << "yamlDataDict[" << m_yamlFileName << "]\n";
    }
}

void YamlData::loadRefSpec(std::string refSpec)
{
    std::string specNo = refSpec.substr(2, 5).c_str();
    std::string version = getLatestSpecFile(specNo.c_str(), specLatestVersion);

    if (verbose)
    {
        std::cout << version << "\n";
    }

    createSubDir(specNo.c_str(), version.c_str());
    downloadSpecFile(specNo.c_str(), version.c_str());

    char fullfileName[128];
    sprintf(fullfileName, "download/%s/%s/%s", specNo.c_str(), version.c_str(), refSpec.c_str());

    if (verbose)
    {
        std::cout << fullfileName << "\n";
    }
    
    YamlData* pYamlData = new YamlData(fullfileName, refSpec);
}

std::string YamlData::schemaLoaded(std::string schemaName)
{
    std::string ret = "";
    
    if (m_schemaDict.find(schemaName) != m_schemaDict.end())
    {
        ret = m_schemaDict[schemaName].m_type;
        if (ret == "object")
        {
            ret = schemaName;
        }
    }

    if (verbose)
    {
        std::cout << schemaName << " is " << ret << "\n";
    }
    
    return ret;
}

std::string YamlData::getType(std::string type)
{
    if (type == "string")
    {
        return "std::string";
    }
    else if (type == "integer")
    {
        return "int";
    }
    else if (type == "boolean")
    {
        return "bool";
    }
    else if (type == "double" || type == "float")
    {
        return type;
    }
    else 
    {
        return "LS_SBI::" + type;
    }
}

std::string YamlData::getRefType(std::string ref)
{
    size_t pos = ref.find("#");
    std::string refSpec;
    if (pos != std::string::npos)
    {
        refSpec = ref.substr(0, pos);
        if (verbose)
        {
            std::cout << "refSpec:" << refSpec << "\n";
        }

        if (refSpec.length())
        {
            if (yamlDataDict.find(refSpec) == yamlDataDict.end())
            {
                loadRefSpec(refSpec);
            }
        }
        else
        {
            refSpec = m_yamlFileName;
        }
    }

    pos =  ref.find_last_of("/");
    std::string refType = ref.substr(pos+1, ref.length() - pos);
    if (verbose)
    {
        std::cout << ref << " , " << refSpec << " , " << refType << "\n";
    }

    std::string type = yamlDataDict[refSpec]->schemaLoaded(refType);
    if (type.length() == 0)
    {
        type = yamlDataDict[refSpec]->loadSchema(refType);
    }

    return type;
}

bool Schema::isRequired(std::string property)
{
    bool res = false;
    for (auto prop : m_requiredProperties)
    {
        if (property == prop)
        {
            res = true;
        }
    }

    if (verbose)
    {
        std::cout << "return " << res << "\n";
    }
    
    return res;
}

void Schema::addRequiredProperty(std::string property)
{
    m_requiredProperties.push_back(property);
}

bool Schema::isBasicType()
{
    if (m_type == "string" || m_type == "bool" || m_type == "float" || m_type == "integer" || m_type == "number")
    {
        return true;
    }

    return false;
}

void Path::addOperation(std::string operation)
{
    m_operations.push_back(operation);
}

void Path::addResponse(std::string operation, std::string response)
{
    m_responses[operation].push_back(response);
}

std::vector<std::string> Path::getAllResponses(std::string operation)
{
    std::vector<std::string> res;
    
    if (m_responses.find(operation) == m_responses.end())
    {
        if (verbose)
        {
            std::cout << operation << " not existed\n";
        }
        
        return res;
    }

    res = m_responses[operation];

    return res;
}

