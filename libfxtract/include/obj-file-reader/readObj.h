#pragma once

#include <vector>
#include <string>
#include <memory>
#include <boost/tuple/tuple.hpp>

namespace Converter {

    /* Data stucture representing a geometric point (x, y, z) */
    struct gp_Pnt_ {
    public:
        float X;
        float Y;
        float Z;
        
        gp_Pnt_(float x_, float y_, float z_)
        {
            X = x_;
            Y = y_;
            Z = z_;
        }
    };
    
    using gp_Pnt = struct gp_Pnt_;

class ObjReader {

    /*
    * vector of all vertices 'v'
    */ 
    std::vector<gp_Pnt> points;

    /*
    *   vector of all texture values 'vt'
    */
    std::vector<gp_Pnt> textures;

    /*
    *   vector of all vertex normal 'vn'
    */
    std::vector<gp_Pnt> normals;

    /**
     *    array of all vertex components.
     */
    std::vector<long double> faceData;

public:
    /**
    *   This method parses an obj file and stores its points, texture and normal
    *   values.
    */
    void objParser(const std::string &fileName, std::unique_ptr<std::vector<float>> &data);

private:

    /*
    *   This method converts a string to a long double.
    */ 
    long double strToDouble(const std::string& s);

    /*
    *   This method breaks a 3D vertex into components x,y,z and assigns 
    *   each componet to faceData.
    */ 
    void makeVetexObject(std::vector<gp_Pnt>& vertices, std::vector<gp_Pnt>& vnormals, std::unique_ptr<std::vector<float>> &data);

};

}