#ifndef DISPLAYGLWIDGET_H
#define DISPLAYGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <QEvent>
#include <QMouseEvent>

#include "mainwindow.h"
#include "vec.h"
#include "camera.h"

class DisplayGLWidget : public QOpenGLWidget,
                        protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit DisplayGLWidget(QWidget *parent = nullptr);
    ~DisplayGLWidget();

    void setIsDisplayed(bool b);
    bool getIsDisplayed();

    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void updateColorBuffer();
    void updateMeshColor(HeightField hf);

private:
    size_t getVerticesSize() const;
    size_t getColorsSize() const;
    size_t getNormalsSize() const;

    void updateMousePos(int &mx, int &my);
    
    bool isDisplayed;
    vec3 clearColor;

    QOpenGLBuffer m_vbo;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram m_program;

    Qt::MouseButton button;
    int mouseX;
    int mouseY;
    
    Camera m_camera;
    QMatrix4x4 m_view;
    
    std::vector<QVector3D> vertices;
    std::vector<QVector3D> colors;
    std::vector<QVector3D> normals;

    double maxHeight;
    size_t vertexCount; // Pour si on charge une aute image pour risize le VBO

    friend class MainWindow;
};

#endif /* DISPLAYGLWIDGET_H */
