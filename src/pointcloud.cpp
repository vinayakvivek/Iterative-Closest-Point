#include <iostream>
#include "pointcloud.h"
#include <cstring>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/string_cast.hpp>

Pointcloud::Pointcloud(string filename) {
    cout << "Reading scene from " << filename << " ..." << endl;
    char* fname = (char*)filename.c_str();
    fp_in.open(fname);
    if (!fp_in.is_open()) {
        cout << "Error reading from file - aborting!" << endl;
        throw;
    }
	int i = 0;
    while (fp_in.good()) {
        string line;
        i++;
        utilityCore::safeGetline(fp_in, line);
        if (!line.empty() && i%30==0) {
            vector<string> tokens = utilityCore::tokenizeString(line);
			glm::vec4 pt(atoi(tokens[2].c_str()), atoi(tokens[0].c_str()), atoi(tokens[1].c_str()), 1.0);
			points.push_back(pt);
        }
    }
}
