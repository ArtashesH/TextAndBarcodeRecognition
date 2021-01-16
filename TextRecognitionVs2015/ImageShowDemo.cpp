#include "imageshowdemo.h"

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QPushButton>
#include <QGridLayout>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include <QString>
#include <QFileDialog>
#include <QSlider>
#include <QTableWidget>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

#include <time.h>
#include <cmath>

#include <QCloseEvent>

ImageShowDemo::ImageShowDemo(QWidget *parent) :
QWidget(parent)

{
	createMembers();
	setupLayout();
	makeConnections();
	resize(1200, 600);
}

void ImageShowDemo::closeEvent(QCloseEvent *event) {
	if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation?",
		"Are you sure you want to exit?",
		QMessageBox::Yes | QMessageBox::No))
	{
		event->accept();
		m_isCloseApp = true;
	}
}

bool ImageShowDemo::isApplicationclose() {
	return m_isCloseApp;
}

void  ImageShowDemo::createMembers()
{
	m_textRegionExtractor = new TextRegionExtractor;
	m_graphicsViewImage = new QGraphicsView;
	m_graphicsViewImage->setStyleSheet("background-color: gray");
	m_graphicsViewText = new QGraphicsView;
	m_graphicsViewText->setStyleSheet("background-color: gray");

	m_scenImage = new QGraphicsScene;
	m_scenText = new QGraphicsScene;
	m_scenCode = new QGraphicsScene;

	m_timer = new QTimer;
	m_pushButtonRecognize = new QPushButton("Recognize");
	m_pushButtonRecognize->setStyleSheet("QPushButton {background-color: gray; color: black;}");
	
	m_pushButtonOpenImage = new  QPushButton("Choose image");
	m_pushButtonOpenImage->setFixedSize(100, 30);
	m_pushButtonOpenImage->setStyleSheet("QPushButton {background-color: gray; color: black;}");

	m_pushButtonOpenTextFile = new QPushButton("View texts");
	m_pushButtonOpenTextFile->setFixedSize(100, 30);
	m_pushButtonOpenTextFile->setStyleSheet("QPushButton {background-color: gray; color: black;}");

	m_pushButtonOpenCamera = new QPushButton("Open camera");
	m_pushButtonOpenCamera->setFixedSize(100, 30);
	m_pushButtonOpenCamera->setStyleSheet("QPushButton {background-color: gray; color: black;}");
	
	m_pushButtonOpenVideo = new QPushButton("Choose video");
	m_pushButtonOpenVideo->setFixedSize(100, 30);
	m_pushButtonOpenVideo->setStyleSheet("QPushButton {background-color: gray; color: black;}");

	m_pushButtonStart_Stop = new QPushButton("Stop");

	m_pushRadioButtonTesseract = new QRadioButton(tr("Text Recognition"));
	m_pushRadioButtonTesseract->setChecked(true);
	m_pushRadioButtonTesseract->setFixedHeight(40);

	m_pushRadioButtonBarcode = new QRadioButton(tr("Barcode Recognition"));
	m_pushRadioButtonBarcode->setFixedHeight(40);

	m_pushRadioButtonQrCode = new QRadioButton(tr("Qr Code Recognition"));
	m_pushRadioButtonQrCode->setFixedHeight(40);

	m_pushMenuBarSelectMode = new QMenu("Choose mode");
	m_pushMenuBarSelectMode->setStyleSheet("QMenu {background-color: gray; color: black;}");


	m_pushAction_RIL_TEXTLINE = new QAction(m_pushMenuBarSelectMode);
	m_pushAction_RIL_TEXTLINE->setText("Textline");
	m_pushAction_RIL_WORD = new QAction(m_pushMenuBarSelectMode);
	m_pushAction_RIL_WORD->setText("Word");
	m_pushAction_RIL_BLOCK = new QAction(m_pushMenuBarSelectMode);
	m_pushAction_RIL_BLOCK->setText("Block");
	m_pushAction_RIL_SYMBOL = new QAction(m_pushMenuBarSelectMode);
	m_pushAction_RIL_SYMBOL->setText("Symbol");


	m_pushMenuBarSelectMode->addAction(m_pushAction_RIL_TEXTLINE);
	m_pushMenuBarSelectMode->addAction(m_pushAction_RIL_WORD);
	m_pushMenuBarSelectMode->addAction(m_pushAction_RIL_BLOCK);
	m_pushMenuBarSelectMode->addAction(m_pushAction_RIL_SYMBOL);

	m_pushMenuBarSelectModeBar = new QMenuBar;
	m_pushMenuBarSelectModeBar->addMenu(m_pushMenuBarSelectMode);
	m_pushMenuBarSelectModeBar->setStyleSheet("QMenuBar {background-color: gray; color: white;}");

}

