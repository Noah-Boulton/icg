#include <OpenGP/GL/Application.h>
#include <OpenGP/external/LodePNG/lodepng.cpp>

using namespace OpenGP;

const int width=720, height=720;
typedef Eigen::Transform<float,3,Eigen::Affine> Transform;
#define POINTSIZE 10.0f
const float SpeedFactor = 0.5;

const char* fb_vshader =
#include "fb_vshader.glsl"
;
const char* fb_fshader =
#include "fb_fshader.glsl"
;
const char* quad_vshader =
#include "quad_vshader.glsl"
;
const char* quad_fshader =
#include "quad_fshader.glsl"
;

const char* line_vshader =
#include "line_vshader.glsl"
;
const char* line_fshader =
#include "line_fshader.glsl"
;
const char* selection_vshader =
#include "selection_vshader.glsl"
;
const char* selection_fshader =
#include "selection_fshader.glsl"
;

std::unique_ptr<Shader> lineShader;
std::unique_ptr<GPUMesh> line;
std::vector<Vec2> controlPoints;

/// Selection with framebuffer pointers
std::unique_ptr<Shader> selectionShader;
std::unique_ptr<Framebuffer> selectionFB;
std::unique_ptr<RGBA8Texture> selectionColor;
std::unique_ptr<D16Texture> selectionDepth;

void init();
void quadInit(std::unique_ptr<GPUMesh> &quad);
void loadTexture(std::unique_ptr<RGBA8Texture> &texture, const char* filename);
void drawScene(float timeCount);

std::unique_ptr<GPUMesh> quad;

std::unique_ptr<Shader> quadShader;
std::unique_ptr<Shader> fbShader;

std::unique_ptr<RGBA8Texture> greg;
std::unique_ptr<RGBA8Texture> court;

std::unique_ptr<RGBA8Texture> ball;
std::unique_ptr<RGBA8Texture> speech;


//Declare Framebuffer and color buffer texture
std::unique_ptr<Framebuffer> fb;
std::unique_ptr<RGBA8Texture> c_buf;

