#include<stdio.h>
#include<iostream>
#include<opencv2/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/imgcodecs.hpp>

using namespace cv;
using namespace std;

Mat frame;
Point p;
Rect rect;
vector<Rect> rectArray;

bool drawing = false;
int blur_degree = 5;

void mosaicFilter(Mat frame) {

    for (int i = 0; i < rectArray.size(); i++) {
        Rect rec = rectArray[i];
        Point p0(rec.x, rec.y);
        Point p1(rec.x + rec.width, rec.y);
        Point p2(rec.x + rec.width, rec.y + rec.height);
        Point p3(rec.x, rec.y + rec.height);

        int b = 0; int g = 0; int r = 0;
        int totalPixels = blur_degree * blur_degree;
       
        int totalrows = (p2.x - p0.x) / blur_degree;
        int totalcols = (p2.y - p0.y) / blur_degree;

        for (int m = 0; m <= totalrows; m++) {
            for (int n = 0; n <= totalcols; n++) {
                int x1 = p0.x + m * blur_degree;
                int y1 = p0.y + n * blur_degree;

                for (int y = y1; y < y1 + blur_degree; y++)
                {
                    for (int x = x1; x < x1 + blur_degree; x++) {
                        r += frame.ptr<uchar>(y)[3 * x + 0];
                        g += frame.ptr<uchar>(y)[3 * x + 1];
                        b += frame.ptr<uchar>(y)[3 * x + 2];
                    }
                }

                int bAvg = b / totalPixels;
                int gAvg = g / totalPixels;
                int rAvg = r / totalPixels;

                b = 0; g = 0; r = 0;

                for (int y = y1; y < y1 + (blur_degree) && y <= p2.y; y++)
                {
                    for (int x = x1; x < x1 + (blur_degree) && x <= p2.y; x++) {
                        frame.ptr<uchar>(y)[3 * x + 0] = rAvg;
                        frame.ptr<uchar>(y)[3 * x + 1] = gAvg;
                        frame.ptr<uchar>(y)[3 * x + 2] = bAvg;
                    }
                }
            }
        }
    }
}

void onMouseDrag(int event, int x, int y, int flags, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        drawing = true;
        p.x = x;
        p.y = y;
        rect.x = x;
        rect.y = y;

        rect.height = 0;
        rect.width = 0;
    }
    else if (event == EVENT_MOUSEMOVE && EVENT_FLAG_LBUTTON) { 
        if (drawing) {
            if (x < p.x) 
            {
                rect.x = x;
                rect.width = abs(x - p.x);
            }
            else {
                rect.width = abs(p.x - x);
            }
            if (y < p.y) {
                rect.y = y;
                rect.height = abs(y - p.y);
            }
            else {
                rect.height = abs(p.y - y);
            }
        }
    }
    else if (event == EVENT_LBUTTONUP) {
        drawing = false;
        rectArray.push_back(rect);
    }
}

int main(int argc, char** argv)
{
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        printf("Camera isn't Opened.");
        return -1;
    }

    namedWindow("frame", 1);
    setMouseCallback("frame", onMouseDrag, NULL);

    while (1) {
        cap.read(frame);
        for (int i = 0; i < rectArray.size(); i++)
        {
            Rect r = rectArray[i];
            rectangle(frame, r, Scalar(0, 0, 255), 1, 8);           
        }
        mosaicFilter(frame);

        imshow("frame", frame);

        uchar c = waitKey(1);

        if (c == 'i' || c == 'I') {
            printf("%d",blur_degree);
            if (blur_degree < 5) {
                blur_degree++;
            }
            else if (blur_degree >= 5) blur_degree = blur_degree + 5;
            else if (blur_degree >= 50) blur_degree = 50;
        }
        else if (c == 'd' || c == 'D') {

            if (blur_degree > 1 && blur_degree<=5) {
                blur_degree--;
            }
            else if (blur_degree >5) blur_degree = blur_degree - 5;
        }
        else if (c == 's' || c == 'S') {       
            imwrite("C:\\Users\\surya\\Downloads\\result.jpg", frame);
        }
        else if (c == 'r' || c == 'R') {
            for (int i = 0; i < rectArray.size(); i++) {
                rectArray[i] = Rect(0, 0, 0, 0);
            }
        }
        if (c == 27) break;
    }
    return 1;
}