void  ImageShowDemo::setupLayout()
{
	QHBoxLayout* hlay = new QHBoxLayout;
	hlay->addWidget(m_pushButtonOpenImage);
	hlay->addWidget(m_pushButtonOpenVideo);
	hlay->addWidget(m_pushButtonOpenCamera);
	hlay->addStretch(1);
	hlay->addSpacing(5);
	hlay->setContentsMargins(0, 0, 164, 0);

	QHBoxLayout* hlay1 = new QHBoxLayout;
	hlay1->addLayout(hlay);
	hlay1->addWidget(m_pushButtonOpenTextFile);
	hlay1->addStretch(1);

	QHBoxLayout* hlay2 = new QHBoxLayout;
	hlay2->addWidget(m_graphicsViewImage);
	hlay2->addWidget(m_graphicsViewText);

	QHBoxLayout* hlay3 = new QHBoxLayout;
	hlay3->addWidget(m_pushButtonRecognize);

	QHBoxLayout* hlay4 = new QHBoxLayout;
	hlay4->addWidget(m_pushButtonStart_Stop);
	m_pushButtonStart_Stop->hide();

	QHBoxLayout* hlay5 = new QHBoxLayout;
	hlay5->addWidget(m_pushRadioButtonTesseract);
	hlay5->addWidget(m_pushRadioButtonBarcode);
	hlay5->addWidget(m_pushRadioButtonQrCode);
	hlay5->addStretch(1);
	hlay5->addSpacing(5);
	hlay5->setContentsMargins(9, 0, 0, 0);

	QHBoxLayout* hlay6 = new QHBoxLayout;
	hlay6->addWidget(m_pushMenuBarSelectModeBar);
	hlay6->setContentsMargins(0, 0, 0, 90);

	QVBoxLayout* vlay = new QVBoxLayout;

	vlay->addLayout(hlay1);
	vlay->addLayout(hlay2);
	vlay->addLayout(hlay3);
	vlay->addLayout(hlay4);
	vlay->addLayout(hlay5);
	vlay->addLayout(hlay6);

	m_graphicsViewImage->setScene(m_scenImage);
	m_graphicsViewText->setScene(m_scenText);
	m_graphicsViewText->resize(m_graphicsViewImage->size());

	setLayout(vlay);

	m_isRecognizeChecked = false;
}

void  ImageShowDemo::makeConnections()
{
	connect(m_timer, SIGNAL(timeout()), this, SLOT(testImageCapture()));
	connect(m_pushButtonRecognize, SIGNAL(clicked()), this, SLOT(onPushButtonRecognize()));
	connect(m_pushButtonOpenImage, SIGNAL(clicked()), this, SLOT(onPushbuttonOpenImage()));
	connect(m_pushButtonOpenTextFile, SIGNAL(clicked()), this, SLOT(onPushbuttonOpenTextFile()));
	connect(m_pushButtonOpenVideo, SIGNAL(clicked()), this, SLOT(onPushbuttonOpenVideo()));
	connect(m_pushButtonOpenCamera, SIGNAL(clicked()), this, SLOT(onPushbuttonOpenCamera()));
	connect(m_pushButtonStart_Stop, SIGNAL(clicked()), this, SLOT(onPushbuttonOpenStart_Stop()));
	connect(m_pushRadioButtonTesseract, SIGNAL(toggled(bool)), this, SLOT(onPushRadioButtonTesseract()));
	connect(m_pushRadioButtonBarcode, SIGNAL(toggled(bool)), this, SLOT(onPushRadioButtonBarcode()));
	connect(m_pushRadioButtonQrCode, SIGNAL(toggled(bool)), this, SLOT(onPushRadioButtonQrCode()));
	connect(m_pushAction_RIL_TEXTLINE, SIGNAL(triggered(bool)), this, SLOT(onPushAction_RIL_TEXTLINE()));
	connect(m_pushAction_RIL_WORD, SIGNAL(triggered(bool)), this, SLOT(onPushAction_RIL_WORD()));
	connect(m_pushAction_RIL_BLOCK, SIGNAL(triggered(bool)), this, SLOT(onPushAction_RIL_BLOCK()));
	connect(m_pushAction_RIL_SYMBOL, SIGNAL(triggered(bool)), this, SLOT(onPushAction_RIL_SYMBOL()));
}

