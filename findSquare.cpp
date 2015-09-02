#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stack>

using namespace cv;
using namespace std;

vector<Point2f> circles;

void findCircles(Mat image){

    printf("Allg at findcircles start\n");
    Mat temp;
    image.copyTo(temp);
    printf("Allg after copy\n");
    stack<Point2i> pixelstack;
    
    for(int i = 0; i < temp.rows; i++){
        for(int j = 0; j < temp.cols; j++){
            if(temp.at<int>(i,j) == 0){
                pixelstack.push(Point2i(i,j));
                printf("Allg in if push\n");
                int rowsum = 0, colsum = 0, count = 0;
                while(!pixelstack.empty()){
                    printf("Allg at while start\n");
                    Point2i curr = pixelstack.top();
                    printf("Allg after top\n");
                    pixelstack.pop();
                    printf("Allg after pop\n");
                    if(temp.at<int>(curr.x,curr.y) == 0){
                        count++;
                        printf("Allg in if\n");
                        temp.at<int>(curr.x,curr.y) = 255;
                        rowsum += curr.x;
                        colsum += curr.y;
                        if(curr.y > 0)
                        pixelstack.push(Point2i(curr.x, curr.y-1));
                        if(curr.x > 0)
                        pixelstack.push(Point2i(curr.x-1, curr.y));
                        if(curr.y < temp.cols - 1)
                        pixelstack.push(Point2i(curr.x, curr.y+1));
                        if(curr.x < temp.rows - 1)
                        pixelstack.push(Point2i(curr.x+1, curr.y));
                    }
                }
                printf("Allg before circle push\n");
                circles.push_back(Point2f((rowsum/count), (colsum/count))); //removing casts did not fix
                printf("Allg after circle push\n");
                rowsum = colsum = count = 0;
            }
        }
    }
    imshow("", temp);
    waitKey(0);
}
            


int main( int argc, char** argv )
{
    if( argc != 2)
        {
            cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
            return -1;
        }

    Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
        {
            cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }

    cvtColor(image,image, CV_RGB2GRAY);
    //GaussianBlur(image,image, Size(9, 9), 2, 2 );
    threshold(image, image, 50, 255, THRESH_BINARY);
     findCircles(image);

     for(int i = 0; i < circles.size(); i++){
        Point center(cvRound(circles[i].x), cvRound(circles[i].y));
        circle(image, center, 3, Scalar(255,0,0), -1, 8, 0);
        }
    //namedWindow( "", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "", image );                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}
