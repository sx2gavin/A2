#include <QtWidgets>
#include <QtOpenGL>
#include <iostream>
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Viewer.hpp"
// #include "draw.hpp"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

using namespace std;

Viewer::Viewer(const QGLFormat& format, QWidget *parent) 
    : QGLWidget(format, parent) 
#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    , mVertexBufferObject(QOpenGLBuffer::VertexBuffer)
    , mVertexArrayObject(this)
#else 
    , mVertexBufferObject(QGLBuffer::VertexBuffer)
#endif
{
	mode = MODEL_ROTATE;
	setup_geometry();
}

Viewer::~Viewer() {
    // Nothing to do here right now.
}

QSize Viewer::minimumSizeHint() const {
    return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
    return QSize(300, 300);
}

void Viewer::set_perspective(double fov, double aspect,
                             double near, double far)
{
    // Fill me in!
}

void Viewer::reset_view()
{
    // Fill me in!
}

void Viewer::set_mode(Mode mode) {
	this->mode = mode;
}

void Viewer::setup_geometry() {
	
	points_3d << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, 0.5, 0.0)
	<< QVector3D(0.0, 0.0, 0.0) << QVector3D(0.5, 0.0, 0.0)
	<< QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, 0.0, 0.5)
	<< QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, 0.5, 0.0)
	<< QVector3D(0.0, 0.0, 0.0) << QVector3D(0.5, 0.0, 0.0)
	<< QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, 0.0, 0.5)
	<< QVector3D(-1.0, 1.0, -1.0) << QVector3D(-1.0, 1.0, 1.0)
	<< QVector3D(-1.0, 1.0, -1.0) << QVector3D(1.0, 1.0, -1.0)
	<< QVector3D(-1.0, 1.0, -1.0) << QVector3D(-1.0, -1.0, -1.0)
	<< QVector3D(-1.0, -1.0, 1.0) << QVector3D(-1.0, 1.0, 1.0)
	<< QVector3D(-1.0, -1.0, 1.0) << QVector3D(-1.0, -1.0, -1.0)
	<< QVector3D(-1.0, -1.0, 1.0) << QVector3D(1.0, -1.0, 1.0)
	<< QVector3D(1.0, 1.0, 1.0) << QVector3D(-1.0, 1.0, 1.0)
	<< QVector3D(1.0, 1.0, 1.0) << QVector3D(1.0, -1.0, 1.0)
	<< QVector3D(1.0, 1.0, 1.0) << QVector3D(1.0, 1.0, -1.0)
	<< QVector3D(1.0, -1.0, -1.0) << QVector3D(-1.0, -1.0, -1.0)
	<< QVector3D(1.0, -1.0, -1.0) << QVector3D(1.0, 1.0, -1.0)
	<< QVector3D(1.0, -1.0, -1.0) << QVector3D(1.0, -1.0, 1.0);

	m_projection.setToIdentity();
	m_model.setToIdentity();
	m_view.setToIdentity();
	
	// m_model.scale(0.5);

}	

void Viewer::transform_points() {
	points_2d.clear();
	QVector2D draw_point;
	QVector3D temp_point;

	for (int i = 0 ; i < 6; i++) {
		temp_point = points_3d[i];
		draw_point.setX(temp_point.x());
		draw_point.setY(temp_point.y());
		points_2d.push_back(draw_point);
	}

	for (int i = 6 ; i < points_3d.size(); i++) {
		temp_point = m_model * points_3d[i];
		draw_point.setX(temp_point.x());
		draw_point.setY(temp_point.y());
		points_2d.push_back(draw_point);
	}
}