cv::Mat ImageShowDemo::rotateImageWithAngle(const double angle, const cv::Mat& initImg)
{
	cv::Mat m_clone = initImg.clone();
	static int ang = -5;
	cv::Point2f center(m_clone.cols / 2.0, m_clone.rows / 2.0);
	cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
	// determine bounding rectangle
	unsigned rect_angle = 5;
	if (angle <= -10) {
		rect_angle = 0;
	}
	cv::Rect bbox = cv::RotatedRect(center, m_clone.size(), rect_angle).boundingRect();

	// adjust transformation matrix
	rot.at<double>(0, 2) += bbox.width / 2.0 - center.x;
	rot.at<double>(1, 2) += bbox.height / 2.0 - center.y;

	cv::warpAffine(m_clone, m_clone, rot, bbox.size());
	ang -= 5;
	return m_clone;
}

void ImageShowDemo::testImageCapture()
{
	if (!m_imageCapture.isOpened() && m_whichButton != WhichButton::OPEN_IMAGE) {
		std::cout << "Image capture is not valid \n";
		exit(1);
	}
	cv::Mat frame;
	if (m_whichButton == WhichButton::OPEN_IMAGE) {
		frame = m_image;
	}
	else if (m_whichButton == WhichButton::OPEN_VIDEO 
		 ||  m_whichButton == WhichButton::OPEN_CAMERA) {
		m_imageCapture >> frame;
		m_image = frame;
	}
	if (frame.empty()) {
		m_timer->stop();
		return;
	}
	static int i = 0;
	cv::Mat rgbFrame;
	cv::cvtColor(frame, rgbFrame, CV_BGR2RGB);
	/*if ((frame.rows > 2000 && frame.rows < 2500)
		&& (frame.cols > 2000 && frame.cols < 2500)) {
		cv::resize(frame, frame, cv::Size(frame.cols / 2.5, frame.rows / 2.5));
	}
	else if (frame.rows >= 2500 && frame.cols >= 2500) {
		cv::resize(frame, frame, cv::Size(frame.cols / 3.7, frame.rows / 3.7));
	}*/
	//m_textRegionExtractor->setFrame(frame);
	m_currentPixmap = QPixmap::fromImage(QImage((uchar*)rgbFrame.data, rgbFrame.cols, rgbFrame.rows, rgbFrame.step1(), QImage::Format_RGB888).copy());
	m_scenImage->clear();
	m_scenImage->addPixmap(m_currentPixmap);
	m_graphicsViewText->resize(m_graphicsViewImage->size());
}

void ImageShowDemo::onPushButtonRecognize()
{
	if (m_image.empty()) {
		QMessageBox::StandardButton message;
		message = QMessageBox::warning(this, "Warning", "Please choose image!!!", QMessageBox::Ok);
		return;
	}

	m_pushButtonRecognize->setStyleSheet("QPushButton {background-color: gray; color: black;}");

	if (m_pushRadioButtonTesseract->isChecked()) {
		if (!m_isModeChecked) {
			QMessageBox::StandardButton message;
			message = QMessageBox::warning(this, "Warning", "Please choose text recognition mode!!!", QMessageBox::Ok);
			return;
		}
		m_isRecognizeChecked = true;
		onPushRadioButtonTesseract();
	}
	else if(m_pushRadioButtonBarcode->isChecked()) {
		m_isRecognizeChecked = true;
		onPushRadioButtonBarcode();
	}
	else if (m_pushRadioButtonQrCode->isChecked()) {
		m_isRecognizeChecked = true;
		onPushRadioButtonQrCode();
	}
	m_isRecognizeChecked = false;
}

