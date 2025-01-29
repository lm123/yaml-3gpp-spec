#ifndef __UTILITIES_H
#define __UTILITIES_H

#include <string>
#include <vector>
#include <map>

extern bool fileExists(const char *fileName);
extern void createSubDir(const char *specNo, const char *version);
extern void execCmd(const char *cmd, std::string &result);
extern void getAllYamlFileNames(const char *spec, const char *version, std::vector<std::string> &yamlFileNames);
extern std::string getLatestSpecFile(const char *specNo, const char *versionPrefix);
extern void downloadSpecFile(const char *specNo, const char *version);
extern void compareYamlNodePaths(std::string level2Name, std::string level1Name, std::string nodeName, YAML::Node node1, YAML::Node node2);
extern void compareYamlNodeSchemas(std::string level2Name, std::string level1Name, std::string nodeName, YAML::Node node1, YAML::Node node2,
                                   std::map<std::string, std::vector<std::string>> &updatedSchemas,
                                   std::vector<std::string> &newSchemas);
extern void genAllSchemas();
extern void clearYamlDataDict();

extern bool verbose;
extern char specLatestVersion[2];

#endif
