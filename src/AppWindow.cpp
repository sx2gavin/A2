#include <QtWidgets>
#include <iostream>
#include "AppWindow.hpp"

AppWindow::AppWindow() {
    setWindowTitle("488 Assignment Two");

    QGLFormat glFormat;
    glFormat.setVersion(3,3);
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);

    QVBoxLayout *layout = new QVBoxLayout;
    // m_menubar = new QMenuBar;
    m_viewer = new Viewer(glFormat, this);
    layout->addWidget(m_viewer);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);

    createActions();
    createMenu();
}


void AppWindow::createActions() {

	QAction* resetAct = new QAction(tr("&Reset"), this);
    m_menu_app_actions.push_back(resetAct);
	resetAct->setShortcut(Qt::Key_A);
    resetAct->setStatusTip(tr("Reset the view"));
    connect(resetAct, SIGNAL(triggered()), this, SLOT(reset()));

    // Creates a new action for quiting and pushes it onto the menu actions vector 
    QAction* quitAct = new QAction(tr("&Quit"), this);
    m_menu_app_actions.push_back(quitAct);

    // We set the accelerator keys
    // Alternatively, you could use: setShortcuts(Qt::CTRL + Qt::Key_P); 
    quitAct->setShortcuts(QKeySequence::Quit);

    // Set the tip
    quitAct->setStatusTip(tr("Exits the file"));

    // Connect the action with the signal and slot designated
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

	// set to Model rotation mode
	QAction* model_rotate_Act = new QAction(tr("Model &Rotation"), this);
    m_menu_mode_actions.push_back(model_rotate_Act);
	model_rotate_Act->setShortcut(Qt::Key_R);
    model_rotate_Act->setStatusTip(tr("Rotate the model around model axis"));
    connect(model_rotate_Act, SIGNAL(triggered()), this, SLOT(mode_model_rotate()));

	// set to Model translate mode
	QAction* model_translate_Act = new QAction(tr("Model &Translation"), this);
    m_menu_mode_actions.push_back(model_translate_Act);
	model_translate_Act->setShortcut(Qt::Key_T);
    model_translate_Act->setStatusTip(tr("Model translation"));
    connect(model_translate_Act, SIGNAL(triggered()), this, SLOT(mode_model_translate()));
	
	// set to Model scale mode
	QAction* model_scale_Act = new QAction(tr("Model &Scale"), this);
    m_menu_mode_actions.push_back(model_scale_Act);
	model_scale_Act->setShortcut(Qt::Key_S);
    model_scale_Act->setStatusTip(tr("Model Scale"));
    connect(model_scale_Act, SIGNAL(triggered()), this, SLOT(mode_model_scale()));

	// set to view rotation mode
	QAction* view_rotation_Act = new QAction(tr("View R&otation"), this);
    m_menu_mode_actions.push_back(view_rotation_Act);
	view_rotation_Act->setShortcut(Qt::Key_O);
    view_rotation_Act->setStatusTip(tr("View Rotation"));
    connect(view_rotation_Act, SIGNAL(triggered()), this, SLOT(mode_view_rotate()));

	// set to view translation mode
	QAction* view_translate_Act = new QAction(tr("View Translatio&n"), this);
    m_menu_mode_actions.push_back(view_translate_Act);
	view_translate_Act->setShortcut(Qt::Key_N);
    view_translate_Act->setStatusTip(tr("View Translation"));
    connect(view_translate_Act, SIGNAL(triggered()), this, SLOT(mode_view_translate()));

	// set to view perspective mode
	QAction* view_perspective_Act = new QAction(tr("View &Perspective"), this);
    m_menu_mode_actions.push_back(view_perspective_Act);
	view_perspective_Act->setShortcut(Qt::Key_P);
    view_perspective_Act->setStatusTip(tr("View Perspective"));
    connect(view_perspective_Act, SIGNAL(triggered()), this, SLOT(mode_view_perspective()));

	// set to viewport mode
	QAction* viewport_Act = new QAction(tr("&Viewport"), this);
    m_menu_mode_actions.push_back(viewport_Act);
	viewport_Act->setShortcut(Qt::Key_V);
    viewport_Act->setStatusTip(tr("Viewport"));
    connect(viewport_Act, SIGNAL(triggered()), this, SLOT(mode_viewport()));

	QActionGroup* modeGroup = new QActionGroup(this);
	model_rotate_Act->setCheckable(true);
	model_translate_Act->setCheckable(true);
	model_scale_Act->setCheckable(true);
	view_rotation_Act->setCheckable(true);
	view_translate_Act->setCheckable(true);
	view_perspective_Act->setCheckable(true);
	viewport_Act->setCheckable(true);
	model_rotate_Act->setChecked(true);
	
	model_rotate_Act->setActionGroup(modeGroup);
	model_translate_Act->setActionGroup(modeGroup);
	model_scale_Act->setActionGroup(modeGroup);
	view_rotation_Act->setActionGroup(modeGroup);
	view_translate_Act->setActionGroup(modeGroup);
	view_perspective_Act->setActionGroup(modeGroup);
	viewport_Act->setActionGroup(modeGroup);
}

void AppWindow::createMenu() {
    m_menu_app = menuBar()->addMenu(tr("&Application"));
	m_menu_mode = menuBar()->addMenu(tr("&Mode"));

    for (auto& action : m_menu_app_actions) {
        m_menu_app->addAction(action);
    }

	for (auto& action : m_menu_mode_actions) {
        m_menu_mode->addAction(action);
	}
}

void AppWindow::reset() {
	m_viewer->reset_view();
}	

void AppWindow::mode_model_rotate() {
	m_viewer->set_mode(MODEL_ROTATE);
}

void AppWindow::mode_model_translate() {
	m_viewer->set_mode(MODEL_TRANSLATE);
}

void AppWindow::mode_model_scale() {
	m_viewer->set_mode(MODEL_SCALE);
}

void AppWindow::mode_view_rotate() {
	m_viewer->set_mode(VIEW_ROTATE);
}

void AppWindow::mode_view_translate() {
	m_viewer->set_mode(VIEW_TRANSLATE);
}

void AppWindow::mode_view_perspective() {
	m_viewer->set_mode(VIEW_PERSPECTIVE);
}

void AppWindow::mode_viewport() {
	m_viewer->set_mode(VIEWPORT);
}