void ImageShowDemo::onPushbuttonOpenImage()
{
	static bool isCheckedOpenImage = false;
	m_imgFilename = QFileDialog::getOpenFileName(
		this,
		tr("Open image"),
		"",
		"Image files (*.jpg *.tiff *.png *.jpeg)");

	if (isCheckedOpenImage) {
		m_pushButtonRecognize->setStyleSheet("QPushButton {background-color: gray; color: black;}");
	}

	if (m_imgFilename.isEmpty()) {
		return;
	}
	m_isGraphicsCalled = false;

	m_whichButton = WhichButton::OPEN_IMAGE;
	m_scenImage->clear();
	m_scenText->clear();
	m_pushButtonStart_Stop->hide();
	m_pushButtonRecognize->show();
	m_imagePath = m_imgFilename.toStdString();
	m_image = cv::imread(m_imagePath);
	//resizeImageForRecognition();
	/*if ((m_image.rows > 2000 && m_image.rows < 2500)
		&& (m_image.cols > 2000 && m_image.cols < 2500)) {
		cv::resize(m_image, m_image, cv::Size(m_image.cols / 2.5, m_image.rows / 2.5));
	}
	else if (m_image.rows >= 2000 && m_image.cols >= 2000) {
		cv::resize(m_image, m_image, cv::Size(m_image.cols / 5, m_image.rows / 7));
	}*/
	//m_pushMenuBarSelectMode->title().clear();
	//m_pushMenuBarSelectMode->setTitle("Choose mode");
	m_isModeChecked = false;
	if (m_pushMenuBarSelectMode->title() != "Choose mode") {
		m_isModeChecked = true;
	}

	m_graphicsViewText->setStyleSheet("background-color: white");
	//m_image = rotateImageWithAngle(45, m_image);
	//m_imageCapture.open(m_imagePath);

	testImageCapture();
	isCheckedOpenImage = true;
}

void ImageShowDemo::onPushbuttonOpenTextFile() {	
	m_txtFilename = QFileDialog::getOpenFileName(
		this,
		tr("Open Text file"),
		"$(SolutionDir)",
		"Text files (*.txt)");
	if (m_txtFilename.isEmpty()) {
		return;
	}
	
	QProcess *proc = new QProcess(this);
	m_txtFilename.replace('/', '\\');
	proc->setProgram("notepad.exe");
	proc->setArguments(QStringList() << m_txtFilename);
	qDebug() << m_txtFilename;
	proc->start();
}

void ImageShowDemo::onPushbuttonOpenVideo()
{
	m_videoFilename = QFileDialog::getOpenFileName(
		this,
		tr("Open image"),
		"",
		"Video files (*.mp4 *.avi)");
	
	if (m_videoFilename.isEmpty()) {
		m_timer->stop();
		return;
	}
	m_isGraphicsCalled = false;

	m_whichButton = WhichButton::OPEN_VIDEO;
	m_pushButtonRecognize->hide();
	m_pushButtonStart_Stop->setText("Stop");
	m_pushButtonStart_Stop->show();
	m_scenImage->clear();
	m_scenText->clear();
	m_imageCapture.open(m_videoFilename.toStdString());
	if (!m_imageCapture.isOpened()) {
		qDebug() << "VideoCapture is not openned fileame is not valid" << m_videoFilename;
		m_timer->stop();
		return;
	}

	m_graphicsViewText->setStyleSheet("background-color: white");
	m_timer->start(30);
}

void ImageShowDemo::onPushbuttonOpenCamera() {
	m_imageCapture.open(0);
	if (!m_imageCapture.isOpened()) {
		qDebug() << "VideoCapture is not openned. Camera not worked" << m_videoFilename;
		return;
	}
	m_isGraphicsCalled = false;

	m_scenImage->clear();
	m_whichButton = WhichButton::OPEN_CAMERA;
	m_pushButtonRecognize->hide();
	m_pushButtonStart_Stop->setText("Stop");
	m_pushButtonStart_Stop->show();
	m_scenText->clear();
	m_timer->start(30);

	m_graphicsViewText->setStyleSheet("background-color: white");
}

