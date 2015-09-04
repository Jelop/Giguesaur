#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stack>

#define res_x 1920
#define res_y 1080

using namespace cv;
using namespace std;

struct blob{
    Point2i topleft;
    Point2i botright;
    int row;
    int col;
    int count;
};

vector<blob> blobs;

int findBlobs(Mat image){

    Mat temp;
    image.copyTo(temp);
    for(int row = 0; row < temp.rows; row++){
        for(int col = 0; col < temp.cols; col++){
            if(temp.at<int>(row,col) == 0){
                for(int k = 0; k < blobs.size(); k++){
                    if(row >= blobs[k].topleft.x && row <= blobs[k].botright.x &&
                       col >= blobs[k].topleft.y && col <= blobs[k].botright.y){
                        blobs[k].row += row;
                        blobs[k].col += col;
                        blobs[k].count++;
                        break;
                    } else if(k == blobs.size() - 1){
                        //blob *blob1 = (blob *)malloc(sizeof (blob));
                        blob *blob1 = new blob;
                        blob1->topleft = Point2i(row - 10, col - 75);
                        blob1->botright = Point2i(row + 210, col + 125);
                        blob1->row = row;
                        blob1->col = col;
                        blob1->count = 1;

                        blobs.push_back(blob1);
                        
                        if(blob1->topleft.x < 0 || blob1->topleft.y < 0 ||
                           blob1->botright.x > res_y || blob1->botright.y > res_x){
                            delete blob1;
                            return -1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}
                        
                      
   
/*void findCircles(Mat image){

    printf("Allg at findcircles start\n");
    Mat temp;
    image.copyTo(temp);
    printf("Allg after copy\n");
    stack<Point2i> pixelstack;

    for(int i = 0; i < temp.rows; i++){
        for(int j = 0; j < temp.cols; j++){
        cout << temp.at<int>(i,j) << endl;
        }
        }
    
    for(int i = 0; i < temp.rows; i++){
        for(int j = 0; j < temp.cols; j++){
            if(temp.at<int>(i,j) == 0){
                pixelstack.push(Point2i(i,j));
                //printf("Allg in if push\n");
                int rowsum = 0, colsum = 0, count = 0;
                while(!pixelstack.empty()){
                    // printf("Allg at while start\n");
                    Point2i curr = pixelstack.top();
                    //printf("Allg after top\n");
                    pixelstack.pop();
                    //printf("Allg after pop\n");
                    if(temp.at<int>(curr.x,curr.y) == 0){

                        count++;
                        //printf("Allg in if\n");
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
                //printf("Allg before circle push\n");
                circles.push_back(Point2f((rowsum/count), (colsum/count))); //removing casts did not fix
                //printf("Allg after circle push\n");
                rowsum = colsum = count = 0;
            }
        }
        }
    imshow("", temp);
    waitKey(0);
}
      */      


int main( int argc, char** argv )
{
    if( argc != 2)
        {
            cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
            return -1;
        }

    Mat image ;
    image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);   // Read the file
    image.convertTo(image, CV_8U);
    if(! image.data )                              // Check for invalid input
        {
            cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }

    //cvtColor(image,image, CV_RGB2GRAY);
    //GaussianBlur(image,image, Size(9, 9), 2, 2 );
    /* for(int i = 0; i < image.rows; i++){
        for(int j = 0; j < image.cols; j++){
            int pixel = image.at<int>(i,j);
            if(pixel < 0){
                image.at<int>(i,j) = 255;
            } else if (pixel > 0) {
                image.at<int>(i,j) = 255;
            } else {
                cout << image.at<int>(i,j) << endl;
            }
        }
        }*/
    threshold(image, image, 50, 255, THRESH_BINARY);
    //imshow( "", image );
    //waitKey(0);
    
    findBlobs(image);

    for(int i = 0; i < blobs.size(); i++){
        int x = blobs[i].col/blobs[i].count;
        int y = blobs[i].row/blobs[i].count;
      Point center(cvRound(x), cvRound(y));
      circle(image, center, 3, Scalar(255,0,0), -1, 8, 0);
      }
      //namedWindow( "", WINDOW_AUTOSIZE );// Create a window for display.
      imshow( "", image );                   // Show our image inside it.

      waitKey(0); // Wait for a keystroke in the window

      for(int i = 0; i < blobs.size(); i++){
          delete blobs[i];
      }
    return 0;
}
