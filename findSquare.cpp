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

    cout << "in findBlobs" << endl;
    Mat temp;
    image.copyTo(temp);
    cout << "about to enter for" << endl;
    for(int row = 0; row < temp.rows; row++){
        for(int col = 0; col < temp.cols; col++){
            if(temp.at<int>(row,col) == 0){
                cout << "in if" << endl;
                bool blob_exists = false;
                for(int k = 0; k < blobs.size(); k++){
                    cout << "iterating over blob vector" << endl;
                    if(row >= blobs[k].topleft.x && row <= blobs[k].botright.x &&
                       col >= blobs[k].topleft.y && col <= blobs[k].botright.y){
                        cout << "added to existing blob" << endl;
                        blobs[k].row += row;
                        blobs[k].col += col;
                        blobs[k].count++;
                        blob_exists = true;
                        break;
                    } 
                }

                if(!blob_exists){

                    if(row - 10 < 0 || col - 75 < 0 ||
                       row + 210 > res_y || col + 125 > res_x){
                        return -1;
                    }
                    
                    cout << "creating new blob" << endl;
                    blobs.push_back(blob());
                    int index = blobs.size()-1;
                    
                    blobs[index].topleft = Point2i(row - 10, col - 75);
                    blobs[index].botright = Point2i(row + 210, col + 125);
                    blobs[index].row = row;
                    blobs[index].col = col;
                    blobs[index].count = 1;
                    cout << "successfully created new blob" << endl;
                    
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

    Mat readin, image ;
    readin = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);   // Read the file
    readin.convertTo(image, CV_8U);
    if(! image.data )                              // Check for invalid input
        {
            cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }

    //cvtColor(image,image, CV_RGB2GRAY);
    //GaussianBlur(image,image, Size(9, 9), 2, 2 );
    /*int pixel;
    for(int i = 0; i < image.rows; i++){
        for(int j = 0; j < image.cols; j++){
            //cout << "before pixel assignment" << endl;
            if(i != 1078 && j != 1724)
            pixel = image.at<int>(i,j);
            //  printf("i = %d, j = %d\n", i,j);
            if(pixel < 0 || pixel > 50){
                // cout << "before if" << endl;
                image.at<int>(i,j) = 255;
            } else {
                // cout<< "before else" << endl;
                image.at<int>(i,j) = 0;
            }
        }
        }*/

    // threshold(image, image, 50, 255, THRESH_BINARY);
    /*for(int i = 0; i < image.rows; i++){
        for(int j = 0; j < image.cols; j++){
            if(image.at<int>(i,j) != 0 && image.at<int>(i,j) != 255)
                cout << image.at<int>(i,j) << endl;
        }
        }*/
    //imshow( "", image );
    // waitKey(0);
    
     findBlobs(image);

    cout << "before cirlce drawing" << endl;
     for(int i = 0; i < blobs.size(); i++){
        int x = blobs[i].col/blobs[i].count;
        int y = blobs[i].row/blobs[i].count;
        cout << "circle " << i << " x = " << x << endl;
        cout << "circle " << i << " y = " << y << endl;
      Point center(cvRound(x), cvRound(y));
      circle(image, center, 3, Scalar(255,0,0), -1, 8, 0);
      }
      //namedWindow( "", WINDOW_AUTOSIZE );// Create a window for display.
     imshow( "", image );                   // Show our image inside it.

     waitKey(0); // Wait for a keystroke in the window

      //I dont think i have freed this memory
      /* for(int i = 0; i < blobs.size(); i++){
          blob temp = blobs[i];
          blobs.erase(i);
          delete temp;
          }*/ 
    return 0;
}