void ImageShowDemo::onPushbuttonOpenStart_Stop() {
	if (m_pushButtonStart_Stop->text() == "Stop") {
		m_pushButtonRecognize->show();
		m_timer->stop();
		m_imageCapture >> m_image;
		m_pushButtonStart_Stop->setText("Start");
		return;
	}
	if (m_pushButtonStart_Stop->text() == "Start") {
		m_pushButtonRecognize->hide();
		m_scenText->clear();
		m_timer->start(30);
		m_pushButtonStart_Stop->setText("Stop");
		m_isStartCalled = true;
	}
}

void ImageShowDemo::onPushRadioButtonTesseract() {
	if (m_isRecognizeChecked) {
		resizeImageForRecognition();
		AngleAndRects angleAndRects = getAngle();
		if (!angleAndRects.size()) {
			return;
		}
		double   selectedAngle{};
		unsigned countOfAlmostZeroAngle{};
		unsigned countOfAlmostEqualAngle{};
		unsigned currentCountOfAlmostEqualAngle{};
		unsigned indexOfAngle{};

		for (int i = 0; i < angleAndRects.size(); ++i) {
			if (angleAndRects[i].first == 0) {
				++countOfAlmostZeroAngle;
				continue;
			}
			if ((angleAndRects[i].first > (selectedAngle + 5))
				|| (angleAndRects[i].first < (selectedAngle - 5))) {
				selectedAngle = angleAndRects[i].first;
				currentCountOfAlmostEqualAngle = 0;
			}
			else {
				++currentCountOfAlmostEqualAngle;
			}
			if (currentCountOfAlmostEqualAngle > countOfAlmostEqualAngle) {
				countOfAlmostEqualAngle = currentCountOfAlmostEqualAngle;
				indexOfAngle = i;
			}
		}
		if (countOfAlmostZeroAngle > countOfAlmostEqualAngle) {
			selectedAngle = 0;
		}
		else {
			selectedAngle = angleAndRects[indexOfAngle].first;
		}

		/*unsigned maxSize{};
		for (int i = 0; i < angleAndRects.size(); ++i) {
			for (int j = 0; j < angleAndRects[i].second.size(); ++j) {
				if (maxSize < angleAndRects[i].second[j].size()) {
					maxSize = angleAndRects[i].second[j].size();
					indexOfAngle = i;
				}
			}
		}
		selectedAngle = angleAndRects[indexOfAngle].first;*/

		//for (int i = 0; i < angles.size(); ++i) {
		//	if (angles[i] > -7 && angles[i] < 7) {
		//		++countOfAlmostZeroAngle;
		//	}
		//	if ((angles[i] >(selectedAngle + 5))
		//		|| (angles[i] < (selectedAngle - 5))) {
		//		selectedAngle = angles[i];
		//		currentCountOfAlmostEqualAngle = 0;
		//	}
		//	else {
		//		++currentCountOfAlmostEqualAngle;
		//	}
		//	//std::cout << "\nAngle = " << angles[i] << std::endl;
		//	if (currentCountOfAlmostEqualAngle > countOfAlmostEqualAngle) {
		//		countOfAlmostEqualAngle = currentCountOfAlmostEqualAngle;
		//		indexOfAngle = i;
		//	}
		//}
		//if (m_points.size() < 10 && countOfAlmostZeroAngle > 0) {
		//	selectedAngle = 0;
		//}
		//else if (countOfAlmostZeroAngle >= 2) {
		//	selectedAngle = 0;
		//}
		//else {
		//	selectedAngle = angles[indexOfAngle];
		//}
		//if (selectedAngle > -8 && selectedAngle < 8) {
		//	selectedAngle = 0;
		//}

		cv::Mat currentImg = m_image.clone();
		if (selectedAngle) {
			currentImg = rotateImageWithAngle(selectedAngle, currentImg);
		}
		
		m_textRegionExtractor->setFrame(currentImg);
		TextRegionExtractor* opt = new TextRegionExtractor;
		StartRec rt = m_textRegionExtractor->run(*opt, m_mode);

		if (m_isGraphicsCalled && !m_isStartCalled) {
			return;
		}
		QRect newPosrectText;
		QPen pen(Qt::green, 2, Qt::SolidLine, Qt::MPenCapStyle, Qt::MPenJoinStyle);
		m_scenText->clear();

		time_t  timev;
		time(&timev);
		std::string strTime = std::to_string(timev);

		m_inputText.open(strTime + ".txt");

		m_graphicsViewText->setScene(m_scenText);
		m_scenImage->addPixmap(m_currentPixmap);
		for (int i = 0; i < rt.size(); ++i) {
			newPosrectText = QRect(rt.at(i).first.x, rt.at(i).first.y, rt.at(i).first.width, rt.at(i).first.height);
			QString text = QString::fromStdString(rt.at(i).second);
			m_inputText << text.toUtf8().constData();
			QPixmap pixmap = getStringImg(text, newPosrectText);
			QGraphicsPixmapItem* item1 = new QGraphicsPixmapItem(pixmap);
			item1->setPos(newPosrectText.x(), newPosrectText.y());
			m_scenText->addItem(item1);
			m_scenImage->addRect(newPosrectText, pen);
		}
		//cv::Rect newPosrectTextCopy;
		//newPosrectTextCopy.x = newPosrectText.x();
		//newPosrectTextCopy.y = newPosrectText.y();
		//newPosrectTextCopy.width = newPosrectText.width();
		//newPosrectTextCopy.height = newPosrectText.height();
		//cv::Point2f center{ (newPosrectText.x() + newPosrectText.width()) / 2.0F, (newPosrectText.y() + newPosrectText.height()) / 2.0F };

		//cv::RotatedRect rot = cv::RotatedRect(center, newPosrectTextCopy.size(), selectedAngle);
		//cv::Point2f vertices[4];
		//rot.points(vertices);
		//for (int i = 0; i < 4; i++) {
		//	line(m_image, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0));
		//}
		//m_currentPixmap = QPixmap::fromImage(QImage((uchar*)m_image.data, m_image.cols, m_image.rows, m_image.step1(), QImage::Format_RGB888).copy());
		//m_scenImage->clear();
		//m_scenImage->addPixmap(m_currentPixmap);
		if (m_inputText.is_open()) {
			m_inputText.close();
		}
		m_isGraphicsCalled = true;
		m_points.clear();
	}
}

