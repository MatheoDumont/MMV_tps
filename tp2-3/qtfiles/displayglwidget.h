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

    void updateMeshTopology(HeightField hf);
    void updateMeshColor(HeightField hf, HeightField::ColorType type,
                         SpecificDisplay spec = Default, int rangemin = 0, int rangemax = 359);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void updateVertexBuffer();
    void updateColorBuffer();
    void updateNormalBuffer();

private:
    size_t getVerticesSize() const;
    size_t getColorsSize() const;
    size_t getNormalsSize() const;

    void updateMousePos(int &mx, int &my);
    void updateVBO();
    void centerCamera();
    
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
    size_t vertexCount; // Pour si on charge une aute image pour resize le VBO

    friend class MainWindow;
};

#endif /* DISPLAYGLWIDGET_H */
