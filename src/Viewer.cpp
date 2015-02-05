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
	viewport_top = 15;
	viewport_bottom = 285;
	viewport_left = 15;
	viewport_right = 285;
	fov = 30.0;
	aspect = 1;
	near = 0;
	far = 10;
	m_projection.setToIdentity();
	set_perspective(fov, aspect, near, far);
	m_model.setToIdentity();
	m_view.setToIdentity();
	matrix_translation(m_view, Qt::RightButton, 500);
	m_scale.setToIdentity();
	set_mode(MODEL_ROTATE);
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
	double f = tan(M_PI_2 - fov * M_PI / 360.0);
	m_projection.setColumn(0, QVector4D(f/aspect, 0.0, 0.0, 0.0));
	m_projection.setColumn(1, QVector4D(0.0, f, 0.0, 0.0));
	m_projection.setColumn(2, QVector4D(0.0, 0.0, (near+far)/(near-far), -1));
	m_projection.setColumn(3, QVector4D(0.0, 0.0, 2 * (near * far)/(near-far), 0.0));
}

void Viewer::reset_view()
{
	fov = 30.0;
	aspect = 1;
	near = 0;
	far = 10;
	viewport_top = size().height() * 0.05;
	viewport_bottom = size().height() * 0.95;
	viewport_left = size().width() * 0.05;
	viewport_right = size().width() * 0.95;

	m_projection.setToIdentity();
	set_perspective(fov, aspect, near, far);
	m_model.setToIdentity();
	m_view.setToIdentity();
	matrix_translation(m_view, Qt::RightButton, 500);
	m_scale.setToIdentity();
	set_mode(MODEL_ROTATE);
	update();
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

	viewport << QVector2D(1.0, 1.0) << QVector2D(1.0, 0.4)
	<< QVector2D(1.0, 1.0) << QVector2D(0.4, 1.0)
	<< QVector2D(-1.0, 1.0) << QVector2D(-1.0, 0.4)
	<< QVector2D(-1.0, 1.0) << QVector2D(-0.4, 1.0)
	<< QVector2D(-1.0, -1.0) << QVector2D(-1.0, -0.4)
	<< QVector2D(-1.0, -1.0) << QVector2D(-0.4, -1.0)
	<< QVector2D(1.0, -1.0) << QVector2D(1.0, -0.4)
	<< QVector2D(1.0, -1.0) << QVector2D(0.4, -1.0);

	viewport_draw.resize(viewport.size());
}	

void Viewer::transform_points() {
	points_2d.clear();
	QVector2D draw_point;
	QVector<QVector3D> temp_points;
	QVector<QVector3D> output_points;
	float win_l = size().width();
	float view_l = viewport_right - viewport_left;
	float win_h = size().height();
	float view_h = viewport_bottom - viewport_top;
	float left_gap = ((float)viewport_left + (float)viewport_right - win_l) / win_l;
	float top_gap = ((float)viewport_top + (float)viewport_bottom - win_h) / win_h;

	QMatrix4x4 invert_view = m_view.inverted();

	// world coordinates axis
	for (int i = 0 ; i < 6; i++) {
		temp_points.push_back(invert_view * points_3d[i]);
	}

	// model coordinates axis
	for (int i = 6; i < 12; i++) {
		temp_points.push_back(invert_view * m_model * points_3d[i]);
	}

	// box 
	for (int i = 12 ; i < points_3d.size(); i++) {
		temp_points.push_back(invert_view * m_model * m_scale * points_3d[i]);
	}

// 	clip_near_far(temp_points, output_points, near, far);

//	for (int i = 0; i < output_points.size(); i++) {
//		output_points[i] = m_projection * output_points[i];
//		points_2d.push_back(QVector2D(output_points[i].x(), output_points[i].y()));
//	}

	for (int i = 0; i < temp_points.size(); i++) {
		temp_points[i] = m_projection * temp_points[i];
		points_2d.push_back(QVector2D(temp_points[i].x(), temp_points[i].y()));
	}

	// viewport transformation
	for (int i = 0; i < points_2d.size(); i++) {
		points_2d[i].setX(view_l / win_l * points_2d[i].x() + left_gap );
		points_2d[i].setY(view_h / win_h * points_2d[i].y() - top_gap);
	}

	for (int i = 0; i < viewport.size(); i++) {
		viewport_draw[i].setX(view_l / win_l * viewport[i].x() + left_gap);
		viewport_draw[i].setY(view_h / win_h * viewport[i].y() - top_gap);
	}	
}