void ImageShowDemo::onPushRadioButtonBarcode() {
	if (m_isRecognizeChecked) {
		Code_Reader obj;
		obj.setImage(m_image, MODEOFCODE::BARCODE);
		std::string result = obj.showCode();
		std::cout << "\nAngle = " << obj.get_angle() << std::endl;
		if (result.empty()) { 
			result = "\nCould not recognize barcode ";
		}
		else {
			std::cout << "Detected Barcode : " << result << std::endl;
		}

		m_graphicsViewText->setScene(m_scenCode);
		m_scenCode->setSceneRect(0, 0, 41, 41);

		int x = m_scenCode->width() / 2 - 175;
		int y = m_scenCode->height() / 2 - 50;

		QRect posrectBarcode{ x, y, 350, 100 };
		m_scenCode->clear();
		QString text = QString::fromStdString(result);
		QPixmap pixmap = getStringImg(text, posrectBarcode);
		QGraphicsPixmapItem* item1 = new QGraphicsPixmapItem(pixmap);
		item1->setPos(posrectBarcode.x(), posrectBarcode.y());
		m_scenCode->addItem(item1);
		//m_scenImage->addRect(posrectBarcode);
		m_isGraphicsCalled = false;
	}
}

void ImageShowDemo::onPushRadioButtonQrCode() {
	if (m_isRecognizeChecked) {
		Code_Reader obj;
		obj.setImage(m_image, MODEOFCODE::QRCODE);
		std::string result = obj.showCode();
		std::cout << "\nAngle = " << obj.get_angle() << std::endl;
		if (result.empty()) {
			result = "\nCould not recognize QR code ";
		}
		else {
			std::cout << "Detected QR code : " << result << std::endl;
		}

		m_graphicsViewText->setScene(m_scenCode);
		m_scenCode->setSceneRect(0, 0, 41, 41);

		int x = m_scenCode->width() / 2 - 175;
		int y = m_scenCode->height() / 2 - 50;

		QRect posrectQrCode{ x, y, 350, 100 };
		m_scenCode->clear();
		QString text = QString::fromStdString(result);
		QPixmap pixmap = getStringImg(text, posrectQrCode);
		QGraphicsPixmapItem* item1 = new QGraphicsPixmapItem(pixmap);
		
		item1->setPos(posrectQrCode.x(), posrectQrCode.y());
		m_scenCode->addItem(item1);
		m_isGraphicsCalled = false;
	}
}

