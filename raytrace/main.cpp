#include "OpenGP/Image/Image.h"
#include "bmpwrite.h"
#include "shape.h"
#include "sphere.h"
#include "plane.h"

using namespace OpenGP;
using Colour = Vec3; // RGB Value
Colour red() { return Colour(1.0f, 0.0f, 0.0f); }
Colour pink() { return Colour(1.0f, 0.71f, 0.757f); }
Colour white() { return Colour(1.0f, 1.0f, 1.0f); }
Colour black() { return Colour(0.0f, 0.0f, 0.0f); }
Colour grey() { return Colour(0.5f, 0.5f, 0.5f); }
Colour blue() { return Colour(0.0f, 0.0f, 1.0f); }
Colour silver() { return Colour(0.753f, 0.753f, 0.753f); }

int ray_count = 0;

//Colour trace(Vec3 &ray, sphere sheres[], int numSpheres, plane planes[], int numPlanes, Vec3 &origin, Vec3 lightPos[], float lightInt, int depth, float ambient, int numLights);
bool isShadow(shape * shapes[], int numShapes, Vec3 &origin, Vec3 &lightPos, int index);
void setup(sphere spheres[], int numSpheres, plane planes[], int numPlanes);
Colour trace(Vec3 &ray, shape * shapes[], int numShapes, Vec3 &origin, Vec3 lightPos[], float lightInt, int depth, float ambient, int numLights);

int main(int, char**){
    int wResolution = 720;
    int hResolution = 480;
    float aspectRatio = float(wResolution) / float(hResolution); // Corrects for the screen size
    Image<Colour> image(hResolution, wResolution);

    /// TODO: define camera position
    Vec3 W = Vec3(0.0f, 0.0f, -1.0f); //view vector
    Vec3 V = Vec3(0.0f, 1.0f, 0.0f); //up vector
    Vec3 U = Vec3(1.0f, 0.0f, 0.0f); //cross product of v and w
    float d = 1;
    Vec3 camera = -d*W; // -d because the origin is ahead of the camera

    float left = -1.0f*aspectRatio;
    float right = 1.0f*aspectRatio;
    float bottom = -1.0f;
    float top = 1.0f;

    // Define arrays for spheres and planes
    int numSpheres = 3;
    sphere spheres[numSpheres];
    int numPlanes = 6;
    plane planes[numPlanes];

    //Sutup spheres and planes
    setup(spheres, numSpheres, planes, numPlanes);

    int numShapes = numSpheres + numPlanes;
    shape * shapes[numShapes];
    for(int i = 0; i < numShapes; i++){
        if(i >= numSpheres){
            shapes[i] = &planes[i - numSpheres];
        } else {
            shapes[i] = &spheres[i];
        }
    }

    //define light posistion
//    int numLights = 3;
//    Vec3 lightPos[3] = {Vec3(2.0f, 9.0f, 2.0f), Vec3(-2.0f, 9.0f, 2.0f), Vec3(0.0f, 9.0f, 2.0f)};
    int numLights = 1;
    Vec3 lightPos[1] = {Vec3(2.0f, 9.0f, 2.0f)};
    float lightInt = 1.0f; // Change this to dim the scene
    float ambient = 0.3f; // ambient light intensity

    for (int row = 0; row < image.rows(); ++row) {
        for (int col = 0; col < image.cols(); ++col) {
            /// TODO: build primary rays
            Vec3 pixel = left*U + (col*(right-left)/image.cols())*U; // horizontal coordinate of the pixel
            pixel += bottom*V + (row*(top-bottom)/image.rows())*V; // vertical coordinate of the pixel

            Vec3 ray = pixel - camera;
            ray = ray.normalized();
            // Colour the pixel based on the return of trace
            image(row, col) = trace(ray, shapes, numShapes, camera, lightPos, lightInt, 1, ambient, numLights);
        }
    }
    std::cout << "Rays Traced: " << ray_count << std::endl;
    bmpwrite("../../out.bmp", image);
    imshow(image);
    return EXIT_SUCCESS;
}