void Viewer::matrix_rotation(QMatrix4x4 &matrix, Qt::MouseButton mouse, const float angle) {
	qreal data[16];
	if (mouse == Qt::LeftButton) { // rotate on x-axis
		data[0] = 1; // m11
		data[1] = 0; // m12
		data[2] = 0; // m13
		data[4] = 0; // m21
		data[5] = cos(angle * M_PI / 180.0); // m22
		data[6] = -sin(angle * M_PI / 180.0); // m23
		data[8] = 0; // m31
		data[9] = sin(angle * M_PI / 180.0); // m32
		data[10] = cos(angle * M_PI / 180.0); // m33
	} else if (mouse == Qt::MidButton) {// rotate on y-axis
		data[0] = cos(angle * M_PI / 180.0); // m11
		data[1] = 0; // m12 
		data[2] = sin(angle * M_PI / 180.0); // m13
		data[4] = 0; // m21 
		data[5] = 1; // m22 
		data[6] = 0; // m23
		data[8] = -sin(angle * M_PI / 180.0); // m31
		data[9] = 0; // m32
		data[10] = cos(angle * M_PI / 180.0); // m33
	} else if (mouse == Qt::RightButton) {// rotate on z-axis
		data[0] = cos(angle * M_PI / 180.0); // m11 
        data[1] = -sin(angle * M_PI / 180.0); // m12 
        data[2] = 0; // m13 
        data[4] = sin(angle * M_PI / 180.0); // m21
        data[5] = cos(angle * M_PI / 180.0); // m22
        data[6] = 0; // m23 
        data[8] = 0; // m31
        data[9] = 0; // m32
        data[10] = 1; // m33
	} else {
		data[0] = 0; 
		data[1] = 0; 
		data[2] = 0;
		data[4] = 0;
		data[5] = 0;
		data[6] = 0;
		data[8] = 0;
		data[9] = 0;
		data[10] = 0;
	}
	data[3] = 0;
	data[7] = 0;
	data[11] = 0;
	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 1;

	QMatrix4x4 temp_matrix(data[0], data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],data[13],data[14],data[15]);
	matrix = matrix * temp_matrix;
}

void Viewer::matrix_translation(QMatrix4x4 &matrix, Qt::MouseButton mouse, const float distance) {
	QMatrix4x4 temp_matrix;
	temp_matrix.setToIdentity();
	if (mouse == Qt::LeftButton) { //translate along the x-axis
		temp_matrix.setColumn(3, QVector4D(distance/100.0, 0, 0, 1));
	} else if (mouse == Qt::MidButton) {//translate along the y-axis
		temp_matrix.setColumn(3, QVector4D(0, distance/100.0, 0, 1));
	} else if (mouse == Qt::RightButton) {//translate along the z-axis
		temp_matrix.setColumn(3, QVector4D(0, 0, distance/100.0, 1));
	}	
	matrix = matrix * temp_matrix;
}

void Viewer::matrix_scale(QMatrix4x4 &matrix, Qt::MouseButton mouse, const float factor) {
	QMatrix4x4 temp_matrix;
	temp_matrix.setToIdentity();
	if (mouse == Qt::LeftButton) { //scale along the x-axis
		temp_matrix.setColumn(0, QVector4D(1 + factor/100.0, 0, 0, 0));
	} else if (mouse == Qt::MidButton) {//scale along the y-axis
		temp_matrix.setColumn(1, QVector4D(0, 1 + factor/100.0, 0, 0));
	} else if (mouse == Qt::RightButton) {//scale along the z-axis
		temp_matrix.setColumn(2, QVector4D(0, 0, 1 + factor/100.0, 0));
	}	
	matrix = matrix * temp_matrix;
}