void ImageShowDemo::onPushAction_RIL_TEXTLINE() {
	m_pushMenuBarSelectMode->title().clear();
	m_pushMenuBarSelectMode->setTitle("Textline");
	m_mode = tesseract::RIL_TEXTLINE;
	m_isRecognizeChecked = true;
	m_isGraphicsCalled = false;
	m_isModeChecked = true;
}

void ImageShowDemo::onPushAction_RIL_WORD() {
	m_pushMenuBarSelectMode->title().clear();
	m_pushMenuBarSelectMode->setTitle("Word");
	m_mode = tesseract::RIL_WORD;
	m_isRecognizeChecked = true;
	m_isGraphicsCalled = false;
	m_isModeChecked = true;
}

void ImageShowDemo::onPushAction_RIL_BLOCK() {
	m_pushMenuBarSelectMode->title().clear();
	m_pushMenuBarSelectMode->setTitle("Block");
	m_mode = tesseract::RIL_BLOCK;
	m_isRecognizeChecked = true;
	m_isGraphicsCalled = false;
	m_isModeChecked = true;
}

void ImageShowDemo::onPushAction_RIL_SYMBOL() {
	m_pushMenuBarSelectMode->title().clear();
	m_pushMenuBarSelectMode->setTitle("Symbol");
	m_mode = tesseract::RIL_SYMBOL;
	m_isRecognizeChecked = true;
	m_isGraphicsCalled = false;
	m_isModeChecked = true;
}

QPixmap ImageShowDemo::getStringImg(const QString& str, const QRect& rect)
{
	QString qStr = str;
	QStringList qStrList = qStr.split("\n");
	int maxLength = qStrList.at(0).length();
	for (int i = 0; i < qStrList.size(); ++i) {
		if (qStrList.at(i).length() > maxLength) {
			maxLength = qStrList.at(i).length();
		}
	}

	QFont font("times", rect.width() / maxLength);
	QPixmap pixmap(rect.width() +4, rect.height()+4 );
	pixmap.fill(QColor(255, 255, 255, 255));
	QPainter painter(&pixmap);
	QPen pen(QColor(255, 0, 0, 255));
	painter.setPen(pen);
	painter.setFont(font);
	painter.drawText(pixmap.rect(), Qt::AlignHCenter, qStr, &pixmap.rect());

	return pixmap;
}

AngleAndRects ImageShowDemo::getAngle() {
	AngleAndRects angleOfRect;
	double angle{};
	VecVecPoint piecesOfText = getPieceOfText(m_image.clone());
	
	cv::Point maxYPoint;
	cv::Point minXPoint;
	cv::Point maxXPoint;
	unsigned indexOfMaxYPoint{};
	unsigned countOfMaxXPoint{};
	for (int k = 0; k < piecesOfText.size(); ++k) {
		for (int i = 0; i < piecesOfText[k].size(); ++i) {
			if (piecesOfText[k][0].x < piecesOfText[k][i].x) {
				++countOfMaxXPoint;
			}
		}
		if (countOfMaxXPoint == 1) {
			minXPoint = piecesOfText[k][piecesOfText[k].size() - 2];
			maxXPoint = piecesOfText[k][piecesOfText[k].size() - 1];
		}
		else if (countOfMaxXPoint == 2) {
			minXPoint = piecesOfText[k][piecesOfText[k].size() - 3];
			maxXPoint = piecesOfText[k][piecesOfText[k].size() - 2];
		}
		countOfMaxXPoint = 0;
		
		maxYPoint = piecesOfText[k][piecesOfText[k].size() - 1];
		for (int i = 0; i < (piecesOfText[k].size() - 1); ++i) {
			if (maxYPoint.y < piecesOfText[k][i].y) {
				maxYPoint = piecesOfText[k][i];
				indexOfMaxYPoint = i;
			}
		}
		if (abs(piecesOfText[k][indexOfMaxYPoint + 1].x - piecesOfText[k][indexOfMaxYPoint].x) < 2
		||  abs(piecesOfText[k][indexOfMaxYPoint + 1].y - piecesOfText[k][indexOfMaxYPoint].y) < 2) {
			angle = 0;
			angleOfRect.push_back({ angle, piecesOfText });
		}
		else {
			double a = maxXPoint.x - minXPoint.x;
			double b = maxXPoint.y - minXPoint.y;
			
			angle = (b / a) * (180 / PI);
			if (a && angle < 360) {
				angleOfRect.push_back({ angle, piecesOfText });
			}
		}
	}
		return angleOfRect;
}

