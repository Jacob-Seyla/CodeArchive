/**
 * Jacob Lon
 * Cs 200 Fall 23
 * Assignment 6
 */
#include "TextureRender.h"
#include <stdexcept>

/**
 * @brief
 *  Compiles and links shaders
 */
cs200::TextureRender::TextureRender(void)
{
    GLint value;
    // fragment shader
    const char *fragment_shader_text =
            "#version 130\n\
     uniform sampler2D usamp;\
     in vec2 vtexcoord;\
     out vec4 frag_color;\
     void main(void) {\
       frag_color = texture(usamp,vtexcoord);\
     }";
    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader,1,&fragment_shader_text,0);
    glCompileShader(fshader);
    glGetShaderiv(fshader,GL_COMPILE_STATUS,&value);
    // if shader link failed
    if (!value) {
        char buffer[1024];
        glGetShaderInfoLog(fshader,1024,0,buffer);
        throw std::runtime_error(buffer);
        //std::cerr << buffer << endl;
    }

    // vertex shader
    const char *vertex_shader_text =
            "#version 130\n\
     in vec4 position;\
     in vec2 texcoord;\
     uniform mat4 transform;\
     out vec2 vtexcoord;\
     void main() {\
       gl_Position = transform * position;\
       vtexcoord = texcoord;\
     }";
    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader,1,&vertex_shader_text,0);
    glCompileShader(vshader);
    glGetShaderiv(vshader,GL_COMPILE_STATUS,&value);

    // if vertex shader link failed
    if (!value) {
        char buffer[1024];
        glGetShaderInfoLog(vshader,1024,0,buffer);
        throw std::runtime_error(buffer);
        //cerr << buffer << endl;
    }

    // create program
    program = glCreateProgram();
    glAttachShader(program,fshader);
    glAttachShader(program,vshader);
    glLinkProgram(program);
    glGetProgramiv(program,GL_LINK_STATUS,&value);
    if (!value) {
        //cerr << "shader program failed to link" << endl;
        char buffer[1024];
        glGetProgramInfoLog(program,1024,0,buffer);
        throw std::runtime_error(buffer);
        //cerr << buffer << endl;
    }
    glDeleteShader(fshader);
    glDeleteShader(vshader);
}

/**
 * @brief
 *  Sets used program to 0 and delete generated program
 */
cs200::TextureRender::~TextureRender(void)
{
    glUseProgram(0);
    glDeleteProgram(program);
}

/**
 * @brief
 *  clears frame and sets background to given color
 * @param c
 *  vec4 of RGBA values
 */
void cs200::TextureRender::clearFrame(const glm::vec4 &c)
{
    // clears background using color c
    glClearColor(c[0], c[1], c[2], c[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

/**
 * @brief
 *  load data from given texture data
 * @param rgbdata
 *  array of bmp data
 * @param width
 *  width of bitmap
 * @param height
 *  height of bitmap
 */
void cs200::TextureRender::loadTexture(unsigned char *rgbdata, int width, int height)
{
    // texture buffer
    glGenTextures(1,&texture_buffer);
    glBindTexture(GL_TEXTURE_2D,texture_buffer);
    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,
                 0,GL_RGB,GL_UNSIGNED_BYTE,rgbdata);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
}

/**
 * @brief
 *  delete texture coordinate buffer, delete texture
 */
void cs200::TextureRender::unloadTexture(void)
{
    glDeleteBuffers(1, &texcoord_buffer);
    glDeleteTextures(1, &texture_buffer);
}

/**
 * @brief
 *  Sets the uniform transform
 * @param M
 *  transform to use
 */
void cs200::TextureRender::setTransform(const glm::mat4 &M)
{
    glUseProgram(program);
    glUniformMatrix4fv(utransform, 1, false, &M[0][0]);
}

/**
 * @brief
 *  load a given textured mesh, uses texture wrapping
 * @param m
 *  Textured Mesh object
 */
void cs200::TextureRender::loadMesh(const TexturedMesh &m)
{
    mesh_face_count = m.faceCount();
    // vertex buffer for standard square
    glGenBuffers(1,&vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
    float vertices[16] = { 1,1,0,1, -1,1,0,1, -1,-1,0,1, 1,-1,0,1 };
    //glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, m.vertexCount() * sizeof(*m.vertexArray()),m.vertexArray(),GL_STATIC_DRAW);

    // tex coord
    glGenBuffers(1,&texcoord_buffer);
    glBindBuffer(GL_ARRAY_BUFFER,texcoord_buffer);
    float texcoords1[8] = { 1,1, 0,1, 0,0, 1,0 };
    //glBufferData(GL_ARRAY_BUFFER,sizeof(texcoords1),texcoords1,GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, m.texcoordArray()->length() * 2 * sizeof(*m.texcoordArray()), m.texcoordArray(),GL_STATIC_DRAW);

    // face list buffer
    glGenBuffers(1,&face_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,face_buffer);
    unsigned faces[6] = { 0,1,2, 0,2,3 };
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(faces),faces,GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.faceCount() * sizeof(*m.faceArray()),m.faceArray(),GL_STATIC_DRAW);

    // VAO for first texturing
    GLint aposition = glGetAttribLocation(program,"position"),
            atexcoord = glGetAttribLocation(program,"texcoord");
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
    glVertexAttribPointer(aposition,4,GL_FLOAT,false,0,0);
    glEnableVertexAttribArray(aposition);
    glBindBuffer(GL_ARRAY_BUFFER,texcoord_buffer);
    glVertexAttribPointer(atexcoord,2,GL_FLOAT,false,0,0);
    glEnableVertexAttribArray(atexcoord);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,face_buffer);
    glBindVertexArray(0);

    // shader parameter location
    utransform = glGetUniformLocation(program,"transform");
}

/**
 * @brief
 *  deletes face and vertex buffers, delete vao
 */
void cs200::TextureRender::unloadMesh(void)
{
    glDeleteBuffers(1, &face_buffer);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteVertexArrays(1, &vao);
}

/**
 * @brief
 *  draw faces using texture
 */
void cs200::TextureRender::displayFaces(void)
{
    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, texture_buffer);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, mesh_face_count * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

