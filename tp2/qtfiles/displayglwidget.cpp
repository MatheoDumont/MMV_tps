#include "displayglwidget.h"

#include <QApplication>
#include <QMatrix4x4>
#include <QString>
#include <QVector3D>

#include <iostream>

DisplayGLWidget::DisplayGLWidget(QWidget *parent)
    : QOpenGLWidget(parent), isDisplayed(false),
      clearColor(0.2), vertexCount(0)
{}

DisplayGLWidget::~DisplayGLWidget()
{
    makeCurrent();

    m_vbo.destroy();
    m_vao.destroy();
    m_program.removeAllShaders();

    doneCurrent();
}

bool DisplayGLWidget::getIsDisplayed()
{
    return isDisplayed;
}

void DisplayGLWidget::setIsDisplayed(bool b)
{
    isDisplayed = b;
}

void DisplayGLWidget::mousePressEvent(QMouseEvent *ev)
{
    button = ev->button();
}

void DisplayGLWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    button = Qt::NoButton;
}

inline void DisplayGLWidget::updateMousePos(int &mx, int &my)
{
    int tmpx, tmpy;
    
    tmpx = mx;
    tmpy = my;

    mx -= mouseX;
    my -= mouseY;

    mouseX = tmpx;
    mouseY = tmpy;
}

void DisplayGLWidget::mouseMoveEvent(QMouseEvent *ev)
{
    int mx, my;

    mx = ev->x();
    my = ev->y();

    updateMousePos(mx, my);

    switch (button)
    {
        case Qt::LeftButton:
            m_camera.rotation(mx, my);
            break;

        case Qt::RightButton:
            m_camera.move(mx);
            break;

        case Qt::MiddleButton:
            m_camera.translation(float(mx) / float(width()),
                                 float(my) / float(height()));
            break;

        default:
            break;
    }
}

void DisplayGLWidget::centerCamera()
{
    vec3 pmin, pmax, middle;
    pmin = vertices[0];
    pmax = vertices[vertices.size() - 2];

    m_camera.lookAt(pmin, pmax);
}

inline size_t DisplayGLWidget::getVerticesSize() const { return vertices.size() * sizeof(QVector3D); }
inline size_t DisplayGLWidget::getColorsSize() const { return colors.size() * sizeof(QVector3D); }
inline size_t DisplayGLWidget::getNormalsSize() const { return normals.size() * sizeof(QVector3D); }

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
    m_vbo.write(offset, vertices.data(), size);
    glVertexAttribPointer(0,
                          3, GL_FLOAT,
                          GL_FALSE,
                          0, (const GLvoid *)offset);
    m_program.enableAttributeArray(0);

    // Colors
    offset += size;
    size = getColorsSize();
    m_vbo.write(offset, colors.data(), size);
    glVertexAttribPointer(1,
                          3, GL_FLOAT,
                          GL_FALSE,
                          0, (const GLvoid *)offset);
    m_program.enableAttributeArray(1);

    // Normals
    offset += size;
    size = getNormalsSize();
    m_vbo.write(offset, normals.data(), size);
    glVertexAttribPointer(2,
                          3, GL_FLOAT,
                          GL_FALSE,
                          0, (const GLvoid *)offset);
    m_program.enableAttributeArray(2);

    m_vbo.release();
    m_vao.release();

    m_program.release();

    glClearColor(clearColor.r(), clearColor.g(), clearColor.b(), 1.0);

    glClearDepth(1.f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    vertexCount = vertices.size();

    centerCamera();
}

void DisplayGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void DisplayGLWidget::paintGL()
{
    if (isDisplayed == false)
        return;

    if (vertexCount != vertices.size())
        updateVBO();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    
    m_program.bind();
    m_vao.bind();

    QMatrix4x4 model;
    QMatrix4x4 view = m_camera.view();
    QMatrix4x4 proj = m_camera.projection(width(), height(), 45.f);
    QMatrix4x4 mvp = proj * view * model;

    vec3 camera_pos = m_camera.position();
    
    m_program.setUniformValue("u_mvp", mvp);
    m_program.setUniformValue("u_camera", QVector3D(camera_pos.x, camera_pos.y, camera_pos.z));
    m_program.setUniformValue("u_source", QVector3D(camera_pos.x, camera_pos.y, camera_pos.z));

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    m_vao.release();
    m_program.release();
    update();
}

void DisplayGLWidget::updateVertexBuffer()
{
    m_vao.bind();
    m_vbo.bind();

    size_t offset = 0;
    size_t size = getVerticesSize();
    m_vbo.write(offset, vertices.data(), size);
    glVertexAttribPointer(0,
                          3, GL_FLOAT,
                          GL_FALSE,
                          0, (const GLvoid *)offset);
    
    m_vao.release();
    m_vbo.release();
}

void DisplayGLWidget::updateColorBuffer()
{
    m_vao.bind();
    m_vbo.bind();

    size_t offset = getVerticesSize();
    size_t size = getColorsSize();
    m_vbo.write(offset, colors.data(), size);
    glVertexAttribPointer(1,
                          3, GL_FLOAT,
                          GL_FALSE,
                          0, (const GLvoid *)offset);
    
    m_vao.release();
    m_vbo.release();
}

void DisplayGLWidget::updateNormalBuffer()
{
    m_vao.bind();
    m_vbo.bind();

    size_t offset = getVerticesSize() + getColorsSize();
    size_t size = getNormalsSize();
    m_vbo.write(offset, normals.data(), size);
    glVertexAttribPointer(2,
                          3, GL_FLOAT,
                          GL_FALSE,
                          0, (const GLvoid *)offset);
    
    m_vao.release();
    m_vbo.release();
}
    
void DisplayGLWidget::updateMeshColor(HeightField hf)
{
    colors.clear();

    for (int i = 0; i < hf.getNX(); ++i)
        for (int j = 0; j < hf.getNY(); ++j)
            hf.colorCell(i, j, this->colors);

    updateColorBuffer();
}

void DisplayGLWidget::updateVBO()
{
    if (vertexCount < vertices.size())
    {
        m_vao.bind();
        m_vbo.bind();

        m_vbo.allocate(getVerticesSize() + getColorsSize() + getNormalsSize());

        m_vbo.release();
        m_vao.release();
    }
    
    updateVertexBuffer();
    updateColorBuffer();
    updateNormalBuffer();

    vertexCount = vertices.size();
    centerCamera();
}
