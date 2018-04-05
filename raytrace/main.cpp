#include "OpenGP/Image/Image.h"
#include "bmpwrite.h"

using namespace OpenGP;
using Colour = Vec3; // RGB Value
Colour red() { return Colour(1.0f, 0.0f, 0.0f); }
Colour pink() { return Colour(1.0f, 0.71f, 0.757f); }
Colour white() { return Colour(1.0f, 1.0f, 1.0f); }
Colour black() { return Colour(0.0f, 0.0f, 0.0f); }
Colour grey() { return Colour(0.5f, 0.5f, 0.5f); }
Colour blue() { return Colour(0.0f, 0.0f, 1.0f); }
Colour silver() { return Colour(0.753f, 0.753f, 0.753f); }

struct sphere{
    Vec3 center;
    float radius;
    Colour colour;
    bool mirror;
    bool glass;
    Colour spec;
    Vec3 ambientCol;
    float specCof;
    float reflective;
};

struct plane{
    Vec3 center;
    Vec3 normal;
    Colour colour;
    Colour spec;
    Vec3 ambientCol;
    float specCof;
    float reflective;
};

Colour trace(Vec3 &ray, sphere sheres[], int numSpheres, plane planes[], int numPlanes, Vec3 &origin, Vec3 lightPos[], float lightInt, int depth, float ambient, int numLights);
bool isShadow(sphere spheres[], int numSpheres, plane planes[], int numPlanes, Vec3 &origin, Vec3 &lightPos, int index);
void setup(sphere spheres[], int numSpheres, plane planes[], int numPlanes);

