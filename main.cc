#include <stdio.h>
#include <string>
#include <vector>
#include <bits/stdc++.h>
#include <filesystem>
#include "yaml-cpp/yaml.h"
#include "utilities.h"
#include "yamldata.h"

bool verbose = false;
char specLatestVersion[2];

using std::string;
using std::vector;
using std::cout;
using std::max;

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cout << "Usage: DiffYaml 29540 h60 f40\n";
        return 0;
    }

    if (argc > 4)
    {
        if (!strcmp(argv[4], "-v"))
        {
            verbose = true;
        }
    }

    specLatestVersion[0] = max(argv[2][0], argv[3][0]);
    specLatestVersion[1] = '\0';

    cout << "Latest Spec Version: " + std::string(specLatestVersion) + "\n";
    
    //std::string res = getLatestSpecFile(argv[1], "h");
    //printf("res = %s\n", res.c_str());

    // create sub directory
    createSubDir(argv[1], argv[2]);
    createSubDir(argv[1], argv[3]); 

    // download sepc file
    vector<std::string> yamlFileNames; 
    downloadSpecFile(argv[1], argv[2]);
    downloadSpecFile(argv[1], argv[3]);
    getAllYamlFileNames(argv[1], argv[2], yamlFileNames);

    for (auto yamlFileName : yamlFileNames)
    {
        char file1[128], file2[128];
        sprintf(file1, "download/%s/%s/%s", argv[1], argv[2], yamlFileName.c_str());
        sprintf(file2, "download/%s/%s/%s", argv[1], argv[3], yamlFileName.c_str());

        cout << "===============================================\n";
        cout << argv[1] << " " << argv[2] << "<-->" << argv[3] << " ---- " << yamlFileName <<"\n";

        std::map<std::string, std::vector<std::string>> updatedSchemas;
        std::vector<std::string> newSchemas;
        
        try {
            YAML::Node data1 = YAML::LoadFile(file1);
            YAML::Node data2 = YAML::LoadFile(file2);

            compareYamlNodePaths("", "", "paths", data1["paths"], data2["paths"]);
            compareYamlNodeSchemas("", "", "components", data1["components"], data2["components"],
                                   updatedSchemas, newSchemas);
        } catch(YAML::ParserException& e) {
            cout << e.what() << "\n";
        } catch(...) {
            cout << "catch exception\n";
        }

        std::string fullFileName;
        if (argv[2][0] == specLatestVersion[0])
        {
            fullFileName = file1;
        }
        else
        {
            fullFileName = file2;
        }

        YamlData *pYamlData = new YamlData(fullFileName, yamlFileName);
        
        for (auto it : updatedSchemas)
        {
            std::string schema = it.first;
            std::vector<std::string> props = it.second;

            for (auto prop : props)
            {
                pYamlData->loadProperty(schema, prop);
            }
        }
        
        for (auto newSchema : newSchemas)
        {
            pYamlData->loadSchema(newSchema, pYamlData->m_yamlNode["components"]["schemas"][newSchema]);
        }

        genAllSchemas();
    
        clearYamlDataDict();
    }

    return 1;
}