int main(int, char**){

    Application app;
    init();

    //Initialize framebuffer
    fb = std::unique_ptr<Framebuffer>(new Framebuffer());
    //Initialize color buffer texture, and allocate memory
    c_buf = std::unique_ptr<RGBA8Texture>(new RGBA8Texture());
    c_buf->allocate(width, height);
    //Attach color texture to framebuffer
    fb->attach_color_texture(*c_buf);

    Window& window = app.create_window([](Window&){
        glViewport(0,0,width,height);
        //Draw the scene onto framebuffer
        //Bind and then unbind framebuffer
        fb->bind();
            glClear(GL_COLOR_BUFFER_BIT);
            drawScene(glfwGetTime());
        fb->unbind();
        //Render to Window, uncomment the lines and do TODOs
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        fbShader->bind();
        //Bind texture and set uniforms
        glActiveTexture(GL_TEXTURE0);
        c_buf->bind();
        fbShader->set_uniform("tex", 0);
        fbShader->set_uniform("tex_width", float(width));
        fbShader->set_uniform("tex_height", float(height));
        quad->set_attributes(*fbShader);
        quad->draw();
        //Unbind the texture
        c_buf->unbind();
        fbShader->unbind();
    });
    window.set_title("Assignment 3");
    window.set_size(width, height);

    /// Selection shader
    selectionShader = std::unique_ptr<Shader>(new Shader());
    selectionShader->verbose = true;
    selectionShader->add_vshader_from_source(selection_vshader);
    selectionShader->add_fshader_from_source(selection_fshader);
    selectionShader->link();
    /// Framebuffer for selection shader
    selectionFB = std::unique_ptr<Framebuffer>(new Framebuffer());
    selectionColor = std::unique_ptr<RGBA8Texture>(new RGBA8Texture());
    selectionColor->allocate(width,height);
    selectionDepth = std::unique_ptr<D16Texture>(new D16Texture());
    selectionDepth->allocate(width,height);
    selectionFB->attach_color_texture(*selectionColor);
    selectionFB->attach_depth_texture(*selectionDepth);

    //Setup mouse input for editing Bezier curve in seperate window
    // Mouse position and selected point
    Vec2 pixelPosition = Vec2(0,0);
    Vec2 position = Vec2(0,0);
    Vec2 *selection = nullptr;
    int offsetID = 0;

    // Display callback
    Window& mouseWindow = app.create_window([&](Window&){
        glViewport(0,0,width,height);
        glClear(GL_COLOR_BUFFER_BIT);
        glPointSize(POINTSIZE);

        lineShader->bind();

        // Draw line red
        lineShader->set_uniform("selection", -1);
        line->set_attributes(*lineShader);
        line->set_mode(GL_LINE_STRIP);
        line->draw();

        // Draw points red and selected point blue
        if(selection!=nullptr) lineShader->set_uniform("selection", int(selection-&controlPoints[0]));
        line->set_mode(GL_POINTS);
        line->draw();

        lineShader->unbind();
    });
    mouseWindow.set_title("Mouse");
    mouseWindow.set_size(width, height);

    // Mouse movement callback
    mouseWindow.add_listener<MouseMoveEvent>([&](const MouseMoveEvent &m){
        // Mouse position in clip coordinates
        pixelPosition = m.position;
        Vec2 p = 2.0f*(Vec2(m.position.x()/width,-m.position.y()/height) - Vec2(0.5f,-0.5f));
        if( selection && (p-position).norm() > 0.0f) {
            selection->x() = position.x();
            selection->y() = position.y();
            line->set_vbo<Vec2>("vposition", controlPoints);
        }
        position = p;
    });

    // Mouse click callback
    mouseWindow.add_listener<MouseButtonEvent>([&](const MouseButtonEvent &e){
        // Mouse selection case
        if( e.button == GLFW_MOUSE_BUTTON_LEFT && !e.released) {

            /// Draw element id's to framebuffer
            selectionFB->bind();
            glViewport(0,0,width, height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color must be 1,1,1,1
            glPointSize(POINTSIZE);
            selectionShader->bind();
            selectionShader->set_uniform("offsetID", offsetID);
            line->set_attributes(*selectionShader);
            line->set_mode(GL_POINTS);
            line->draw();
            selectionShader->unbind();
            glFlush();
            glFinish();

            selection = nullptr;
            unsigned char a[4];
            glReadPixels(int(pixelPosition[0]), height - int(pixelPosition[1]), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &a);
            selection = &controlPoints[0] + (int)a[0];
            selectionFB->unbind();
        }
        // Mouse release case
        if( e.button == GLFW_MOUSE_BUTTON_LEFT && e.released) {
            if(selection) {
                selection->x() = position.x();
                selection->y() = position.y();
                selection = nullptr;
                line->set_vbo<Vec2>("vposition", controlPoints);
            }
        }
    });

    return app.run();
}

void init(){
    glClearColor(1,1,1, /*solid*/1.0 );

    fbShader = std::unique_ptr<Shader>(new Shader());
    fbShader->verbose = true;
    fbShader->add_vshader_from_source(fb_vshader);
    fbShader->add_fshader_from_source(fb_fshader);
    fbShader->link();

    quadShader = std::unique_ptr<Shader>(new Shader());
    quadShader->verbose = true;
    quadShader->add_vshader_from_source(quad_vshader);
    quadShader->add_fshader_from_source(quad_fshader);
    quadShader->link();

    quadInit(quad);

    loadTexture(greg, "greg.png");
    loadTexture(court, "court.png");
    loadTexture(ball, "basketball.png");
    loadTexture(speech, "speech.png");

    glClearColor(1,1,1, /*solid*/1.0 );

    lineShader = std::unique_ptr<Shader>(new Shader());
    lineShader->verbose = true;
    lineShader->add_vshader_from_source(line_vshader);
    lineShader->add_fshader_from_source(line_fshader);
    lineShader->link();

    controlPoints = std::vector<Vec2>();
    controlPoints.push_back(Vec2( 0.9f, 0.9f));
    controlPoints.push_back(Vec2( 0.7f, 0.7f));
    controlPoints.push_back(Vec2( 0.25f, 0.25f));
    controlPoints.push_back(Vec2( 0.0f, 0.0f));
    controlPoints.push_back(Vec2(-0.5f,-0.5f));
    controlPoints.push_back(Vec2( -0.9f, -0.9f));

    line = std::unique_ptr<GPUMesh>(new GPUMesh());
    line->set_vbo<Vec2>("vposition", controlPoints);
    std::vector<unsigned int> indices = {0,1,2,3,4,5};
    line->set_triangles(indices);
}

void quadInit(std::unique_ptr<GPUMesh> &quad) {
    quad = std::unique_ptr<GPUMesh>(new GPUMesh());
    std::vector<Vec3> quad_vposition = {
        Vec3(-1, -1, 0),
        Vec3(-1,  1, 0),
        Vec3( 1, -1, 0),
        Vec3( 1,  1, 0)
    };
    quad->set_vbo<Vec3>("vposition", quad_vposition);
    std::vector<unsigned int> quad_triangle_indices = {
        0, 2, 1, 1, 2, 3
    };
    quad->set_triangles(quad_triangle_indices);
    std::vector<Vec2> quad_vtexcoord = {
        Vec2(0, 0),
        Vec2(0,  1),
        Vec2( 1, 0),
        Vec2( 1,  1)
    };
    quad->set_vtexcoord(quad_vtexcoord);
}

void loadTexture(std::unique_ptr<RGBA8Texture> &texture, const char *filename) {
    // Used snippet from https://raw.githubusercontent.com/lvandeve/lodepng/master/examples/example_decode.cpp
    std::vector<unsigned char> image; //the raw pixels
    unsigned width, height;
    //decode
    unsigned error = lodepng::decode(image, width, height, filename);
    //if there's an error, display it
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...

    // unfortunately they are upside down...lets fix that
    unsigned char* row = new unsigned char[4*width];
    for(int i = 0; i < int(height)/2; ++i) {
        memcpy(row, &image[4*i*width], 4*width*sizeof(unsigned char));
        memcpy(&image[4*i*width], &image[image.size() - 4*(i+1)*width], 4*width*sizeof(unsigned char));
        memcpy(&image[image.size() - 4*(i+1)*width], row, 4*width*sizeof(unsigned char));
    }
    delete row;

    texture = std::unique_ptr<RGBA8Texture>(new RGBA8Texture());
    texture->upload_raw(width, height, &image[0]);
}

void drawScene(float timeCount) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float t = timeCount * SpeedFactor;
    Transform TRS = Transform::Identity();
    //background.draw(TRS.matrix());
    quadShader->bind();
    quadShader->set_uniform("M", TRS.matrix());
    // Make texture unit 0 active
    glActiveTexture(GL_TEXTURE0);
    // Bind the texture to the active unit for drawing
    court->bind();
    // Set the shader's texture uniform to the index of the texture unit we have bound the texture to
    quadShader->set_uniform("tex", 0);
    quad->set_attributes(*quadShader);
    quad->draw();
    court->unbind();

    //Restrict the time to between 0 and 1
    t = fmod(t,1.0f);

    // Control points for Bezier curve
    Vec2 P0 = controlPoints[0];
    Vec2 P1 = controlPoints[1];
    Vec2 P2 = controlPoints[2];
    Vec2 P3 = controlPoints[3];
    Vec2 P4 = controlPoints[4];
    Vec2 P5 = controlPoints[5];

    //Calculate Greg's posistion based on the Bezier Curve
    Vec2 pos = pow((1-t), 5.0f)*P0
                + 5.0f*t*pow((1-t), 4.0f)*P1
                + 10.0f*pow(t, 2.0f)*pow((1-t), 3.0f)*P2
                + 10.0f*pow(t, 3.0f)*pow((1-t), 2.0f)*P3
                + 5.0f*pow(t, 4.0f)*(1-t)*P4
                + pow(t, 5.0f)*P5;

    //Setup translation and scaling
    TRS *= Eigen::Translation3f(pos(0), pos(1), 0);
    TRS *= Eigen::AlignedScaling3f(t*0.5f, t*0.5f, 1);
    quadShader->bind();
    quadShader->set_uniform("M", TRS.matrix());
    // Make texture unit 0 active
    glActiveTexture(GL_TEXTURE0);
    // Bind the texture to the active unit for drawing
    greg->bind();
    // Set the shader's texture uniform to the index of the texture unit we have bound the texture to
    quadShader->set_uniform("tex", 0);
    quad->set_attributes(*quadShader);
    quad->draw();
    greg->unbind();

    //Setup transalaton for speech bubble relative to Greg
    TRS *= Eigen::Translation3f(1.0, 1.0, 0.0);
    quadShader->bind();
    quadShader->set_uniform("M", TRS.matrix());
    // Make texture unit 0 active
    glActiveTexture(GL_TEXTURE0);
    // Bind the texture to the active unit for drawing
    speech->bind();
    // Set the shader's texture uniform to the index of the texture unit we have bound the texture to
    quadShader->set_uniform("tex", 0);
    quad->set_attributes(*quadShader);
    quad->draw();
    speech->unbind();

    //Translate back to Greg's posistion to setup ball matrix
    TRS *= Eigen::Translation3f(-1.0, -1.0, 0.0);

    // Make the ball orbit around Greg
    TRS *= Eigen::AngleAxisf(t/(0.25/4.0), Eigen::Vector3f::UnitZ());
    TRS *= Eigen::Translation3f(1.0, 1.0, 0.0);
    // Make the ball spin
    TRS *= Eigen::AngleAxisf(-t/(0.25/4.0), -Eigen::Vector3f::UnitZ());
    // Make the ball smaller
    TRS *= Eigen::AlignedScaling3f(0.5f, 0.5f, 1);

    quadShader->bind();
    // Make texture unit 0 active
    quadShader->set_uniform("M", TRS.matrix());
    glActiveTexture(GL_TEXTURE0);
    // Bind the texture to the active unit for drawing
    ball->bind();
    // Set the shader's texture uniform to the index of the texture unit we have bound the texture to
    quadShader->set_uniform("tex", 0);
    quad->set_attributes(*quadShader);
    quad->draw();
    ball->unbind();

    quadShader->unbind();

    glDisable(GL_BLEND);
}
