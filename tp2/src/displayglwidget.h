#ifndef DISPLAYGLWIDGET_H
#define DISPLAYGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include "mainwindow.h"
#include "vec.h"

class DisplayGLWidget : public QOpenGLWidget,
                        protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit DisplayGLWidget(QWidget *parent = nullptr);
    ~DisplayGLWidget();

    void setIsDisplayed(bool b);
    bool getIsDisplayed();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void updateColorBuffer(/*std::vector<QVector3D>*/);
    void updateMeshColor(HeightField hf);

private:
    inline size_t getVerticesSize() const { return vertices.size() * sizeof(QVector3D); }
    inline size_t getColorsSize() const { return colors.size() * sizeof(QVector3D); }
    inline size_t getNormalsSize() const { return normals.size() * sizeof(QVector3D); }

    bool isDisplayed;
    vec3 clearColor;

    QOpenGLBuffer m_vbo;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram m_program;

    std::vector<QVector3D> vertices;
    std::vector<QVector3D> colors;
    std::vector<QVector3D> normals;

    double maxHeight;

    friend class MainWindow;
};

#endif /* DISPLAYGLWIDGET_H */
