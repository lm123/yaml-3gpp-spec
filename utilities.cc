#include <stdio.h>
#include <bits/stdc++.h>
#include <filesystem>
#include "yaml-cpp/yaml.h"

extern bool verbose;

bool fileExists(const char *fileName)
{
    if (std::filesystem::exists(fileName))
    {
        return true;
    }

    return false;
}
                 
void compareYamlNodePaths(std::string level2Name, std::string level1Name, std::string nodeName, YAML::Node node1, YAML::Node node2)
{
    if (node1.Type() == YAML::NodeType::Sequence)
    {
        return;
    }

    std::vector<std::string> added, removed;
    
    for (auto it=node1.begin(); it != node1.end(); it++)
    {
        std::string name = it->first.as<std::string>();

        if (!node2[name])
        {
            added.push_back(name);
        }
        else
        {
            compareYamlNodePaths(level1Name, nodeName, name, it->second, node2[name]);
        }
    }

    for (auto it=node2.begin(); it != node2.end(); it++)
    {
        std::string name = it->first.as<std::string>();
        if (!node1[name])
        {
            removed.push_back(name);
        }
    }

    if (nodeName == "paths" || nodeName == "responses")
    {
        if (added.size())
        {
            if (nodeName == "responses")
            {
                std::cout << level2Name;
                if (level2Name.length())
                {
                    std::cout << "  ";
                }

                std::cout << level1Name;
                if (level1Name.length())
                {
                    std::cout << "  ";
                }
            }

            std::cout << nodeName << " Added:\n";

            int count = 0;
            for (auto it : added)
            {
                if ((count % 5) == 0)
                {
                    std::cout << "\t";
                }

                std::cout << it << " ";

                if ((count % 5) == 4)
                {
                    std::cout << "\n";
                }

                count++;
            }

            if (count % 5)
            {
               std::cout << "\n";
            }
        }

        if (removed.size())
        {
            if (nodeName == "responses")
            {
              std::cout << level2Name;
              if (level2Name.length())
              {
                  std::cout << "  ";
              }

              std::cout << level1Name;
              if (level1Name.length())
              {
                  std::cout << "  ";
              }
            }
            std::cout << nodeName << " Removed:\n";

            int count = 0;
            for (auto it : removed)
            {
              if ((count % 5) == 0)
              {
                  std::cout << "\t" << it;
              }
              else
              {
                  std::cout << it;
              }

              std::cout << " ";

              if ((count % 5) == 4)
              {
                  std::cout << "\n";
              }

              count++;
            }

            if (count % 5)
            {
              std::cout << "\n";
            }
        }
    }
}

void compareYamlNodeSchemas(std::string level2Name, std::string level1Name, std::string nodeName, YAML::Node node1, YAML::Node node2,
                           std::map<std::string, std::vector<std::string>> &updatedSchemas,
                           std::vector<std::string> &newSchemas)
{
    if (node1.Type() == YAML::NodeType::Sequence)
    {
        return;
    }

    std::vector<std::string> added, removed;
    
    for (auto it=node1.begin(); it != node1.end(); it++)
    {
        std::string name = it->first.as<std::string>();

        if (!node2[name])
        {
            added.push_back(name);
        }
        else
        {
            compareYamlNodeSchemas(level1Name, nodeName, name, it->second, node2[name],
                                   updatedSchemas, newSchemas);
        }
    }

    for (auto it=node2.begin(); it != node2.end(); it++)
    {
        std::string name = it->first.as<std::string>();
        if (!node1[name])
        {
            removed.push_back(name);
        }
    }

    if (nodeName == "properties" || nodeName == "schemas")
    {
        if (added.size())
        {
            if (nodeName == "properties")
            {
                std::cout << level1Name;
                if (level1Name.length())
                {
                    std::cout << "  ";
                }
            }

            if (nodeName == "schemas")
            {
                newSchemas = added;
            }
            else if (nodeName == "properties")
            {
                updatedSchemas[level1Name] = added;
            }

            std::cout << nodeName << " Added:\n";

            int count = 0;
            for (auto it : added)
            {
                if ((count % 5) == 0)
                {
                    std::cout << "\t";
                }

                std::cout << it << " ";

                if ((count % 5) == 4)
                {
                    std::cout << "\n";
                }

                count++;
           }

           if (count % 5)
           {
               std::cout << "\n";
           }
        }

        if (removed.size())
        {
            if (nodeName == "properties")
            {
              std::cout << level1Name;
              if (level1Name.length())
              {
                  std::cout << "  ";
              }
            }
            std::cout << nodeName << " Removed:\n";

            int count = 0;
            for (auto it : removed)
            {
              if ((count % 5) == 0)
              {
                  std::cout << "\t" << it;
              }
              else
              {
                  std::cout << it;
              }

              std::cout << " ";

              if ((count % 5) == 4)
              {
                  std::cout << "\n";
              }

              count++;
            }

            if (count % 5)
            {
              std::cout << "\n";
            }
        }
    }
}

