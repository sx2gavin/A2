#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <vector>
#include "Viewer.hpp"

class AppWindow : public QMainWindow
{
    Q_OBJECT

public:
    AppWindow();

private slots :
	void reset();
	void mode_model_rotate();
	void mode_model_translate();
	void mode_model_scale();
	void mode_view_rotate();
	void mode_view_translate();
	void mode_view_perspective();
	void mode_viewport();

private:
    void createActions();
    void createMenu();

    // Each menu itself
    QMenu* m_menu_app;
	QMenu* m_menu_mode;

    std::vector<QAction*> m_menu_app_actions;
	std::vector<QAction*> m_menu_mode_actions;
    Viewer* m_viewer;
};

#endif
