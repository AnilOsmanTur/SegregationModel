#include "segmodel.h"
#include <iostream>

using namespace cv;
using namespace std;

//segregation model class function definitions
SegModel::SegModel(int size, int seed, unsigned int wP, unsigned int bP)
{
    matSize = size;
    simSeed = seed;
    srand(simSeed);
    init();
    if(wP + bP > 100){
        unsigned int gap = 100 - ( wP + bP );
        int i = rand()%2;
        do{
            if(i){
                wP--;
            }else{
                bP--;
            }
            i = (i + 1)%2;
            gap--;
        }while(gap);
    }
    whiteProbTresh = wP;
    blackProbTresh = bP + wP;
    fill2DSimMatrix(SimSurface);

    makeImage(SimSurfaceImg);
    showSimResult();

}

SegModel::~SegModel()
{
    destroy2Dmatrix(SimSurface);
    destroy2Dmatrix(SimChanges);
    destroyWindow(winName);
}

int** SegModel::create2DSimMatrix(void)
{
     // allocating memory
    int** p;
    p = new int*[matSize];
    for(int i = 0; i < matSize; ++i){
        p[i] = new int[matSize];
    }

    return p;
}

/*
 * 0's are empty spaces
 * 1's are whites
 * 2's are blacks
 */
void SegModel::fill2DSimMatrix(int **p)
{
    unsigned int act = 0;
    for( int i = 0; i < matSize; ++i ){
        for( int j = 0; j < matSize; ++j ){
            act = rand()%101;
            if(act <= whiteProbTresh){
                p[i][j] = 1;
            }else if(act <= blackProbTresh){
                p[i][j] = 2;
            }else{
                p[i][j] = 0;
            }

        }
    }

}

// initailization of matrises
void SegModel::init()
{
    SimSurface = create2DSimMatrix();
    SimChanges = create2DSimMatrix();
    SimSurfaceImg = Mat::zeros( matSize, matSize, CV_8UC3);
    namedWindow( winName, WINDOW_NORMAL );
    resizeWindow( winName, 400, 400);
    moveWindow( winName, 100, 200);
}

void SegModel::destroy2Dmatrix(int** &p)
{
    //freeing memory
    for( int i = matSize-1; i >= 0; --i){
        delete[] p[i];
    }
    delete[] p;
}

// data copying between matrises
void SegModel::copyMatrix(int **dest, int **src )
{
    // move data matrix to matrix
    int i, j;
    for (i=0; i < matSize; ++i){
        for(j=0; j < matSize; ++j){
            dest[i][j] = src[i][j];
        }
    }
}

// visualization part
void SegModel::makeImage( Mat& img )
{
    uchar* p;
    for(int i = 0; i < matSize; ++i){
        p = img.ptr(i);
        for(int j = 0; j < matSize; j++){
            uchar* uc_pixel = p;
            if(SimSurface[i][j] == 2) { // red
                uc_pixel[0] = 0;
                uc_pixel[1] = 0;
                uc_pixel[2] = 200;
            } else  if (SimSurface[i][j] == 0){ // blue
                uc_pixel[0] = 200;
                uc_pixel[1] = 0;
                uc_pixel[2] = 0;
            } else { // SimSurface[i][j] == 1  green
                uc_pixel[0] = 0;
                uc_pixel[1] = 200;
                uc_pixel[2] = 0;
            }
            p += 3;
        }
     }
}

void SegModel::showSimResult()
{
    imshow(winName, SimSurfaceImg);
    waitKey(200);
}

// Segregation Model rules definitions
float SegModel::satisfaction(int x, int y, int race)
{
    // 8 neighborhood counting
    // count the neigbors of that coordinates and return it.
    float sat=0.0;
    int i,j;
    int sameRaceCount=0, diffRaceCount=0, emptiness=0;
    int xStart, xEnd, yStart, yEnd;
    //cout<<"location x:"<< x <<" y:"<< y <<endl;
    // size generating
    if (x == 0){
        xStart = matSize-1;
    }else{
        xStart = x - 1;
    }
    xEnd = (x + 1)%matSize;
    if (y == 0){
        yStart = matSize-1;
    }else{
        yStart = y - 1;
    }
    yEnd = (y + 1)%matSize;
    // size generating

    // counting
    for(i = xStart; i <= xEnd; ++i){
        i = i%matSize;
        for(j = yStart; j <= yEnd; ++j){
            j = j%matSize;
            if(SimSurface[i][j] == race){
                sameRaceCount++;
            }else if(SimSurface[i][j] != 0){
                diffRaceCount++;
            }else {
                emptiness++;
            }
            //cout<<"!! location i:"<< i <<" j:"<< j <<endl;
        }
    }

    if (sameRaceCount > diffRaceCount){
        if(diffRaceCount > 0){
            sat = (float)sameRaceCount / (float)diffRaceCount;
        }else if(emptiness > 0){
            sat = (float)sameRaceCount / (float)emptiness;
        }else
            sat = 1.0;
    } else if (sameRaceCount < diffRaceCount){
        sat = (float)sameRaceCount / (float)diffRaceCount;
    } else {
        if(emptiness > 0){
            sat = (float)sameRaceCount / (float)emptiness;
        }else
            sat = (float)sameRaceCount / (float)diffRaceCount;
    }
    //cout<<"satisfaction: "<< sat <<endl;
    return sat;
}

void SegModel::moveOrStay(int i, int j)
{
    int x,y, tryMove=0;
    float sat = satisfaction(i,j, SimSurface[i][j]);
    if( sat < 0.5 ){
        //try to move somewhere
        x = rand()%matSize;
        y = rand()%matSize;
        do {
            do {
                tryMove++;
                if(!SimSurface[x][y]){
                    SimChanges[x][y] = SimSurface[i][j];
                    SimChanges[i][j] = SimSurface[x][y];
                    tryMove=0;
                }else{
                    if( sat > satisfaction(x,y, SimSurface[x][y]) ){
                        SimChanges[x][y] = SimSurface[i][j];
                        SimChanges[i][j] = SimSurface[x][y];
                        tryMove=0;
                    }
                }
                x = (x + 1) % matSize;
            }while(tryMove);
            y = (y + 1) % matSize;
            //cout<<"trying to move x:"<< x <<" y:"<< y <<endl;
        }while(tryMove);
    }else if( sat > 0.5){
        SimChanges[i][j] = SimSurface[i][j];
        //cout<<"not moving"<<endl;
    }else{
        //try to move once to a empty space
        x = rand()%matSize;
        y = rand()%matSize;
        if(SimSurface[x][y] == 0){
            SimChanges[x][y] = SimSurface[i][j];
            SimChanges[i][j] = SimSurface[x][y];
        }else{
            SimChanges[i][j] = SimSurface[i][j];
        }
    }
}

void SegModel::simCycle()
{
    //cout<<"just before Cycle"<<endl;
    // pass through the matrix and make changes in  the other matrix
    copyMatrix(SimChanges, SimSurface);
    for(int i = 0; i < matSize; ++i) {
        for (int j = 0; j < matSize; ++j) {
            if(SimSurface[i][j])
                moveOrStay(i,j);
        }
    }
    //cout<<"just before matris copy"<<endl;
    copyMatrix(SimSurface, SimChanges);
    //cout<<"just before imaging"<<endl;
    makeImage(SimSurfaceImg);
    showSimResult();
    //cout<<"window created"<<endl;
}

void SegModel::simNCycle(int n)
{
    for (int i = 0; i < n; ++i){
        cout <<i <<". "<<"Cycle"<<endl;
        simCycle();
    }
    waitKey(0);
}
