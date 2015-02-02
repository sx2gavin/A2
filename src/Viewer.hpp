#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <QtGlobal>
#include <QVector>
#include "algebra.hpp"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#else 
#include <QGLBuffer>
#endif

enum Mode {VIEW_ROTATE, VIEW_TRANSLATE, VIEW_PERSPECTIVE, MODEL_ROTATE, MODEL_TRANSLATE, MODEL_SCALE};

class Viewer : public QGLWidget {
    
    Q_OBJECT


public:
    Viewer(const QGLFormat& format, QWidget *parent = 0);
    virtual ~Viewer();
    
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    // If you want to render a new frame, call do not call paintGL(),
    // instead, call update() to ensure that the view gets a paint 
    // event.

    // *** Fill in these functions (in viewer.cpp) ***

    // Set the parameters of the current perspective projection using
    // the semantics of gluPerspective().
    void set_perspective(double fov, double aspect,
                         double near, double far);

    // Restore all the transforms and perspective parameters to their
    // original state. Set the viewport to its initial size.
    void reset_view();

	// set the mode of the mouse control
	void set_mode(Mode mode);

protected:

    // Events we implement

    // Called when GL is first initialized
    virtual void initializeGL();
    // Called when our window needs to be redrawn
    virtual void paintGL();
    // Called when a mouse button is pressed
    virtual void mousePressEvent ( QMouseEvent * event );
    // Called when a mouse button is released
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    // Called when the mouse moves
    virtual void mouseMoveEvent ( QMouseEvent * event );
 
    // Draw a line -- call draw_init first!
    void draw_line(const QVector2D& p1, const QVector2D& p2) ;

    // Set the current colour
    void set_colour(const QColor& col);

    // Call this before you begin drawing. Width and height are the width
    // and height of the GL window.
    void draw_init();
	
	// define the geometry in this function.
	void setup_geometry();

	// Translate from 3d to 2d
	void transform_points();

	// Rotate Matrix
	void matrix_rotation(QMatrix4x4 &matrix, Qt::MouseButtons mouse,  const float angle);

	// Translate Matrix
	void matrix_translation(QMatrix4x4 &matrix, Qt::MouseButtons mouse,  const float distance);

	// scale Matrix
	void matrix_scale(QMatrix4x4 &matrix, Qt::MouseButtons  mouse, const float factor);	

private:

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    QOpenGLBuffer mVertexBufferObject;
    QOpenGLVertexArrayObject mVertexArrayObject;
#else 
    QGLBuffer mVertexBufferObject;
#endif

    QGLShaderProgram mProgram;

    int mColorLocation;

	QVector<QVector3D> points_3d;
	QVector<QVector2D> points_2d;
	QVector<QVector2D> viewport;
	
	Qt::MouseButtons pressedMouseButton;
	int prePos;
	Mode mode;
	
	// perspective
	double fov;
	double aspect;
	double near;
	double far;
    
    // *** Fill me in ***
    // You will want to declare some more matrices here
    QMatrix4x4 m_projection;    
	QMatrix4x4 m_model;
	QMatrix4x4 m_view;
	QMatrix4x4 m_scale;
};

#endif
