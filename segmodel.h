/*
 * Anıl Osman TUR - 2017
 * 17812602
 * Ankara University - Computer Engineering Master
 * Complex Networks - Thomas Shelling - Segregtion Model
 *
 */

#ifndef SEGMODEL_H
#define SEGMODEL_H

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv/cv.h"


class SegModel
{
    int matSize = 0, simSeed = 1;
    int **SimSurface = NULL;
    int **SimChanges = NULL;
    unsigned int whiteProbTresh = 0, blackProbTresh = 0;
    cv::Mat SimSurfaceImg;
    const std::string winName = "SegModel";

    // some helper functions
    int** create2DSimMatrix(void); //dinamic memory alocation !!note: call after initializing the size
    void fill2DSimMatrix(int **p);
    void init(); // initialization function
    void destroy2Dmatrix(int** &p); // memory freeing
    void copyMatrix(int **dest, int **src );

    // model decision part
    float satisfaction(int x, int y, int race);
    void moveOrStay(int i, int j);

    // visualizaition part
    void makeImage(cv::Mat& img);
    void showSimResult();

public:
    SegModel(int size, int seed, unsigned int wP, unsigned int bP);
    ~SegModel();

    void simCycle();
    void simNCycle(int n);

};

#endif // SEGMODEL_H