Colour trace(Vec3 &ray, shape * shapes[], int numShapes, Vec3 &origin, Vec3 lightPos[], float lightInt, int depth, float ambient, int numLights){
    ray_count++;
    if(depth > 2){  // Limit the depth to avoid non-terminating recursive calls
        return black();
    }

    Colour colour = black();
    float time = INFINITY;
    int index = -1;
    for(int i = 0; i < numShapes; i++){
        float t = INFINITY;
        //ray-shape intersection
        t = shapes[i]->intersect(origin, ray);
        if(t < time && t > 0){
            time = t;
            index = i;
        }
    }

    if(time == INFINITY){
        //hit nothing
        return black();
    }
    Colour ambientColour = black();
    Colour diffuseColour = black();
    Colour specularColour = black();

    Vec3 hitPos = origin + ray*time; // posistion on the object that was hit

    Vec3 normal = shapes[index]->get_normal(hitPos);;
    Colour shade = shapes[index]->get_colour();;
    Colour spec = shapes[index]->get_spec();;
    float specCof = shapes[index]->get_specCof();;
    float reflective = shapes[index]->get_reflective();
    Vec3 ambientCol = shapes[index]->get_ambientCol();

    //ambient
    ambientColour = ambient*ambientCol*lightInt;

    if(shapes[index]->get_mirror()){ // Mirror reflection
        Vec3 R = ray - 2.0f*(ray.dot(normal)*normal);
        R = R.normalized();
        shade += trace(R, shapes, numShapes, hitPos, lightPos, lightInt, depth+1, ambient, numLights);
    }

    // check if the point is in shadow
    int shadowCount = 0;
    for(int i = 0; i < numLights; i++){
        if(isShadow(shapes, numShapes, hitPos, lightPos[i], index)){
            shadowCount++;
        }
    }

    if(shadowCount>0){
        // point is in shadow of at least one light soruce
        return ambientColour + ambientColour*(numLights - shadowCount);
    }

    for(int i = 0; i < numLights; i++){
        Vec3 lightDir = lightPos[i] - hitPos;
        lightDir = lightDir.normalized();

        //Diffuse
        float diffuse = std::fmaxf(normal.dot(lightDir), 0.0f);
        diffuseColour += 0.5f*diffuse*lightInt*shade;

        Vec3 V = origin - hitPos;
        V = V.normalized();
        Vec3 H = V + lightDir;
        H = H.normalized();
        float specular = H.dot(normal);

        if(diffuse > 0.0f){ // Specular is added only if the diffuse is greater than 0
            specularColour += 0.5f*std::powf((std::max(0.0f, specular)), specCof)*spec*lightInt;
        }
    }

//            if(spheres[index].get_glass()){ // Glass refraction
//                Vec3 norm = normal;
//                Vec3 incident = ray;
//                float angle1 = (-1.0f*norm.dot(incident));
//                if(angle1 < 0){ // make sure the angle is positive
//                    norm = -1.0f*norm;
//                    angle1 = (norm.dot(incident));
//                }

//                float angle2 = std::sqrt(1.0f - std::powf((1.0f/1.5f), 2.0f)*(1.0f - std::powf(((angle1)), 2)));
//                Vec3 refract = (1.0f/1.5f)*incident + ((1.0f/1.5f)*angle1 - angle2)*norm;
//                refract = refract.normalized();

//                hitPos = hitPos + refract*0.00001f; // move the hit posistion forward to avoid hitting the same location on the object
//                Vec3 hitPosSubC = hitPos - spheres[index].get_center();
//                float disc = std::powf(refract.dot(hitPosSubC), 2) - hitPosSubC.dot(hitPosSubC) + spheres[index].get_radius()*spheres[index].get_radius();
//                if(disc < 0){
//                    return black();
//                }

//                float t = -(refract.dot(hitPosSubC)) - std::sqrtf(disc);
//                Vec3 hitPos2 = hitPos + refract*t;
//                norm = (spheres[index].get_center() - hitPos2)/spheres[index].get_radius();

//                angle1 = (norm.dot(refract));
//                if(angle1 < 0){
//                    norm = -1.0f*norm;
//                    angle1 = (norm.dot(refract));
//                }

//                float crit = std::asin((1.0f/1.5f));
//                if(std::acos(angle1) >= crit){ // use the critical angle to handle total internal reflection
//                    return black();
//                }

//                // Calculate second refraction on the back surface of the sphere
//                angle2 = std::sqrt(1.0f - std::powf((1.5f/1.0f), 2.0f)*(1.0f - std::powf((angle1), 2)));
//                Vec3 refract2 = (1.5f/1.0f)*refract + ((1.5f/1.0f)*angle1 - angle2)*norm;
//                refract2 = refract2.normalized();

//                //colour the pixel the colour of the refracted ray hit
//                shade += trace(refract2, spheres, numSpheres, planes, numPlanes, hitPos, lightPos, lightInt, depth+1, ambient, numLights);
//            }

    if(reflective == 0.0f){
        return ambientColour + diffuseColour + specularColour;
    }


    Vec3 R = ray - 2.0f*(ray.dot(normal)*normal);
    R = R.normalized();
    //Add the reflected colour to reflective surfaces

    return ambientColour + diffuseColour + specularColour;
}