void Viewer::matrix_rotation(QMatrix4x4 &matrix, Qt::MouseButtons mouse, const float angle) {
	qreal data[16];
	data[3] = 0;
	data[7] = 0;
	data[11] = 0;
	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 1;

	if (mouse & Qt::LeftButton) { // rotate on x-axis
		data[0] = 1; // m11
		data[1] = 0; // m12
		data[2] = 0; // m13
		data[4] = 0; // m21
		data[5] = cos(angle * M_PI / 180.0); // m22
		data[6] = -sin(angle * M_PI / 180.0); // m23
		data[8] = 0; // m31
		data[9] = sin(angle * M_PI / 180.0); // m32
		data[10] = cos(angle * M_PI / 180.0); // m33
		QMatrix4x4 temp_matrix(data[0], data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],data[13],data[14],data[15]);
		matrix = matrix * temp_matrix;
	} 

   	if (mouse & Qt::MidButton) {// rotate on y-axis
		data[0] = cos(angle * M_PI / 180.0); // m11
		data[1] = 0; // m12 
		data[2] = sin(angle * M_PI / 180.0); // m13
		data[4] = 0; // m21 
		data[5] = 1; // m22 
		data[6] = 0; // m23
		data[8] = -sin(angle * M_PI / 180.0); // m31
		data[9] = 0; // m32
		data[10] = cos(angle * M_PI / 180.0); // m33
		QMatrix4x4 temp_matrix(data[0], data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],data[13],data[14],data[15]);
		matrix = matrix * temp_matrix;
	} 

   	if (mouse & Qt::RightButton) {// rotate on z-axis
		data[0] = cos(angle * M_PI / 180.0); // m11 
        data[1] = -sin(angle * M_PI / 180.0); // m12 
        data[2] = 0; // m13 
        data[4] = sin(angle * M_PI / 180.0); // m21
        data[5] = cos(angle * M_PI / 180.0); // m22
        data[6] = 0; // m23 
        data[8] = 0; // m31
        data[9] = 0; // m32
        data[10] = 1; // m33
		QMatrix4x4 temp_matrix(data[0], data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],data[13],data[14],data[15]);
		matrix = matrix * temp_matrix;
	}	
}

void Viewer::matrix_translation(QMatrix4x4 &matrix, Qt::MouseButtons mouse, const float distance) {
	QMatrix4x4 temp_matrix;
	temp_matrix.setToIdentity();
	if (mouse & Qt::LeftButton) { //translate along the x-axis
		temp_matrix.setRow(0, QVector4D(1.0, 0, 0, distance/100.0));
	} 
   	if (mouse & Qt::MidButton) {//translate along the y-axis
		temp_matrix.setRow(1, QVector4D(0, 1.0, 0, distance/100.0 ));
	} 
   	if (mouse & Qt::RightButton) {//translate along the z-axis
		temp_matrix.setRow(2, QVector4D(0, 0, 1.0, distance/100.0));
	}	
	matrix = matrix * temp_matrix;
}

void Viewer::matrix_scale(QMatrix4x4 &matrix, Qt::MouseButtons mouse, const float factor) {
	QMatrix4x4 temp_matrix;
	temp_matrix.setToIdentity();
	if (mouse & Qt::LeftButton) { //scale along the x-axis
		temp_matrix.setColumn(0, QVector4D(1 + factor/100.0, 0, 0, 0));
	} 
	if (mouse & Qt::MidButton) {//scale along the y-axis
		temp_matrix.setColumn(1, QVector4D(0, 1 + factor/100.0, 0, 0));
	}
    if 	(mouse & Qt::RightButton) {//scale along the z-axis
		temp_matrix.setColumn(2, QVector4D(0, 0, 1 + factor/100.0, 0));
	}	
	matrix = matrix * temp_matrix;
}