VecVecPoint ImageShowDemo::getPieceOfText(cv::Mat inputImg) {
	VecVecPoint pointsOfRects;

	cv::Mat rgb = inputImg.clone();
	// downsample and use it for processing
	cv::pyrDown(inputImg, rgb);
	cv::pyrDown(rgb, rgb);
	cv::Mat small;
	cv::cvtColor(rgb, small, CV_BGR2GRAY);
	// morphological gradient
	cv::Mat grad;
	cv::Mat morphKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
	
	cv::morphologyEx(small, grad, cv::MORPH_GRADIENT, morphKernel);
	
	// binarize
	cv::Mat bw;
	cv::threshold(grad, bw, 0.0, 255.0, CV_THRESH_BINARY | CV_THRESH_OTSU);
	
	// connect horizontally oriented regions
	cv::Mat connected;
	morphKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 1));
	
	cv::morphologyEx(bw, connected, cv::MORPH_CLOSE, morphKernel);
	
	// find contours
	cv::Mat mask = cv::Mat::zeros(bw.size(), CV_8UC1);
	
	VecVecPoint contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	// filter contours
	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0]) {
		cv::Rect rect = cv::boundingRect(contours[idx]);
		cv::Mat maskROI(mask, rect);
		
		maskROI = cv::Scalar(0, 0, 0);
		// fill the contour
		cv::drawContours(mask, contours, idx, cv::Scalar(255, 255, 255), CV_FILLED);
		
		cv::RotatedRect rrect = cv::minAreaRect(contours[idx]);
		double r = (double)countNonZero(maskROI) / (rrect.size.width * rrect.size.height);

		cv::Scalar color;
		int thickness = 1;
		// assume at least 25% of the area is filled if it contains text
		if (r > 0.25 &&
			(rrect.size.height > 8 && rrect.size.width > 8) // constraints on region size
															// these two conditions alone are not very robust. better to use something 
															//like the number of significant peaks in a horizontal projection as a third condition
			) {
			thickness = 2;
			color = cv::Scalar(0, 255, 0);
		}
		else
		{
			thickness = 1;
			color = cv::Scalar(0, 0, 255);
		}

		cv::Point2f pts[4];
		rrect.points(pts);
		std::vector<cv::Point> temprory;
		for (int i = 0; i < 4; i++)
		{
			line(rgb, cv::Point((int)pts[i].x, (int)pts[i].y), cv::Point((int)pts[(i + 1) % 4].x, (int)pts[(i + 1) % 4].y), color, thickness);
			temprory.push_back(cv::Point((int)pts[i].x, (int)pts[i].y));
		}
		pointsOfRects.push_back(temprory);
	}
	return pointsOfRects;
}

void ImageShowDemo::resizeImageForRecognition() {
	std::vector<std::pair<unsigned, double>> combine;
	combine.push_back({ 1000,1.2 });
	combine.push_back({ 1500,1.7 });
	combine.push_back({ 2000,2.2 });
	combine.push_back({ 2500,2.7 });
	combine.push_back({ 3000,3.2 });
	combine.push_back({ 3500,3.7 });
	combine.push_back({ 4000,4.2 });
	combine.push_back({ 4500,4.7 });
	for (int i = 0; i < combine.size(); ++i) {
		if (m_image.rows - combine[i].first < 500 || m_image.cols - combine[i].first < 500) {
			if (abs(m_image.cols - m_image.rows) < 1000) {
				cv::resize(m_image, m_image, cv::Size(m_image.cols / combine[i].second, m_image.rows / combine[i].second));
				break;
			}
		}
	}
}