/**
 * @brief isShadow - checks if the point in the scene is in shadow for the given light
 * @param spheres - sphere array to check intersection
 * @param numSpheres - number of spheres in the scene
 * @param planes - plane array to check intersection
 * @param numPlanes - number of planes in the scenee
 * @param origin - the initial point of the ray
 * @param lightPos - array of lights
 * @param index - index of the object the given point is on
 * @return - boolean value for if the point is in shadow
 */
bool isShadow(shape * shapes[], int numShapes, Vec3 &origin, Vec3 &lightPos, int index){
    ray_count++;
    Vec3 lightDir = lightPos - origin;
    lightDir = lightDir.normalized();
    float time0 = (lightPos - origin)(0)/lightDir(0);
    float time1 = (lightPos - origin)(1)/lightDir(1);
    float time2 = (lightPos - origin)(2)/lightDir(2);

    float time = std::fmaxf(time0, time1);
    time = std::fmaxf(time, time2);
    for(int i = 0; i < numShapes; i++){
        float t = shapes[i]->intersect(origin, lightDir);
        if(t > 0.00001f && t < time){
            return true;
        }
    }
    return false;
}


/**
 * @brief trace
 * @param ray - the direction ray to be traced
 * @param spheres - sphere array to check intersection
 * @param numSpheres - number of spheres in the scene
 * @param planes - plane array to check intersection
 * @param numPlanes - number of planes in the scenee
 * @param origin - the initial point of the ray
 * @param lightPos - array of lights
 * @param lightInt - light intensity of the scene
 * @param depth - depth of the trace call, used to terminiate recursive calls
 * @param ambient - ambient light coefficient for the scene
 * @param numLights - number of point light sources in the scene
 * @return - the colour of the ray
 */
//Colour trace(Vec3 &ray, sphere spheres[], int numSpheres, plane planes[], int numPlanes, Vec3 &origin, Vec3 lightPos[], float lightInt, int depth, float ambient, int numLights){
//    ray_count++;
//    if(depth > 1){  // Limit the depth to avoid non-terminating recursive calls
//        return black();
//    }

//    Colour colour = Vec3(0.0f, 0.0f, 0.0f);
//    float time = INFINITY;
//    int index = -1;
//    for(int i = 0; i < numSpheres; i++){
//        float t = INFINITY;
//        //ray-sphere intersection
//        t = spheres[i].intersect(origin, ray);
//        if(t < time && t > 0){
//            time = t;
//            index = i;
//        }
//    }

//    // Ray plane intersection
//    for(int i = 0; i < numPlanes; i++){
//        float t = planes[i].intersect(origin, ray);
//        if(t > 0.0001f){
//            // intersects in front of camera
//            if(t < time){
//                time = t;
//                index = numSpheres + i;
//            }
//        }
//    }

//    if(time == INFINITY){
//        //hit nothing
//        return black();
//    }

//    Vec3 hitPos = origin + ray*time; // posistion on the object that was hit

//    // check if the point is in shadow
//    int shadowCount = 0;
//    for(int i = 0; i < numLights; i++){
//        if(isShadow(spheres, numSpheres, planes, numPlanes, hitPos, lightPos[i], index)){
//            shadowCount++;
//        }
//    }

//    Vec3 normal;
//    Colour shade;
//    Colour spec;
//    float specCof;
//    float reflective;

