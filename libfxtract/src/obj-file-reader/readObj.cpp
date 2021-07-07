#include "../../include/obj-file-reader/readObj.h"

#include <fstream>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <Wt/WAny.h>
#include <stdlib.h>

namespace Converter {

    void ObjReader::objParser(const std::string &fileName, std::unique_ptr<std::vector<float>> &data)
    {
        std::string line;
        std::vector<gp_Pnt> tempFaceData, tempNormData;

        std::ifstream f(fileName.c_str());

        while(getline(f, line)){

            if(f){
                std::vector<std::string> splitLine;

                boost::split(splitLine, line, boost::is_any_of(" "), boost::algorithm::token_compress_on);

                if (splitLine[0] == "v")
                {
                    gp_Pnt point(
                        strToDouble(splitLine[1]),
                        strToDouble(splitLine[2]),
                        strToDouble(splitLine[3])
                    );

                    points.push_back(point);
                    
                } else if (splitLine[0] == "vn")
                {
                    gp_Pnt point(
                        strToDouble(splitLine[1]),
                        strToDouble(splitLine[2]),
                        strToDouble(splitLine[3])
                    );

                    normals.push_back(point);

                } else if (splitLine[0] == "vt")
                {
                    gp_Pnt point(
                        Wt::asNumber(splitLine[1]),
                        Wt::asNumber(splitLine[2]),
                        ((splitLine.size()>3) ? Wt::asNumber(splitLine[3]) : 0)
                    );

                    textures.push_back(point);

                } else if (splitLine[0] == "f")
                {
                    for (size_t i = 1; i < splitLine.size(); ++i)
                    {
                        std::vector<std::string> faceLine;
                        
			            // std::cout << "Size : " << splitLine.size() << std::endl;

                        boost::split(faceLine, splitLine[i], boost::is_any_of("/"), boost::algorithm::token_compress_off);
                        
                        // std::cout << "Size : " << faceLine.size() << std::endl;

			            // std::cout << faceLine[0] << " " << faceLine[1] << " " << faceLine[2] << std::endl;
                        int v =  Wt::asNumber(faceLine[0]);
                        int n =  std::stoi(faceLine[2]);

			            // std::cout << "Index : " << (n - 1) << std::endl;
                        tempFaceData.push_back(points[v - 1]);
                        tempNormData.push_back(normals[n - 1]);                        
                    }
                    
                    //std::cout << '\n';
                } else if (splitLine[0] == "#") {
	            ; // ignore comments
	       
	        } else if (splitLine[0] == " ") {
	            ; // ignore empty lines
	       
	        } else {
	            std::cerr << "ERROR in obj file: unknown line: " << line << "\n";
	        }
            }
        }

	std::cout << "Number of vertices : " << tempFaceData.size() << std::endl;
	std::cout << "Number of normals : " << tempNormData.size() << std::endl;
	
	makeVetexObject(tempFaceData, tempNormData, data); 
    }

    long double ObjReader::strToDouble(const std::string& S)
    {
        return atof(S.c_str());
    }

    void ObjReader::makeVetexObject(std::vector<gp_Pnt>& vertices, std::vector<gp_Pnt>& vnormals, std::unique_ptr<std::vector<float>> &data)
    {
        bool bvertices = false;
        for (size_t i = 0; i < vertices.size(); i++)
        {
            if (vertices[i].X >= 50 || vertices[i].Y >= 50 || vertices[i].Z >= 50)
            {
                bvertices = true;
                break;
            }            
        }

        if (bvertices)
        {
            for (size_t i = 0; i < vertices.size(); i++)
            {
                vertices[i].X = vertices[i].X / 50;
                vertices[i].Y = vertices[i].Y / 50;
                vertices[i].Z = vertices[i].Z / 50;
            }            
        } 

        bool bvnormals = false;
        for (size_t i = 0; i < vnormals.size(); i++)
        {
            if (vnormals[i].X >= 50 || vnormals[i].Y >= 50 || vnormals[i].Z >= 50)
            {
                bvnormals = true;
                break;
            }            
        }

        if (bvnormals)
        {
            for (size_t i = 0; i < vnormals.size(); i++)
            {
                vnormals[i].X = vnormals[i].X / 50;
                vnormals[i].Y = vnormals[i].Y / 50;
                vnormals[i].Z = vnormals[i].Z / 50;
            }            
        }        
        
        for (size_t i = 0; i < vertices.size(); ++i){
            data->push_back(vertices[i].X);
            data->push_back(vertices[i].Y);
            data->push_back(vertices[i].Z);
            data->push_back(vnormals[i].X);
            data->push_back(vnormals[i].Y);
            data->push_back(vnormals[i].Z);
        }
    }

}
