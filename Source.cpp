#include <iostream>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <thread>

using namespace cv;
using namespace cv::cuda;
using namespace std;

//マウス入力用のパラメータ
struct mouseParam {
	Mat img;
	Mat img_zoom;
	float scale;
	int sx;
	int sy;
	bool isZoomed;
	Rect rect;
	int event;
	int flags;
	vector<Point2f> pt;
};

void CallBackFunc2(int eventType, int x, int y, int flags, void* userdata)
{
	mouseParam *mp = static_cast<mouseParam*> (userdata);

	if (eventType == EVENT_LBUTTONDOWN) {
		if (mp->isZoomed) {
			Rect rect = mp->rect;
			float xx = rect.x + x / mp->scale;
			float yy = rect.y + y / mp->scale;
			mp->pt.push_back(Point2f(xx, yy));
			drawMarker(mp->img_zoom, Point(x, y), Scalar(0, 0, 0), MARKER_CROSS, 10, 1);
			cout << "zoom " << xx << ", " << yy << endl;
		}
	}
	if (eventType == EVENT_RBUTTONDOWN) {
		if (mp->pt.size() > 0)
			mp->pt.pop_back();
	}
}

void CallBackFunc(int eventType, int x, int y, int flags, void* userdata)
{
	mouseParam *mp = static_cast<mouseParam*> (userdata);

	if (eventType == EVENT_LBUTTONDOWN) {
		cout << "not zoom " << x << ", " << y << endl;
	}
	if (eventType == EVENT_RBUTTONDOWN) {
		if (!mp->isZoomed) {
			int sx = max(x - 50, 0);
			int sy = max(y - 50, 0);
			int ex = min(x + 49, mp->img.cols - 1);
			int ey = min(y + 49, mp->img.rows - 1);
			Rect rect = Rect(sx, sy, ex - sx, ey - sy);
			mp->rect = rect;
			mp->img_zoom = mp->img.clone()(rect);
			cv::resize(mp->img_zoom, mp->img_zoom, Size(0, 0), mp->scale, mp->scale);
			
			mp->isZoomed = true;
		}
		else {
			mp->img_zoom.release();
			mp->img_zoom = mp->img;
			mp->isZoomed = false;
		}

	}

	if (eventType == EVENT_FLAG_RBUTTON) {

	}

	if (eventType == EVENT_RBUTTONUP) {
		int rect_x = min(mp->sx, x);
		int rect_y = min(mp->sy, y);
		int rect_w = abs(mp->sx - x);
		int rect_h = abs(mp->sy - y);
		Rect rect = Rect(rect_x, rect_y, rect_w, rect_h);
		//cout << x << "," << y << endl;
		//mp->img_zoom = mp->img_zoom(rect);
		//mp->rect = rect;
	}
}

int main()
{
	int k;
	mouseParam mouseEvent;

	Mat img;
	img = imread("D:/img1.jpg");
	//表示するウィンドウ名
	String name = "img";
	String name_zoom = "zoom";

	//画像の表示
	imshow(name, img);
	imshow(name_zoom, img);

	//コールバックの設定
	cv::setMouseCallback(name, CallBackFunc, &mouseEvent);
	cv::setMouseCallback(name_zoom, CallBackFunc2, &mouseEvent);
	mouseEvent.scale = 5;
	mouseEvent.sx = 0;
	mouseEvent.sy = 0;
	mouseEvent.isZoomed = false;
	mouseEvent.img = img.clone();
	mouseEvent.img_zoom = img;
	
	while (1) {
		k = waitKey(10);
		imshow(name, mouseEvent.img);
		imshow(name_zoom, mouseEvent.img_zoom);
	}
	return 0;
}