//    if(index < numSpheres){
//        //ambient
//        shade = spheres[index].get_colour();
//        Vec3 ambientCol = spheres[index].get_ambientCol();
//        colour = ambient*ambientCol*lightInt;

//        if(shadowCount>0){
//            // point is in shadow of at least one light soruce
//            return colour + colour*(numLights - shadowCount);
//        }

//        normal = (hitPos - spheres[index].get_center())/spheres[index].get_radius();
//        spec = spheres[index].get_spec();
//        specCof = spheres[index].get_specCof();
//        reflective = spheres[index].get_reflective();

//        if(spheres[index].get_mirror()){ // Mirror reflection
//            Vec3 R = ray - 2.0f*(ray.dot(normal)*normal);
//            R = R.normalized();
//            shade += trace(R, spheres, numSpheres, planes, numPlanes, hitPos, lightPos, lightInt, depth+1, ambient, numLights);
//        }

//        if(spheres[index].get_glass()){ // Glass refraction
//            Vec3 norm = normal;
//            Vec3 incident = ray;
//            float angle1 = (-1.0f*norm.dot(incident));
//            if(angle1 < 0){ // make sure the angle is positive
//                norm = -1.0f*norm;
//                angle1 = (norm.dot(incident));
//            }

//            float angle2 = std::sqrt(1.0f - std::powf((1.0f/1.5f), 2.0f)*(1.0f - std::powf(((angle1)), 2)));
//            Vec3 refract = (1.0f/1.5f)*incident + ((1.0f/1.5f)*angle1 - angle2)*norm;
//            refract = refract.normalized();

//            hitPos = hitPos + refract*0.00001f; // move the hit posistion forward to avoid hitting the same location on the object
//            Vec3 hitPosSubC = hitPos - spheres[index].get_center();
//            float disc = std::powf(refract.dot(hitPosSubC), 2) - hitPosSubC.dot(hitPosSubC) + spheres[index].get_radius()*spheres[index].get_radius();
//            if(disc < 0){
//                return black();
//            }

//            float t = -(refract.dot(hitPosSubC)) - std::sqrtf(disc);
//            Vec3 hitPos2 = hitPos + refract*t;
//            norm = (spheres[index].get_center() - hitPos2)/spheres[index].get_radius();

//            angle1 = (norm.dot(refract));
//            if(angle1 < 0){
//                norm = -1.0f*norm;
//                angle1 = (norm.dot(refract));
//            }

//            float crit = std::asin((1.0f/1.5f));
//            if(std::acos(angle1) >= crit){ // use the critical angle to handle total internal reflection
//                return black();
//            }

//            // Calculate second refraction on the back surface of the sphere
//            angle2 = std::sqrt(1.0f - std::powf((1.5f/1.0f), 2.0f)*(1.0f - std::powf((angle1), 2)));
//            Vec3 refract2 = (1.5f/1.0f)*refract + ((1.5f/1.0f)*angle1 - angle2)*norm;
//            refract2 = refract2.normalized();

//            //colour the pixel the colour of the refracted ray hit
//            shade += trace(refract2, spheres, numSpheres, planes, numPlanes, hitPos, lightPos, lightInt, depth+1, ambient, numLights);
//        }

//    } else if(index >= numSpheres){
//        //ambient
//        Vec3 ambientCol = planes[index - numSpheres].get_ambientCol();
//        shade = planes[index - numSpheres].get_colour();
//        colour = ambient*ambientCol*lightInt;

//        if(shadowCount>0){
//            // point is in shadow of at least one light source
//            return colour + colour*(numLights - shadowCount);
//        }

//        normal = planes[0].get_normal();
//        spec = planes[index + numSpheres].get_spec();
//        specCof = planes[index - numSpheres].get_specCof();
//        reflective = planes[index -numSpheres].get_reflective();
//    }

//    for(int i = 0; i < numLights; i++){
//        Vec3 lightDir = lightPos[i] - hitPos;
//        lightDir = lightDir.normalized();

//        //Diffuse
//        float diffuse = std::fmaxf(normal.dot(lightDir), 0.0f);
//        colour += 0.5f*diffuse*lightInt*shade;

