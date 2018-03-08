#include "OpenGP/Image/Image.h"
#include "bmpwrite.h"

using namespace OpenGP;
using Colour = Vec3;

struct Triangle {
    Vec3 v1, v2, v3;
};

struct Line {
    Vec3 v1, v2;
};

float triangleArea(Vec3 v1, Vec3 v2, Vec3 v3) {
    /// TODO: Calculate the area of the triangle
    return 0.5f*(v1(0)*v2(1) + v2(0)*v3(1) + v3(0)*v1(1) - v1(0)*v3(1) - v2(0)*v1(1) - v3(0)*v2(1));
}

void rasterize(Triangle t, Image<Colour> &image) {

    // Vectors projected onto z=0 plane
    Vec3 s1 = Vec3(t.v1(0), t.v1(1), 0);
    Vec3 s2 = Vec3(t.v2(0), t.v2(1), 0);
    Vec3 s3 = Vec3(t.v3(0), t.v3(1), 0);

    /// OPTIONAL: Restrict these bounds to only rasterize triangle bounding box
    int iMin = 0;
    int iMax = image.cols();
    int jMin = 0;
    int jMax = image.rows();

    float totalArea = triangleArea(s1, s2, s3);

    for (int i = iMin;i < iMax;i++) {
        for (int j = jMin;j < jMax;j++) {

            // Location of fragment in image space
            Vec3 pt = Vec3(((float)i) / image.cols(), ((float)j) / image.rows(), 0.0);
            pt = 2 * (pt - Vec3(0.5,0.5,0));

            /// TODO: Calculate barycentric coordinates of the fragment within current triangle
            float alpha = triangleArea(pt, s2, s3)/totalArea;
            float beta = triangleArea(s1, pt, s3)/totalArea;
            float gamma = triangleArea(s1, s2, pt)/totalArea;

            if (alpha <= 1.0f && alpha >= 0.0f && beta <= 1.0f && beta >= 0.0f && gamma <= 1.0f && gamma >= 0.0f){
                image(j,i) = Colour(alpha, beta, gamma);
            }
        }
    }

}

void rasterize(Line l, Image<Colour> &image) {

    // Vectors projected onto z=0 plane
    Vec3 s1 = Vec3(l.v1(0), l.v1(1), 0);
    Vec3 s2 = Vec3(l.v2(0), l.v2(1), 0);

    /// TODO: Calculate error delta
    float deltax = s2(0) - s1(0);
    float deltay = s2(1) - s1(1);
    float deltaerr = std::abs(deltay/deltax);
    float error = 0.0f;

    /// TODO: Calculate starting pixel y-coordinate
    int y = ((s1(1)+1)*0.5f)*image.rows();

    /// TODO: Calculate for-loop bounds (pixel x-coorinate bounds)
    for ( int x = ((s1(0)+1)*0.5f)*image.cols() ; x <= ((s2(0)+1)*0.5f)*image.cols() ; ++x) {
        image(y,x) = Colour(0.5f,1.0f,0);
        /// TODO: Rest of Bresenham's line algorithm
        error += deltaerr;
        while(error >= 0.05f){
            y += deltay > 0 ? 1 : -1;
            error -= 1.0f;
        }
    }
}

int main(int, char**){

    int wResolution = 500;
    int hResolution = 500;

    Image<Colour> image(wResolution, hResolution);

    Triangle t = {
        Vec3(-0.3f,0.0f,0.0f), Vec3(0.0f,0.6f,0.0f), Vec3(0.3f,0.0f,0.0f)
    };

    Line l = {
        Vec3(-0.5f, -0.75f, 0.0f), Vec3(0.5f, -0.25f, 0.0f)
    };

    rasterize(t, image);
    rasterize(l, image);

    bmpwrite("../../out.bmp", image);
    imshow(image);

    return EXIT_SUCCESS;
}
