#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv/cv.h"
#include "segmodel.h"

using namespace std;

int main()
{
    SegModel simModel(100, 5, 20, 30);

    cout << "Segregation Demo! \nPress any key:" << endl;
    getchar();
    cout << "\nlet's start." << endl;
    simModel.simNCycle(100);
    cout << "wuf" << endl;
    return 0;
}