//        Vec3 V = origin - hitPos;
//        V = V.normalized();
//        Vec3 H = V + lightDir;
//        H = H.normalized();
//        float specular = H.dot(normal);

//        if(diffuse > 0.0f){ // Specular is added only if the diffuse is greater than 0
//                colour += 0.5f*std::powf((std::max(0.0f, specular)), specCof)*spec*lightInt;
//        }
//    }

//    if(reflective == 0.0f){
//        return colour;
//    }

//    Vec3 R = ray - 2.0f*(ray.dot(normal)*normal);
//    R = R.normalized();
//    //Add the reflected colour to reflective surfaces
//    colour += reflective*trace(R, spheres, numSpheres, planes, numPlanes, hitPos, lightPos, lightInt, depth+1, ambient, numLights);
//    return colour;
//}

/**
 * @brief setup - initializes the objects in the scene
 * @param spheres - sphere array to hold the sphere structs
 * @param numSpheres - number of spheres in the scene
 * @param planes - plane array to hold the plane structs
 * @param numPlanes - number of planes in the scene
 */
void setup(sphere spheres[], int numSpheres, plane planes[], int numPlanes){
    sphere s = sphere(2.0f, false, false, Vec3(0.0f, 1.0f, -10.0f), pink(), grey(), pink(), 570.0f, 0.0f);

    sphere s2 = sphere(2.0f, false, true, Vec3(4.0f, 1.0f, -5.0f), black(), white(), grey(), 600.0f, 0.025f);

    sphere s3 = sphere(2.0f, true, false, Vec3(-4.0f, 1.0f, -5.0f), Colour(0.1f, 0.1f, 0.1f), silver(), Colour(0.1f, 0.1f, 0.1f), 1000.0f, 0.0f);

    // biuld the floor as a plane parallel to the camera view
    Vec3 floorNorm = Vec3(0.0f, 1.0f, 0.0f);
    floorNorm = floorNorm.normalized();
    Vec3 floorCenter = Vec3(0.0f, -4.0f, -6.0f);
    plane floor = plane(floorCenter, floorNorm, grey(), grey(), grey(), 380.0f, 0.1f, false);

    Vec3 backNorm = Vec3(0.0f, 0.0f, 1.0f);
    backNorm = backNorm.normalized();
    Vec3 backCenter = Vec3(0.0f, 0.0f, -20.0f);

    plane back = plane(backCenter, backNorm, pink(), grey(), pink(), 1000.0f, 0.1f, false);

    Vec3 behindNorm = Vec3(0.0f, 0.0f, -1.0f);
    behindNorm = behindNorm.normalized();
    Vec3 behindCenter = Vec3(0.0f, 0.0f, 6.0f);

    plane behind = plane(behindCenter, behindNorm, pink(), grey(), pink(), 1000.0f, 0.1f, false);

    Vec3 ceilingNorm = Vec3(0.0f, -1.0f, 0.0f);
    ceilingNorm = ceilingNorm.normalized();
    Vec3 ceilingCenter = Vec3(0.0f, 10.0f, 0.0f);

    plane ceiling = plane(ceilingCenter, ceilingNorm, grey(), grey(), grey(), 1000.0f, 0.0f, false);

    Vec3 leftWallNorm = Vec3(1.0f, 0.0f, 0.0f);
    leftWallNorm = leftWallNorm.normalized();
    Vec3 leftWallCenter = Vec3(-10.0f, 0.0f, 0.0f);

    plane leftWall = plane(leftWallCenter, leftWallNorm, red(), grey(), red(), 1000.0f, 0.1f, false);

    Vec3 rightWallNorm = Vec3(-1.0f, 0.0f, 0.0f);
    rightWallNorm = rightWallNorm.normalized();
    Vec3 rightWallCenter = Vec3(10.0f, 0.0f, 0.0f);

    plane rightWall = plane(rightWallCenter, rightWallNorm, blue(), grey(), blue(), 1000.0f, 0.1f, false);

    spheres[0] = s;
    spheres[1] = s2;
    spheres[2] = s3;

    planes[0] = floor;
    planes[1] = back;
    planes[2] = behind;
    planes[3] = ceiling;
    planes[4] = leftWall;
    planes[5] = rightWall;
}
