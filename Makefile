DiffYaml: main.cc utilities.cc yamldata.cc
	g++ -g -o DiffYaml main.cc utilities.cc yamldata.cc -lyaml-cpp -Iinc -Iyaml-cpp-master/include -Lyaml-cpp-master/build 
