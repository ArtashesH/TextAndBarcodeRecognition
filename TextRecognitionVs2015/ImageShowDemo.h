#ifndef IMAGESHOWDEMO_H
#define IMAGESHOWDEMO_H

#include <QWidget>
#include <QRadioButton>
#include <QMenuBar>
#include <opencv2/opencv.hpp>

#include "textregionextrator.h"
#include "ReadCode.h"

#include <fstream>

enum class WhichButton { OPEN_IMAGE, OPEN_VIDEO, OPEN_CAMERA };
#define PI 3.14159265
using VecVecPoint = std::vector<std::vector<cv::Point>>;
using StartRec = TextRegionExtractor::RectTextInfo;
using AngleAndRects = std::vector<std::pair<double, VecVecPoint>>;

class QGraphicsView;
class QGraphicsScene;
class QPixmap;
class QTimer;
class QPushButton;
class QGraphicsRectItem;

class ImageShowDemo : public QWidget
{
	Q_OBJECT

public:
	explicit ImageShowDemo(QWidget *parent = 0);
	void closeEvent(QCloseEvent*) override;
	bool isApplicationclose();

	public slots:
	void testImageCapture();
	void onPushButtonRecognize();
	void onPushbuttonOpenImage();
	void onPushbuttonOpenTextFile();
	void onPushbuttonOpenVideo();
	void onPushbuttonOpenCamera();
	void onPushbuttonOpenStart_Stop();
	void onPushRadioButtonTesseract();
	void onPushRadioButtonBarcode();
	void onPushRadioButtonQrCode();
	void onPushAction_RIL_TEXTLINE();
	void onPushAction_RIL_WORD();
	void onPushAction_RIL_BLOCK();
	void onPushAction_RIL_SYMBOL();

private:
	void createMembers();
	void setupLayout();
	void makeConnections();
	QPixmap getStringImg(const QString&, const QRect&);
	cv::Mat rotateImageWithAngle(const double, const cv::Mat&);
	AngleAndRects getAngle();
	VecVecPoint getPieceOfText(cv::Mat);
	void resizeImageForRecognition();

private:
	QGraphicsView*       m_graphicsViewImage;
	QGraphicsView*       m_graphicsViewText;
	QGraphicsScene*      m_scenImage;
	QGraphicsScene*      m_scenText;
	QGraphicsScene*	     m_scenCode;
	QGraphicsRectItem*   m_graphRecat;
	QPixmap	             m_PixMap;
	QTimer*			     m_timer;
	QPushButton*	     m_pushButtonRecognize;
	QPushButton*	     m_pushButtonOpenImage;
	QPushButton*         m_pushButtonOpenTextFile;
	QPushButton*         m_pushButtonOpenVideo;
	QPushButton*         m_pushButtonOpenCamera;
	QPushButton*         m_pushButtonStart_Stop;
	QRadioButton*	     m_pushRadioButtonTesseract;
	QRadioButton*	     m_pushRadioButtonBarcode;
	QRadioButton*	     m_pushRadioButtonQrCode;

	QAction*			 m_pushAction_RIL_TEXTLINE;
	QAction*			 m_pushAction_RIL_BLOCK;
	QAction*			 m_pushAction_RIL_WORD;
	QAction*			 m_pushAction_RIL_SYMBOL;
	QMenuBar*			 m_pushMenuBarSelectModeBar;
	QMenu*				 m_pushMenuBarSelectMode;

	QString              m_imgFilename;
	QString              m_txtFilename;
	QString              m_videoFilename;
	cv::VideoCapture     m_imageCapture;
	std::string          m_imagePath;
	cv::Mat              m_image;
	TextRegionExtractor* m_textRegionExtractor;

	std::ofstream        m_inputText;
	std::string          m_textFilePath;
	QPixmap				 m_currentPixmap;

	bool                 m_isRecognizeChecked;
	bool                 m_isGraphicsCalled;
	bool                 m_isStartCalled;
	bool				 m_isModeChecked;
	WhichButton			 m_whichButton;

	tesseract::PageIteratorLevel m_mode;
	std::vector<cv::Rect>		 m_points;

	bool						 m_isCloseApp;
};

#endif // IMAGESHOWDEMO_H