void Viewer::clip_near_far(QVector<QVector3D> input_points, QVector<QVector3D> output_points, double near, double far) {
	QVector3D normalNear(0.0, 0.0, 1.0);
	QVector3D normalFar(0.0, 0.0, -1.0);
	QVector3D a;
	QVector3D b;
	float wecA;
	float wecB;
	float t;
	QVector3D pNear(0.0, 0.0, -near);
	QVector3D pFar(0.0, 0.0, -far);

	for (int i = 0; i < input_points.size(); i+=2) {
		a = input_points[i];
		b = input_points[i+1];
		// comparing to near plane
		wecA = QVector3D::dotProduct((a - pNear), normalNear);
		wecB = QVector3D::dotProduct((b - pNear), normalNear);

		if (wecA < 0 && wecB < 0) continue;
		if (wecA >= 0 && wecB >= 0) {}
		else {
			t = wecA / (wecA - wecB);
			if ( wecA < 0) {
				a = a + t * (b - a);
			} else {
				b = b + t * (b - a);
			}			
		}
		// comparing to far plane
		wecA = QVector3D::dotProduct((a - pFar), normalFar);
		wecB = QVector3D::dotProduct((b - pFar), normalFar);

		if (wecA < 0 && wecB < 0) continue;
		if (wecA >= 0 && wecB >=0) {
			output_points.push_back(a);
			output_points.push_back(b);
			continue;
		}

		t = wecA / (wecA - wecB);
		if ( wecA < 0) {
			a = a + t * (b - a);
		} else {
			b = b + t * (b - a);
		}			
		output_points.push_back(a);
		output_points.push_back(b);
	}	
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
	
	// draw the gnomon of the world
	set_colour(QColor(1.0, 0.0, 0.0));	
	for (int i = 0; i < 6; i+=2) {
		draw_line(points_2d[i], points_2d[i+1]);
	}

	// draw the gnomon of the box
	set_colour(QColor(0.0, 1.0, 0.0));
	for (int i = 6; i < 12; i+=2) {
		draw_line(points_2d[i], points_2d[i+1]);
	}

	// draw the outline of the box
    set_colour(QColor(1.0, 1.0, 1.0));
	for (int i = 12; i < points_2d.size(); i+=2) {
		draw_line(points_2d[i], points_2d[i+1]);
	}
	
	for (int i = 0; i < viewport_draw.size(); i+=2) {
		draw_line(viewport_draw[i], viewport_draw[i+1]);
	}
}

void Viewer::mousePressEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: button " << event->button() << " pressed\n";
	pressedMouseButton = event->buttons();
	prePos = event->x();
	if (mode == VIEWPORT && event->button() == Qt::LeftButton) {
		start_point.setX(event->x());
		start_point.setY(event->y());
	}
}

void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: button " << event->button() << " released\n";
	if (mode == VIEWPORT && event->button() == Qt::LeftButton) {
		end_point.setX(event->x());
		end_point.setY(event->y());

		viewport_top = start_point.y() < end_point.y() ? start_point.y():end_point.y();
		viewport_bottom = start_point.y() > end_point.y() ? start_point.y():end_point.y();
		viewport_left = start_point.x() < end_point.x() ? start_point.x():end_point.x();
		viewport_right = start_point.x() > end_point.x() ? start_point.x():end_point.x();
		
		viewport_top = viewport_top < 0 ? 0 : viewport_top;
		viewport_bottom = viewport_bottom > size().height() ? size().height() : viewport_bottom;
		viewport_left = viewport_left < 0 ? 0 : viewport_left;
		viewport_right = viewport_right > size().width() ? size().width() : viewport_right;

		update();

	}
	pressedMouseButton = Qt::NoButton;
}

void Viewer::mouseMoveEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: Motion at " << event->x() << ", " << event->y() << std::endl;
	if (mode == MODEL_ROTATE) {
		matrix_rotation(m_model, pressedMouseButton, event->x() - prePos);
	} else if (mode == MODEL_TRANSLATE) {
		matrix_translation(m_model, pressedMouseButton, event->x() - prePos);
	} else if (mode == MODEL_SCALE) {
		matrix_scale(m_scale, pressedMouseButton, event->x() - prePos);
	} else if (mode == VIEW_ROTATE) {
		matrix_rotation(m_view, pressedMouseButton, event->x() - prePos);
	} else if (mode == VIEW_TRANSLATE) {
		matrix_translation(m_view, pressedMouseButton, event->x() - prePos);
	} else if (mode == VIEW_PERSPECTIVE) {
		if (pressedMouseButton & Qt::LeftButton) {
			fov = fov + event->x() - prePos;	
			if (fov < 5.0) {
				fov = 5.0;
			} else if (fov > 160) {
				fov = 160.0;
			}
		}
	   	if (pressedMouseButton & Qt::MidButton) {
			near = near + event->x() - prePos;
		}
		if (pressedMouseButton & Qt::RightButton) {
			far = far + event->x() - prePos;
		}
		set_perspective(fov, aspect, near, far);
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
