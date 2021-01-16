#include "MainWindow.h"

#include <QPushButton>
#include <QHBoxLayout>

MainWidget::MainWidget(QWidget* parent) : QWidget(parent) {
	createMembers();
	setupLayout();
	makeConnections();
}

void MainWidget::createMembers() {
	m_pushBuuttonOpenApplication = new QPushButton("Open application");
	m_app = new ImageShowDemo;
}

void MainWidget::setupLayout() {
	QHBoxLayout* hlay = new QHBoxLayout;
	hlay->addWidget(m_pushBuuttonOpenApplication);

	QVBoxLayout* vlay = new QVBoxLayout;
	vlay->addLayout(hlay);

	setLayout(vlay);
}

void MainWidget::makeConnections() {
	connect(m_pushBuuttonOpenApplication, SIGNAL(clicked()), this, SLOT(onPushButtonOpenApplication()));
}

void MainWidget::onPushButtonOpenApplication() {
	if (m_app->isApplicationclose()) {
		delete m_app;
		ImageShowDemo* temp = new ImageShowDemo;
		m_app = temp;
		temp = nullptr;
	}
	QPalette pal;
	pal.setColor(QPalette::Background, Qt::gray);
	m_app->setAutoFillBackground(true);
	m_app->setPalette(pal);
	m_app->show();
}