int main(int, char**){
    int wResolution = 640;
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

    //define light posistion
    int numLights = 3;
    Vec3 lightPos[3] = {Vec3(2.0f, 9.0f, 2.0f), Vec3(-2.0f, 9.0f, 2.0f), Vec3(0.0f, 9.0f, 2.0f)};
    float lightInt = 0.8f; // Change this to dim the scene
    float ambient = 0.3f; // ambient light intensity

    for (int row = 0; row < image.rows(); ++row) {
        for (int col = 0; col < image.cols(); ++col) {
            /// TODO: build primary rays
            Vec3 pixel = left*U + (col*(right-left)/image.cols())*U; // horizontal coordinate of the pixel
            pixel += bottom*V + (row*(top-bottom)/image.rows())*V; // vertical coordinate of the pixel

            Vec3 ray = pixel - camera;
            ray = ray.normalized();
            // Colour the pixel based on the return of trace
            image(row, col) = trace(ray, spheres, numSpheres, planes, numPlanes, camera, lightPos, lightInt, 1, ambient, numLights);
        }
    }
    bmpwrite("../../out.bmp", image);
    imshow(image);
    return EXIT_SUCCESS;
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
Colour trace(Vec3 &ray, sphere spheres[], int numSpheres, plane planes[], int numPlanes, Vec3 &origin, Vec3 lightPos[], float lightInt, int depth, float ambient, int numLights){
    if(depth > 5){  // Limit the depth to avoid non-terminating recursive calls
        return black();
    }

    Colour colour = Vec3(0.0f, 0.0f, 0.0f);
    float time = INFINITY;
    int index = -1;
    for(int i = 0; i < numSpheres; i++){
        float t = INFINITY;
        //ray-sphere intersection
        Vec3 originSubC = origin - spheres[i].center; // camera - sphere center
        // calculate the discriminet to see if the ray hits the sphere
        float disc = std::powf(ray.dot(originSubC), 2) - originSubC.dot(originSubC) + spheres[i].radius*spheres[i].radius;

        if(disc >= 0){
            // Hits the sphere
            t = -(ray.dot(originSubC)) - std::sqrtf(disc);
        }
        if(t < time && t > 0){
            time = t;
            index = i;
        }
    }

    // Ray plane intersection
    for(int i = 0; i < numPlanes; i++){
        float t = INFINITY;
        if((planes[i].center - origin).dot(planes[i].normal) != 0){
            t = ((planes[i].center - origin).dot(planes[i].normal))/(ray.dot(planes[i].normal));
            if(t > 0.0001f){
                // intersects in front of camera
                if(t < time){
                    time = t;
                    index = numSpheres + i;
                }
            }
        }
    }

    if(time == INFINITY){
        //hit nothing
        return black();
    }

    Vec3 hitPos = origin + ray*time; // posistion on the object that was hit

    // check if the point is in shadow
    int shadowCount = 0;
    for(int i = 0; i < numLights; i++){
        if(isShadow(spheres, numSpheres, planes, numPlanes, hitPos, lightPos[i], index)){
            shadowCount++;
        }
    }

    Vec3 normal;
    Colour shade;
    Colour spec;
    float specCof;
    float reflective;

    if(index < numSpheres){
        //ambient
        shade = spheres[index].colour;
        Vec3 ambientCol = spheres[index].ambientCol;
        colour = ambient*ambientCol*lightInt;

        if(shadowCount>0){
            // point is in shadow of at least one light soruce
            return colour + colour*(numLights - shadowCount);
        }

        normal = (hitPos - spheres[index].center)/spheres[index].radius;
        spec = spheres[index].spec;
        specCof = spheres[index].specCof;
        reflective = spheres[index].reflective;

        if(spheres[index].mirror){ // Mirror reflection
            Vec3 R = ray - 2.0f*(ray.dot(normal)*normal);
            R = R.normalized();
            shade += trace(R, spheres, numSpheres, planes, numPlanes, hitPos, lightPos, lightInt, depth+1, ambient, numLights);
        }

        if(spheres[index].glass){ // Glass refraction
            Vec3 norm = normal;
            Vec3 incident = ray;
            float angle1 = (-1.0f*norm.dot(incident));
            if(angle1 < 0){ // make sure the angle is positive
                norm = -1.0f*norm;
                angle1 = (norm.dot(incident));
            }

            float angle2 = std::sqrt(1.0f - std::powf((1.0f/1.5f), 2.0f)*(1.0f - std::powf(((angle1)), 2)));
            Vec3 refract = (1.0f/1.5f)*incident + ((1.0f/1.5f)*angle1 - angle2)*norm;
            refract = refract.normalized();

            hitPos = hitPos + refract*0.00001f; // move the hit posistion forward to avoid hitting the same location on the object
            Vec3 hitPosSubC = hitPos - spheres[index].center;
            float disc = std::powf(refract.dot(hitPosSubC), 2) - hitPosSubC.dot(hitPosSubC) + spheres[index].radius*spheres[index].radius;
            if(disc < 0){
                return black();
            }

            float t = -(refract.dot(hitPosSubC)) - std::sqrtf(disc);
            Vec3 hitPos2 = hitPos + refract*t;
            norm = (spheres[index].center - hitPos2)/spheres[index].radius;

            angle1 = (norm.dot(refract));
            if(angle1 < 0){
                norm = -1.0f*norm;
                angle1 = (norm.dot(refract));
            }

            float crit = std::asin((1.0f/1.5f));
            if(std::acos(angle1) >= crit){ // use the critical angle to handle total internal reflection
                return black();
            }

            // Calculate second refraction on the back surface of the sphere
            angle2 = std::sqrt(1.0f - std::powf((1.5f/1.0f), 2.0f)*(1.0f - std::powf((angle1), 2)));
            Vec3 refract2 = (1.5f/1.0f)*refract + ((1.5f/1.0f)*angle1 - angle2)*norm;
            refract2 = refract2.normalized();

            //colour the pixel the colour of the refracted ray hit
            shade += trace(refract2, spheres, numSpheres, planes, numPlanes, hitPos, lightPos, lightInt, depth+1, ambient, numLights);
        }

    } else if(index >= numSpheres){
        //ambient
        Vec3 ambientCol = planes[index - numSpheres].ambientCol;
        shade = planes[index - numSpheres].colour;
        colour = ambient*ambientCol*lightInt;

        if(shadowCount>0){
            // point is in shadow of at least one light source
            return colour + colour*(numLights - shadowCount);
        }

        normal = planes[0].normal;
        spec = planes[index + numSpheres].spec;
        specCof = planes[index - numSpheres].specCof;
        reflective = planes[index -numSpheres].reflective;
    }

    for(int i = 0; i < numLights; i++){
        Vec3 lightDir = lightPos[i] - hitPos;
        lightDir = lightDir.normalized();

        //Diffuse
        float diffuse = std::fmaxf(normal.dot(lightDir), 0.0f);
        colour += 0.5f*diffuse*lightInt*shade;

        Vec3 V = origin - hitPos;
        V = V.normalized();
        Vec3 H = V + lightDir;
        H = H.normalized();
        float specular = H.dot(normal);

        if(diffuse > 0.0f){ // Specular is added only if the diffuse is greater than 0
                colour += 0.5f*std::powf((std::max(0.0f, specular)), specCof)*spec*lightInt;
        }
    }

    if(reflective == 0.0f){
        return colour;
    }

    Vec3 R = ray - 2.0f*(ray.dot(normal)*normal);
    R = R.normalized();
    //Add the reflected colour to reflective surfaces
    colour += reflective*trace(R, spheres, numSpheres, planes, numPlanes, hitPos, lightPos, lightInt, depth+1, ambient, numLights);
    return colour;
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
bool isShadow(sphere spheres[], int numSpheres, plane planes[], int numPlanes, Vec3 &origin, Vec3 &lightPos, int index){
    Vec3 lightDir = lightPos - origin;
    lightDir = lightDir.normalized();
    float time0 = (lightPos - origin)(0)/lightDir(0);
    float time1 = (lightPos - origin)(1)/lightDir(1);
    float time2 = (lightPos - origin)(2)/lightDir(2);

    float time = std::fmaxf(time0, time1);
    time = std::fmaxf(time, time2);

    //float time = INFINITY;
    for(int i = 0; i < numSpheres; i++){
        if(index < numSpheres && index == i){
            continue;
        }
        float t = INFINITY;
        //ray-sphere intersection
        /// TODO: ray sphere intersection and shading
        Vec3 originSubC = origin - spheres[i].center; // camera - sphere center
        // calculate the discriminet to see if the ray hits the sphere
        float disc = std::powf(lightDir.dot(originSubC), 2) - originSubC.dot(originSubC) + spheres[i].radius*spheres[i].radius;

        if(disc >= 0){
            // Hits the sphere
            t = -(lightDir.dot(originSubC)) - std::sqrtf(disc);
        }

        if(t < time && t > 0.0001f){
            return true;
        }
    }
    // Ray plane intersection
    for(int i = 0; i < numPlanes; i++){
        if(index >= numSpheres && index == (i + numSpheres)){
            continue;
        }
         float t = INFINITY;
        if((planes[i].center - origin).dot(planes[i].normal) != 0){
            // ray intersects plane
            t = ((planes[i].center - origin).dot(planes[i].normal))/(lightDir.dot(planes[i].normal));
            if(t > 0.001f){
                // intersects in front of camera
                if(t < time){
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief setup - initializes the objects in the scene
 * @param spheres - sphere array to hold the sphere structs
 * @param numSpheres - number of spheres in the scene
 * @param planes - plane array to hold the plane structs
 * @param numPlanes - number of planes in the scene
 */
void setup(sphere spheres[], int numSpheres, plane planes[], int numPlanes){
    sphere s;
    s.center = Vec3(0.0f, 1.0f, -10.0f);
    s.radius = 2.0f;
    s.colour = pink();
    s.mirror = false;
    s.spec = grey();
    s.ambientCol = pink();
    s.specCof = 570.0f;
    s.glass = false;
    s.reflective = 0.0f;

    sphere s2;
    s2.center = Vec3(4.0f, 1.0f, -5.0f);
    s2.radius = 2.0f;
    s2.colour = black();
    s2.mirror = false;
    s2.spec = white();
    s2.ambientCol = grey();
    s2.specCof = 600.0f;
    s2.glass = true;
    s2.reflective = 0.025f;

    sphere s3;
    s3.center = Vec3(-4.0f, 1.0f, -5.0f);
    s3.radius = 2.0f;
    s3.colour = Colour(0.1f, 0.1f, 0.1f);
    s3.mirror = true;
    s3.spec = silver();
    s3.ambientCol = Colour(0.1f, 0.1f, 0.1f);
    s3.specCof = 1000.0f;
    s3.glass = false;
    s3.reflective = 0.0f;

    // biuld the floor as a plane parallel to the camera view
    Vec3 floorNorm = Vec3(0.0f, 1.0f, 0.0f);
    floorNorm = floorNorm.normalized();
    Vec3 floorCenter = Vec3(0.0f, -4.0f, -6.0f);
    plane floor;
    floor.center = floorCenter;
    floor.normal = floorNorm;
    floor.colour = grey();
    floor.spec = grey();
    floor.ambientCol = grey();
    floor.specCof = 380.0f;
    floor.reflective = 0.1f;

    Vec3 backNorm = Vec3(0.0f, 0.0f, 1.0f);
    backNorm = backNorm.normalized();
    Vec3 backCenter = Vec3(0.0f, 0.0f, -20.0f);

    plane back;
    back.center = backCenter;
    back.normal = backNorm;
    back.colour = pink();
    back.spec = grey();
    back.ambientCol = pink();
    back.specCof = 1000.0f;
    back.reflective = 0.1f;

    Vec3 behindNorm = Vec3(0.0f, 0.0f, -1.0f);
    behindNorm = behindNorm.normalized();
    Vec3 behindCenter = Vec3(0.0f, 0.0f, 6.0f);

    plane behind;
    behind.center = behindCenter;
    behind.normal = behindNorm;
    behind.colour = pink();
    behind.spec = grey();
    behind.ambientCol = pink();
    behind.specCof = 1000.0f;
    behind.reflective = 0.1f;

    Vec3 ceilingNorm = Vec3(0.0f, -1.0f, 0.0f);
    ceilingNorm = ceilingNorm.normalized();
    Vec3 ceilingCenter = Vec3(0.0f, 10.0f, 0.0f);

    plane ceiling;
    ceiling.center = ceilingCenter;
    ceiling.normal = ceilingNorm;
    ceiling.colour = grey();
    ceiling.spec = grey();
    ceiling.ambientCol = grey();
    ceiling.specCof = 1000.0f;
    ceiling.reflective = 0.0f;

    Vec3 leftWallNorm = Vec3(1.0f, 0.0f, 0.0f);
    leftWallNorm = leftWallNorm.normalized();
    Vec3 leftWallCenter = Vec3(-10.0f, 0.0f, 0.0f);

    plane leftWall;
    leftWall.center = leftWallCenter;
    leftWall.normal = leftWallNorm;
    leftWall.colour = red();
    leftWall.spec = grey();
    leftWall.ambientCol = red();
    leftWall.specCof = 1000.0f;
    leftWall.reflective = 0.1f;

    Vec3 rightWallNorm = Vec3(-1.0f, 0.0f, 0.0f);
    rightWallNorm = rightWallNorm.normalized();
    Vec3 rightWallCenter = Vec3(10.0f, 0.0f, 0.0f);

    plane rightWall;
    rightWall.center = rightWallCenter;
    rightWall.normal = rightWallNorm;
    rightWall.colour = blue();
    rightWall.spec = grey();
    rightWall.ambientCol = blue();
    rightWall.specCof = 1000.0f;
    rightWall.reflective = 0.1f;

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
