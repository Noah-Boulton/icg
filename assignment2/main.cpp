/*
 * CSC 305 201801 UVIC
 * The purpose of this source file is to demonstrate the Mesh class which you may use in assignment 2
 * Its only functionality is to render vertices/normals/textures and load textures from png files
 * A demonstration of an ImGui menu window is also included in this file
*/
#include "Mesh/Mesh.h"
#include "OpenGP/GL/glfw_helpers.h"

#include <OpenGP/types.h>
#include <OpenGP/MLogger.h>
#include <OpenGP/GL/Application.h>
#include <OpenGP/GL/ImguiRenderer.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string.h>
#include <stdio.h>

using namespace OpenGP;
using namespace std;

//Method definitions
void cube();
void cylinder(int n);
void sphere(int n, int m);
void torus(int n, int m, float r);
bool read(string filename, std::vector<Vec3> &vertList, std::vector<unsigned int> &indexList, std::vector<Vec2> &textList, std::vector<Vec3> &faceText, std::vector<Vec3> &normList);
bool WriteObj(string filename, string shape, std::vector<Vec3> &vertList, std::vector<Vec3> &faceList, std::vector<Vec2> &textList, std::vector<Vec3> &faceText, std::vector<Vec3> &normList);

int main() {
    //Call the 4 shape methods which generate the triangle meshes which are written to .obj files
    cube();
    cylinder(18);
    sphere(32, 32);
    torus(16, 16, 0.25f);


    Application app;
    ImguiRenderer imrenderer;

    Mesh torusMesh;

    /// Example rendering a mesh
    /// Call to compile shaders
    torusMesh.init();
    std::vector<Vec3> vertListTorus;
    std::vector<unsigned int> indexListTorus;
    std::vector<Vec2> textListTorus;
    std::vector<Vec3> faceTextTorus;
    std::vector<Vec3> normListTorus;

    //Read in the mesh that was generated and display in popup window
    read("/Users/nboulton/Desktop/icg/data/torus.obj", vertListTorus, indexListTorus, textListTorus, faceTextTorus, normListTorus);

    torusMesh.loadVertices(vertListTorus, indexListTorus);

    auto &window4 = app.create_window([&](Window &window){
        int width, height;
        std::tie(width, height) = window.get_size();

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glClearColor(0.0f, 0.0f, 0.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /// Wireframe rendering, might be helpful when debugging your mesh generation
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

        float ratio = width / (float) height;
        Mat4x4 modelTransform = Mat4x4::Identity();
        Mat4x4 model = modelTransform.matrix();
        Mat4x4 projection = OpenGP::perspective(70.0f, ratio, 0.1f, 10.0f);

        //camera movement
        float time = .5f * (float)glfwGetTime();
        Vec3 cam_pos(2*cos(time), 2.0, 2*sin(time));
        Vec3 cam_look(0.0f, 0.0f, 0.0f);
        Vec3 cam_up(0.0f, 1.0f, 0.0f);
        Mat4x4 view = OpenGP::lookAt(cam_pos, cam_look, cam_up);

        torusMesh.draw(model, view, projection);
    });
    window4.set_title("Torus");

    Mesh sphereMesh;

    /// Example rendering a mesh
    /// Call to compile shaders
    sphereMesh.init();
    std::vector<Vec3> vertListSphere;
    std::vector<unsigned int> indexListSphere;
    std::vector<Vec2> textListSphere;
    std::vector<Vec3> faceTextSphere;
    std::vector<Vec3> normListSphere;
    read("/Users/nboulton/Desktop/icg/data/sphere.obj", vertListSphere, indexListSphere, textListSphere, faceTextSphere, normListSphere);

    sphereMesh.loadVertices(vertListSphere, indexListSphere);

    auto &window3 = app.create_window([&](Window &window){
        int width, height;
        std::tie(width, height) = window.get_size();

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glClearColor(0.0f, 0.0f, 0.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /// Wireframe rendering, might be helpful when debugging your mesh generation
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

        float ratio = width / (float) height;
        Mat4x4 modelTransform = Mat4x4::Identity();
        Mat4x4 model = modelTransform.matrix();
        Mat4x4 projection = OpenGP::perspective(70.0f, ratio, 0.1f, 10.0f);

        //camera movement
        float time = .5f * (float)glfwGetTime();
        Vec3 cam_pos(2*cos(time), 2.0, 2*sin(time));
        Vec3 cam_look(0.0f, 0.0f, 0.0f);
        Vec3 cam_up(0.0f, 1.0f, 0.0f);
        Mat4x4 view = OpenGP::lookAt(cam_pos, cam_look, cam_up);

        sphereMesh.draw(model, view, projection);
    });
    window3.set_title("Sphere");

    Mesh cylinderMesh;

    /// Example rendering a mesh
    /// Call to compile shaders
    cylinderMesh.init();
    std::vector<Vec3> vertListCylinder;
    std::vector<unsigned int> indexListCylinder;
    std::vector<Vec2> textListCylinder;
    std::vector<Vec3> faceTextCylinder;
    std::vector<Vec3> normListCylinder;
    read("/Users/nboulton/Desktop/icg/data/cylinder.obj", vertListCylinder, indexListCylinder, textListCylinder, faceTextCylinder, normListCylinder);

    cylinderMesh.loadVertices(vertListCylinder, indexListCylinder);

    auto &window2 = app.create_window([&](Window &window){
        int width, height;
        std::tie(width, height) = window.get_size();

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glClearColor(0.0f, 0.0f, 0.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /// Wireframe rendering, might be helpful when debugging your mesh generation
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

        float ratio = width / (float) height;
        Mat4x4 modelTransform = Mat4x4::Identity();
        Mat4x4 model = modelTransform.matrix();
        Mat4x4 projection = OpenGP::perspective(70.0f, ratio, 0.1f, 10.0f);

        //camera movement
        float time = .5f * (float)glfwGetTime();
        Vec3 cam_pos(2*cos(time), 2.0, 2*sin(time));
        Vec3 cam_look(0.0f, 0.0f, 0.0f);
        Vec3 cam_up(0.0f, 1.0f, 0.0f);
        Mat4x4 view = OpenGP::lookAt(cam_pos, cam_look, cam_up);

        cylinderMesh.draw(model, view, projection);
    });
    window2.set_title("Cylinder");

    Mesh cubeMesh;

    /// Example rendering a mesh
    /// Call to compile shaders
    cubeMesh.init();
    std::vector<Vec3> vertListCube;
    std::vector<unsigned int> indexListCube;
    std::vector<Vec2> textListCube;
    std::vector<Vec3> faceTextCube;
    std::vector<Vec3> normListCube;
    read("/Users/nboulton/Desktop/icg/data/cube.obj", vertListCube, indexListCube, textListCube, faceTextCube, normListCube);

    cubeMesh.loadVertices(vertListCube, indexListCube);

    auto &window1 = app.create_window([&](Window &window){
        int width, height;
        std::tie(width, height) = window.get_size();

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glClearColor(0.0f, 0.0f, 0.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /// Wireframe rendering, might be helpful when debugging your mesh generation
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

        float ratio = width / (float) height;
        Mat4x4 modelTransform = Mat4x4::Identity();
        Mat4x4 model = modelTransform.matrix();
        Mat4x4 projection = OpenGP::perspective(70.0f, ratio, 0.1f, 10.0f);

        //camera movement
        float time = .5f * (float)glfwGetTime();
        Vec3 cam_pos(2*cos(time), 2.0, 2*sin(time));
        Vec3 cam_look(0.0f, 0.0f, 0.0f);
        Vec3 cam_up(0.0f, 1.0f, 0.0f);
        Mat4x4 view = OpenGP::lookAt(cam_pos, cam_look, cam_up);

        cubeMesh.draw(model, view, projection);
    });
    window1.set_title("Cube");

    return app.run();
}

bool read(string filename, std::vector<Vec3> &vertList, std::vector<unsigned int> &indexList, std::vector<Vec2> &textList, std::vector<Vec3> &faceText, std::vector<Vec3> &normList){
    //Open the given file
    ifstream infile;
    infile.open(filename, ios::in);
    if(!infile.is_open()){
        //Cannot open file
        return false;
    }

    //Read through the file line by line
    for(string line; getline(infile, line ); ){
        string prefix = line.substr(0,2);
        if(prefix.compare("v ") == 0){
            //Line specifies a vertex
            line = line.substr(2,line.length());
            stringstream ss(line);
            float data[3];
            for(int i=0;i<3;i++){
              ss >> data[i];
            }
            vertList.push_back(Vec3(data[0], data[1], data[2]));
        } else if(prefix.compare("vn") == 0){
            //Line specifies a normal
            line = line.substr(3,line.length());
            stringstream ss(line);
            float data[3];
            for(int i=0;i<3;i++){
                ss >> data[i];
            }
            normList.push_back(Vec3(data[0], data[1], data[2]));
        } else if(prefix.compare("vt") == 0){
            //Line specifies a texture
            line = line.substr(3,line.length());
            stringstream ss(line);
            float data[2];
            for(int i=0;i<2;i++){
                ss >> data[i];
            }
            textList.push_back(Vec2(data[0], data[1]));
        } else if(prefix.compare("f ") == 0){
            //Line specifies a face
            line = line.substr(2,line.length());
            stringstream ss(line);
            string data[3];
            for(int i=0;i<3;i++){
                ss >> data[i];
            }
            //now split data 1-3 into the vertex, normal and texture
            string token;
            stringstream ss2(data[0]);
            int j = 0;
            while(std::getline(ss2, token, '/')) {
                if(j == 0){
                    //vertex
                    indexList.push_back(atoi(&token[0]) - 1.0f); // Face 1
                } else if(j ==1){
                    //texture
                } else{
                    //normal
                }
                j++;
            }
            stringstream ss3(data[1]);
            j = 0;
            while(std::getline(ss3, token, '/')) {
                if(j == 0){
                    //vertex
                    indexList.push_back(atoi(&token[0]) - 1.0f); // Face 1
                } else if(j ==1){
                    //texture
                } else{
                    //normal
                }
                j++;
            }
            stringstream ss4(data[2]);
            j = 0;
            while(std::getline(ss4, token, '/')) {
                if(j == 0){
                    //vertex
                    indexList.push_back(atoi(&token[0]) - 1.0f); // Face 1
                } else if(j ==1){
                    //texture
                } else{
                    //normal
                }
                j++;
            }
        }
    }

    return true;
}

bool WriteObj(string filename, string shape, std::vector<Vec3> &vertList, std::vector<Vec3> &faceList, std::vector<Vec2> &textList, std::vector<Vec3> &faceText, std::vector<Vec3> &normList){
    //Open the output file
    ofstream outfile;
    outfile.open(filename, ios::out);
    if(!outfile.is_open()){
        //Unable to open file
        return false;
    }

    //Write a simple file header
    outfile << "####" << endl;
    outfile << "#" << endl;
    outfile << "# OBJ File Generated by Noah Boulton" << endl;
    outfile << "#" << endl;
    outfile << "####" << endl;
    outfile << "# Object " << shape << ".obj" << endl;
    outfile << "#" << endl;
    outfile << "####" << endl;

    //Write out the list of verticies
    for(int i = 0; i < vertList.size(); i++){
        outfile << "v " << vertList[i](0)<< " " << vertList[i](1)<< " " << vertList[i](2) << endl;
    }

    //Write out the list of normals
    for(int i = 0; i < normList.size(); i++){
        outfile << "vn " << normList[i](0) << " " << normList[i](1) << " " << normList[i](2)<< endl;
    }

    //Write out the list of textures
    for(int i = 0; i < textList.size(); i++){
        outfile << "vt " << textList[i](0)<< " " << textList[i](1)<< endl;
    }
    //Write out the list of faces
    if(normList.size() >= faceList.size()){
        //With normals
        for(int i = 0; i < faceList.size(); i++){
            outfile << "f " << faceList[i](0) << "/" << faceText[i](0) << "/" << i << " " << faceList[i](1) << "/" << faceText[i](1) << "/" << i << " " << faceList[i](2) << "/" << faceText[i](2) << "/" << i << endl;
        }
    }else {
        //Without normals
        for(int i = 0; i < faceList.size(); i++){
            outfile << "f " << faceList[i](0) << "/" << faceText[i](0) << " " << faceList[i](1) << "/" << faceText[i](1) << " " << faceList[i](2) << "/" << faceText[i](2) << endl;
        }
    }

    outfile << "# End of File\n" << endl;
    void close();
    return true;
}

/* Generates a .obj file for a cube
 */
void cube(){
    ofstream outfile;
    outfile.open("/Users/nboulton/Desktop/icg/data/cube.obj", ios::out);

    outfile << "####" << endl;
    outfile << "#" << endl;
    outfile << "# OBJ File Generated by Noah Boulton" << endl;
    outfile << "#" << endl;
    outfile << "####" << endl;
    outfile << "# Object cube.obj" << endl;
    outfile << "#" << endl;
    outfile << "# Vertices: 9" << endl;
    outfile << "# Faces: 12" << endl;
    outfile << "#" << endl;
    outfile << "####" << endl;

    outfile << "v 1.0 1.0 1.0" << endl;
    outfile << "v 1.0 -1.0 1.0" << endl;
    outfile << "v -1.0 -1.0 1.0" << endl;
    outfile << "v -1.0 1.0 1.0" << endl;
    outfile << "v -1.0 -1.0 -1.0" << endl;
    outfile << "v -1.0 1.0 -1.0" << endl;
    outfile << "v 1.0 1.0 -1.0" << endl;
    outfile << "v 1.0 -1.0 -1.0" << endl;

    outfile << "vt 0.0 0.0" << endl;
    outfile << "vt 0.0 1.0" << endl;
    outfile << "vt 1.0 0.0" << endl;
    outfile << "vt 1.0 1.0" << endl;

    outfile << "vn 0.0 0.0 1.0" << endl;
    outfile << "vn 0.0 1.0 0.0" << endl;
    outfile << "vn 1.0 0.0 0.0" << endl;
    outfile << "vn 0.0 0.0 -1.0" << endl;
    outfile << "vn 0.0 -1.0 0.0" << endl;
    outfile << "vn -1.0 0.0 0.0" << endl;
    outfile << "# 9 vertices, 6 vertices normals\n" << endl;

    outfile << "f 3/1/1 2/3/1 1/4/1" << endl;
    outfile << "f 4/2/1 3/1/1 1/4/1" << endl;
    outfile << "f 6/2/2 4/1/2 1/3/2" << endl;
    outfile << "f 1/3/2 7/4/2 6/2/2" << endl;
    outfile << "f 7/4/3 1/2/3 2/1/3" << endl;
    outfile << "f 7/4/3 2/1/3 8/3/3" << endl;
    outfile << "f 6/1/4 7/3/4 8/4/4" << endl;
    outfile << "f 8/4/4 5/2/4 6/1/4" << endl;
    outfile << "f 8/4/5 2/2/5 3/1/5" << endl;
    outfile << "f 5/3/5 8/4/5 3/1/5" << endl;
    outfile << "f 3/2/6 4/1/6 5/4/6" << endl;
    outfile << "f 5/4/6 4/1/6 6/3/6" << endl;
    outfile << "# 6 faces, 4 coords texture\n" << endl;

    outfile << "# End of File\n" << endl;
    void close();
}

/**
 * @brief cylinder - generates a triangle mesh for a cylinder
 * @param n - the number of divisions along the top and bottom of the cylinder
 */
void cylinder(int n){
    float radius = 1.0f;
    float height = 2.0f;
    float azimuth = 0.0f;
    float increase = (2.0f*M_PI)/(float)n;

    float uStep = 1.0f/(float)n;
    float u = 0.0f;

    //Setup vector lists
    std::vector<Vec3> vertList;
    std::vector<Vec2> textList;
    std::vector<Vec3> normList;
    std::vector<Vec3> faceList;
    std::vector<Vec3> faceText;

    //Generate vertex and texture coordinates for the sides of the cylinder
    for(int i = 0; i < n; i++){
        float z = cos(azimuth)*1.0f*radius;
        float x = sin(azimuth)*1.0f*radius;

        vertList.push_back(Vec3(x,1,z));
        vertList.push_back(Vec3(x,-1,z));

        azimuth += increase;

        textList.push_back(Vec2(u,0.5));
        textList.push_back(Vec2(u,0));

        u = u + uStep;
    }
    textList.push_back(Vec2(u,0.5));
    textList.push_back(Vec2(u,0));

    //Caps
    vertList.push_back(Vec3(0,1,0));
    vertList.push_back(Vec3(0,-1,0));

    //Caps
    textList.push_back(Vec2(0.75, 0.75));
    textList.push_back(Vec2(0.25, 0.75));

    float angle = 0.0f;
    u = 0.75f;
    float v = 0.75f;
    float rad = 0.25f;

    //Generate texture coordinates for the top and bottom of the cylinder
    for(int i = 0; i < n; i++){
        float x = cos(angle)*rad + u;
        float y = sin(angle)*rad + v;

        textList.push_back(Vec2(x, y));
        textList.push_back(Vec2(x-0.5f, y));
        angle += increase;
    }

    //Generate the triangles that make up the sides
    for(int i = 1; i < 2*n-1; i++){
        if(i%2 == 1){
            faceList.push_back(Vec3(i+2, i, i+1));
            faceText.push_back(Vec3(i+2, i, i+1));
        } else{
            faceList.push_back(Vec3(i+2, i+1, i));
            faceText.push_back(Vec3(i+2, i+1, i));
        }
    }
    faceList.push_back(Vec3(2*n-1, 2*n, 2));
    faceText.push_back(Vec3(2*n-1, 2*n, 2*n+2));
    faceList.push_back(Vec3(1, 2*n-1, 2));
    faceText.push_back(Vec3(2*n+1, 2*n-1, 2*n+2));

    //Generate the triangles that make up the top and bottom
    for(int i = 1; i <= 2*n-2; i++){
        //top
        if(i%2==1){
            faceList.push_back(Vec3(2*n+1, i, i+2));
            faceText.push_back(Vec3(2*n+3, 2*n+4+i, 2*n+4+i+2));
        }
        //bottom
        else{
            faceList.push_back(Vec3(i, 2*n+2, i+2));
            faceText.push_back(Vec3(2*n+4+i, 2*n+4, 2*n+4+i+2));
        }
    }
    faceList.push_back(Vec3(2*n+1, 2*n-1, 1));
    faceText.push_back(Vec3(2*n+3, 4*n+3, 2*n+5 ));
    faceList.push_back(Vec3(2*n+2, 2, 2*n));
    faceText.push_back(Vec3(2*n+4, 2*n+6, 4*n+4));

    //Calculate the normals for each face
    //↑Nf = (↑B - ↑A) × (↑C - ↑A)
    for(int i = 0; i < faceList.size(); i++){
        Vec3 A = vertList[faceList[i](0)];
        Vec3 B = vertList[faceList[i](1)];
        Vec3 C = vertList[faceList[i](2)];
        Vec3 Normal = (B - A).cross(C - A);
        Normal = Normal.normalized();

        normList.push_back(Normal);
    }

    //Write the mesh to a .obj file
    WriteObj("/Users/nboulton/Desktop/icg/data/cylinder.obj", "cylinder", vertList, faceList, textList, faceText, normList);
}

/**
 * @brief sphere - generates a triangle mesh for a sphere
 * @param n - longitude divisions of the sphere
 * @param m - latidude divisions of the sphere
 */
void sphere(int n, int m){
    //Increase m to have the correct number of divisions
    m++;

    //Setup vector lists
    std::vector<Vec3> vertList;
    std::vector<Vec2> textList;
    std::vector<Vec3> normList;
    std::vector<Vec3> faceList;
    std::vector<Vec3> faceText;
    float radius = 1.0f;
    float theta = 0.0f;
    float phi = 0.0f;

    float tStep = (2.0f*M_PI)/(float)n;
    float pStep = (M_PI)/(float)m;

    float uStep = 1.0f/n;
    float vStep = 1.0f/m;
    float u = 1.0f;
    float v = 1.0f;

    //generate verticies
    vertList.push_back(Vec3(0.0f,1.0f,0.0f));
    for(int i = 0; i <= m; i++){
        for(int j = 0; j < n; j++){
            float x = radius*cos(theta)*sin(phi);
            float z = radius*sin(theta)*sin(phi);
            float y = radius*cos(phi);

            vertList.push_back(Vec3(x,y,z));
            theta += tStep;
        }
        phi += pStep;
    }

    //generate texture coordinates
    for(int i = 0; i <= m; i++){
        u = 1.0f;
        for(int j = 0; j <= n; j++){
            textList.push_back(Vec2(u, v));

            u -= uStep;
        }
        v -= vStep;
    }
    vertList.push_back(Vec3(0.0f,-1.0f,0.0f));

    //generate triangles for the top of the sphere
    for(int i = 1; i < n; i++){
        faceList.push_back(Vec3(i,n+i+2,n+i+1));
        faceText.push_back(Vec3(i,n+i+2,n+i+1));
    }
    faceList.push_back(Vec3(n,n+2,n+n+1));
    faceText.push_back(Vec3(n,n+n+2,n+n+1));

    //generate triangles for the middle of the sphere layer by layer
    int l = 0; //level offset for texture coordinates
    int k = 0;
    for(int i = 1; i < m-1; i++){
        for(int j = 1; j <= n; j++){
            k = n*i+j+1;
            if(j == n){
                faceList.push_back(Vec3(k, k+1-n, k+1));
                faceText.push_back(Vec3(k+l, k+1+l, k+n+l+1+1));
            }  else{
                faceList.push_back(Vec3(k, k+1, k+n+1));
                faceText.push_back(Vec3(k+l, k+1+l, k+n+1+l+1));
            }
            if(j == n){
                faceList.push_back(Vec3(k, k+1, k+n));
                faceText.push_back(Vec3(k+l, k+1+n+1+l, k+n+1+l));
            }else {
                faceList.push_back(Vec3(k, k+n+1, k+n));
                faceText.push_back(Vec3(k+l, k+n+1+l+1, k+n+l+1));
            }
        }
        l++;
    }

    //generate triangles for the bottom of the sphere
    for(int i = vertList.size(); i > vertList.size() - n+1; i--){
        faceList.push_back(Vec3(i,i-n-2,i-n-1));
        faceText.push_back(Vec3(i+l,i-n-2+l,i-n-1+l));
    }
    faceList.push_back(Vec3(vertList.size() - n+1, vertList.size() - n+1-2, vertList.size() - n+1-n-1));
    faceText.push_back(Vec3(vertList.size()+l+1, vertList.size() -n+1-2+l, vertList.size() -n+1-n-1+l+n));

    //Calculate the normals for each face
    //↑Nf = (↑B - ↑A) × (↑C - ↑A)
    for(int i = 0; i < faceList.size(); i++){
        Vec3 A = vertList[faceList[i](0)];
        Vec3 B = vertList[faceList[i](1)];
        Vec3 C = vertList[faceList[i](2)];
        Vec3 Normal = (B - A).cross(C - A);
        Normal = Normal.normalized();

        normList.push_back(Normal);
    }

    //Write the mesh to a .obj file
    WriteObj("/Users/nboulton/Desktop/icg/data/sphere.obj", "sphere", vertList, faceList, textList, faceText, normList);
}

/**
 * @brief torus - generates a triangle mesh for a torus
 * @param n - number of divisions around the minor radius
 * @param m - number of divisions around the major radius
 * @param r - minor radius
 */
void torus(int n, int m, float r){
    float R = 1.0f;
    float theta = 0.0f;
    float phi = 0.0f;

    //Setup vector lists
    std::vector<Vec3> vertList;
    std::vector<Vec3> faceList;
    std::vector<Vec2> textList;
    std::vector<Vec3> faceText;
    std::vector<Vec3> normList;

    float tStep = (2.0f*M_PI)/(float)n;
    float pStep = (2.0f*M_PI)/(float)m;

    //generate verticies
    for(int i = 0; i <= m; i++){
        float theta = 0.0f;
        for(int j = 0; j <= n; j++){
            float x = (R + r*cos(theta))*cos(phi);
            float z = (R + r*cos(theta))*sin(phi);
            float y = r*sin(theta);
            if(i != m && j != n){
                vertList.push_back(Vec3(x,y,z));
            }
            theta += tStep;
        }
        phi += pStep;
    }

    float u = 0.0f;
    float v = 0.0f;
    float uStep = 1.0f/(float)m;
    float vStep = 1.0f/(float)n;

    //Generate texture coordinates
    for(int i = 0; i <= m; i++){
        v = 0.0f;
        for(int j = 0; j <= n; j++){
            textList.push_back(Vec2(u, v));
            v += vStep;
        }
        u += uStep;
    }

    //Generate faces working around each minor circle
    int j = 1;
    for(int i = 0; i < m; i++){
        for(j = 1; j <= n; j++){

            if(i == m-1){
                if(j == n){
                    faceList.push_back(Vec3(j+i*n, j+i*n-n+1, j+1-n));
                    faceText.push_back(Vec3(j+i*n+i, j+i*n+1+i, j+i*n+n+1+i+1));

                    faceList.push_back(Vec3(j+i*n, 1, j));
                    faceText.push_back(Vec3(j+i*n+i, j+i*n+n+i+1+1, j+i*n+n+i+1));
                    continue;
                }
                faceList.push_back(Vec3(j+i*n, j+i*n+1, j+1));
                faceText.push_back(Vec3(j+i*n+i, j+i*n+1+i, j+i*n+n+1+i+1));

                faceList.push_back(Vec3(j+i*n, j+1, j));
                faceText.push_back(Vec3(j+i*n+i, j+i*n+n+1+i+1, j+i*n+n+i+1));
                continue;
            }

            if(j == n){
                faceList.push_back(Vec3(j+i*n, j+i*n-n+1, j+i*n+1));
                faceText.push_back(Vec3(j+i*n+i, j+i*n+1+i, j+i*n+n+1+i+1));

                faceList.push_back(Vec3(j+i*n, j+i*n+1, j+i*n+n));
                faceText.push_back(Vec3(j+i*n+i, j+i*n+n+i+1+1, j+i*n+n+i+1));
                continue;
            }
            faceList.push_back(Vec3(j+i*n, j+i*n+1, j+i*n+n+1));
            faceText.push_back(Vec3(j+i*n+i, j+i*n+1+i, j+i*n+n+1+i+1));

            faceList.push_back(Vec3(j+i*n, j+i*n+n+1, j+i*n+n));
            faceText.push_back(Vec3(j+i*n+i, j+i*n+n+1+i+1, j+i*n+n+i+1));

        }
    }

    //Calculate the normals for each face
    //↑Nf = (↑B - ↑A) × (↑C - ↑A)
    for(int i = 0; i < faceList.size(); i++){
        Vec3 A = vertList[faceList[i](0)];
        Vec3 B = vertList[faceList[i](1)];
        Vec3 C = vertList[faceList[i](2)];
        Vec3 Normal = (B - A).cross(C - A);
        Normal = Normal.normalized();

        normList.push_back(Normal);
    }

    //Write the mesh to a .obj file
    WriteObj("/Users/nboulton/Desktop/icg/data/torus.obj", "torus", vertList, faceList, textList, faceText, normList);
}