void createSubDir(const char *specNo, const char *version)
{
    char buf[80];

    sprintf(buf, "mkdir -p download/%s/%s", specNo, version);

    system(buf);
}

void execCmd(const char *cmd, std::string &result)
{
    FILE *pipe = popen(cmd, "r");

    if (!pipe)
    {
        printf("exec %s failed\n", cmd);
        return;
    }

    char buf[81] = {0};
    while (fgets(buf, 80, pipe) != NULL)
    {
        result.append(buf);
    }
}

void getAllYamlFileNames(const char *spec, const char *version, std::vector<std::string> &yamlFileNames)
{
    std::string res; 

    char buf[80];
    sprintf(buf, "ls download/%s/%s/*.yaml", spec, version);

    if (verbose)
    {
        printf("%s\n", buf);
    }

    execCmd(buf, res);

    std::string s;
    s.append(1, '\n');
    std::regex reg(s);
    std::vector<std::string> elems(std::sregex_token_iterator(res.begin(), res.end(), reg, -1), std::sregex_token_iterator());

    for (auto elem : elems)
    {
        std::string s;
        s.append(1, '/');

        std::regex reg(s);
        std::vector<std::string> elems_(std::sregex_token_iterator(elem.begin(), elem.end(), reg, -1), std::sregex_token_iterator());

        for (auto elem_ : elems_)
        {
            if (elem_.find(".yaml") != std::string::npos)
            {
                if (verbose)
                {
                    printf("%s\n", elem_.c_str());
            }

            yamlFileNames.push_back(elem_);
            }
        }
    } 
}

std::string getLatestSpecFile(const char *specNo, const char *versionPrefix)
{
    const char *lsAllSpecFiles = "curl -s -X GET https://www.3gpp.org/ftp/specs/archive/%s_series/%s"; 

    std::string spec = specNo;
    std::string series = spec.substr(0,2);
    std::string subSpec = spec.substr(2,3);
    std::string specDot = series + "." + subSpec;

    char buf[80];
    sprintf(buf, lsAllSpecFiles, series.c_str(), specDot.c_str()); 

    if (verbose)
    {
        printf("%s\n", buf);
    }

    std::string result;
    execCmd(buf, result);

    char str[result.length() + 1];
    strcpy(str, result.c_str());

    const char* delimiters = "\n";
    char* token = std::strtok(str, delimiters);

    std::string latestSpecPrefix = spec + "-" + versionPrefix;
    std::string latestSpec;

    while (token != NULL) {
        if (strstr(token, latestSpecPrefix.c_str()))
        {
            latestSpec = token;
        }

        token = std::strtok(NULL, delimiters);
    } 

    size_t pos = latestSpec.find(latestSpecPrefix);
    if (pos != std::string::npos)
    {
        latestSpec = latestSpec.substr(pos+6, 3);

        if (verbose)
        {
            printf("%s\n", latestSpec.c_str()); 
        }

        return std::string(latestSpec);
    }

    return "";
}

void downloadSpecFile(const char *specNo, const char *version)
{
    // curl -s -X GET -H 'Transfer-Encoding: chunked' https://www.3gpp.org/ftp/specs/archive/29_series/29.540/29540-h60.zip
    std::string spec = specNo;

    const char *downFileFmt = "curl -s -X GET -H 'Transfer-Encoding: chunked' https://www.3gpp.org/ftp/specs/archive/%s_series/%s/%s-%s.zip -o %s-%s.zip"; 

    std::string series = spec.substr(0,2);
    std::string subSpec = spec.substr(2,3);
    std::string specDot = series + "." + subSpec;

    char buf[256];

    std::string fileName = "download/" + std::string(specNo) + "/" + std::string(version) + "/" + std::string(specNo) + "-" + 
                           std::string(version) + ".zip";
    if (fileExists(fileName.c_str()))
    {
        if (verbose)
        {
            printf("file %s exists.\n", fileName.c_str());
        }
    }
    else
    {
        // download spec
        sprintf(buf, downFileFmt, series.c_str(), specDot.c_str(), spec.c_str(), version, spec.c_str(), version); 
    
        if (verbose)
        {
            printf("%s\n", buf);
        }

        system(buf);

        // move zip file
        sprintf(buf, "mv %s-%s.zip download/%s/%s/", spec.c_str(), version, spec.c_str(), version); 

        if (verbose)
        {
            printf("%s\n", buf);
        }

        system(buf);
    }

    // unzip
    sprintf(buf, "unzip -q -o download/%s/%s/%s-%s.zip -d download/%s/%s", spec.c_str(), version, spec.c_str(), version, spec.c_str(), version); 

    if (verbose)
    {
        printf("%s\n", buf);
    }

    system(buf);
}