void Viewer::initializeGL() {
    // Do some OpenGL setup
    QGLFormat glFormat = QGLWidget::format();
    if (!glFormat.sampleBuffers()) {
        std::cerr << "Could not enable sample buffers." << std::endl;
        return;
    }

    glClearColor(0.7, 0.7, 0.7, 0.0);
    
    if (!mProgram.addShaderFromSourceFile(QGLShader::Vertex, "shader.vert")) {
        std::cerr << "Cannot load vertex shader." << std::endl;
        return;
    }

    if (!mProgram.addShaderFromSourceFile(QGLShader::Fragment, "shader.frag")) {
        std::cerr << "Cannot load fragment shader." << std::endl;
        return;
    }

    if ( !mProgram.link() ) {
        std::cerr << "Cannot link shaders." << std::endl;
        return;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    mVertexArrayObject.create();
    mVertexArrayObject.bind();

    mVertexBufferObject.create();
    mVertexBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);
#else 

    /*
     * if qt version is less than 5.1, use the following commented code
     * instead of QOpenGLVertexVufferObject. Also use QGLBuffer instead of 
     * QOpenGLBuffer.
     */
    uint vao;
     
    typedef void (APIENTRY *_glGenVertexArrays) (GLsizei, GLuint*);
    typedef void (APIENTRY *_glBindVertexArray) (GLuint);
     
    _glGenVertexArrays glGenVertexArrays;
    _glBindVertexArray glBindVertexArray;
     
    glGenVertexArrays = (_glGenVertexArrays) QGLWidget::context()->getProcAddress("glGenVertexArrays");
    glBindVertexArray = (_glBindVertexArray) QGLWidget::context()->getProcAddress("glBindVertexArray");
     
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);    

    mVertexBufferObject.create();
    mVertexBufferObject.setUsagePattern(QGLBuffer::DynamicDraw);
#endif

    if (!mVertexBufferObject.bind()) {
        std::cerr << "could not bind vertex buffer to the context." << std::endl;
        return;
    }

    mProgram.bind();

    mProgram.enableAttributeArray("vert");
    mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

    mColorLocation = mProgram.uniformLocation("frag_color");
}

void Viewer::paintGL() {    
    draw_init();
	transform_points();

    // Here is where your drawing code should go.
    
    /* A few of lines are drawn below to show how it's done. */
    set_colour(QColor(1.0, 1.0, 1.0));

	for (int i = 0; i < points_2d.size(); i+=2) {
		draw_line(points_2d[i], points_2d[i+1]);
	}
		

//    draw_line(QVector2D(-0.9, -0.9), 
//              QVector2D(0.9, 0.9));
//    draw_line(QVector2D(0.9, -0.9),
//              QVector2D(-0.9, 0.9));
//
//    draw_line(QVector2D(-0.9, -0.9),
//              QVector2D(-0.4, -0.9));
//    draw_line(QVector2D(-0.9, -0.9), 
//              QVector2D(-0.9, -0.4));
	

}

void Viewer::mousePressEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: button " << event->button() << " pressed\n";
	pressedMouseButton = event->button();
	prePos = event->x();
}

void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: button " << event->button() << " released\n";
	pressedMouseButton = Qt::NoButton;
}

void Viewer::mouseMoveEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: Motion at " << event->x() << ", " << event->y() << std::endl;
	if (mode == MODEL_ROTATE) {
		matrix_rotation(m_model, pressedMouseButton, event->x() - prePos);
	} else if (mode == MODEL_TRANSLATE) {
		matrix_translation(m_model, pressedMouseButton, event->x() - prePos);
	} else if (mode == MODEL_SCALE) {
		matrix_scale(m_model, pressedMouseButton, event->x() - prePos);
	}
	prePos = event->x();
	update();

}

// Drawing Functions
// ******************* Do Not Touch ************************ // 

void Viewer::draw_line(const QVector2D& p1, const QVector2D& p2) {

  const GLfloat lineVertices[] = {
    p1.x(), p1.y(), 1.0,
    p2.x(), p2.y(), 1.0
  };

  mVertexBufferObject.allocate(lineVertices, 3 * 2 * sizeof(float));

  glDrawArrays(GL_LINES, 0, 2);
}

void Viewer::set_colour(const QColor& col)
{
  mProgram.setUniformValue(mColorLocation, col.red(), col.green(), col.blue());
}

void Viewer::draw_init() {
    glClearColor(0.7, 0.7, 0.7, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (double)width(), 0.0, (double)height());
    glViewport(0, 0, width(), height());

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0);
}
