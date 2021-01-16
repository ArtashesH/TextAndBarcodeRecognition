#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "imageshowdemo.h"

#include <QApplication>
#include <tesseract/baseapi.h>
#include <tesseract/strngs.h>
#include <QDebug>

class QPushButton;

class MainWidget : public QWidget {
	Q_OBJECT
public:
	explicit MainWidget(QWidget* parent = 0);

private:
	QPushButton*   m_pushBuuttonOpenApplication;

	ImageShowDemo* m_app;

public:
	public slots:
	void onPushButtonOpenApplication();

private:
	void createMembers();
	void setupLayout();
	void makeConnections();
};

#endif // !MAINWINDOW_HPP