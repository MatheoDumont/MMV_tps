#include "displayglwidget.h"

#include <QApplication>
#include <QMatrix4x4>
#include <QString>
#include <QVector3D>

#include <iostream>

DisplayGLWidget::DisplayGLWidget(QWidget *parent)
    : QOpenGLWidget(parent), isDisplayed(false), clearColor(0.2)
{
}

DisplayGLWidget::~DisplayGLWidget()
{
    makeCurrent();

    m_vbo.destroy();
    m_vao.destroy();
    m_program.removeAllShaders();

    doneCurrent();
}

void DisplayGLWidget::setIsDisplayed(bool b)
{
    isDisplayed = b;
}

void DisplayGLWidget::initializeGL()
{
    size_t size, offset;
    bool success;
    QString app_dir = QCoreApplication::applicationDirPath();
    
    initializeOpenGLFunctions();
    
    m_program.create();
    
    success = m_program.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                                app_dir + "/shaders/base.vs.glsl");

    if (success == false)
        std::cerr << "Error with the vertex shader...\n"
                  << m_program.log().toStdString() << std::endl;

    
    success = m_program.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                                app_dir + "/shaders/base.fs.glsl");

    if (success == false)
        std::cerr << "Error with the fragment shader...\n"
                  << m_program.log().toStdString() << std::endl;

    success = m_program.link();
    if (success == false)
        std::cerr << "Error while linking shaders...\n"
                  << m_program.log().toStdString() << std::endl; 

    m_vao.create();
    m_vao.bind();

    size = getVerticesSize() + getColorsSize() + getNormalsSize();

    m_vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo.create();
    m_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_vbo.bind();
    m_vbo.allocate(size);

    // Vertices
    offset = 0;
    size = getVerticesSize();
    std::cout << "vertices size : " << size << std::endl;
    m_vbo.write(offset, vertices.data(), size);
    glVertexAttribPointer(0,
                          3, GL_FLOAT,
                          GL_FALSE,
                          0, (const GLvoid*) offset);
    m_program.enableAttributeArray(0);

    // Colors
    offset += size;
    size = getColorsSize();
    m_vbo.write(offset, colors.data(), size);
    glVertexAttribPointer(1,
                          3, GL_FLOAT,
                          GL_FALSE,
                          0, (const GLvoid*) offset);
    m_program.enableAttributeArray(1);

    // Normals
    offset += size;
    size = getNormalsSize();
    m_vbo.write(offset, normals.data(), size);
    glVertexAttribPointer(2,
                          3, GL_FLOAT,
                          GL_FALSE,
                          0, (const GLvoid*) offset);
    m_program.enableAttributeArray(2);

    
    m_vbo.release();
    m_vao.release();

    m_program.release();
    
    glClearColor(clearColor.r(), clearColor.g(), clearColor.b(), 1.0);
    
    glClearDepth(1.f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    
    // glCullFace(GL_BACK);
    // glEnable(GL_CULL_FACE);
}

void DisplayGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void DisplayGLWidget::paintGL()
{
    if (isDisplayed == false)
        return;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    m_program.bind();
    m_vao.bind();
    
    QMatrix4x4 mvp;

    vec3 pmin, pmax, middle;
    pmin = vertices[0];
    pmax = vertices[vertices.size() - 2];

    std::cout << pmin << " | " << pmax << std::endl;
    
    middle = (pmin + pmax) / 2.0;

    std::cout << middle << std::endl;

    // View
    QMatrix4x4 view;
    
    // Magie pour être à un endroit sympa
    view.lookAt(QVector3D(middle.x, middle.y + maxHeight, -middle.z),
                QVector3D(middle.x, middle.y, middle.z),
                QVector3D(0, 1, 0));

    // Projection
    QMatrix4x4 proj;
    proj.perspective(45.f, (float(width()) / float(height())), 0.1f, pmax.x + middle.x);

    
    mvp = proj * view;
    
    m_program.setUniformValue("u_mvp", mvp);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        
    m_vao.release();
    m_program.release();
}
