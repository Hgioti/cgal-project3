#include <iostream>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include "includes/Incremental.h"
#include "includes/ConvexHull.h"
#include "includes/Polygon.h"


using namespace std;



void ReadFiles(const char *basePath,vector<string>& vect)
{
    
    char *path=(char *)malloc(256);
    struct dirent *dp;
    DIR *dir = opendir(basePath);
    struct stat sd;
    

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
        
                strcpy(path, basePath);
                strcat(path, "/");
                strcat(path, dp->d_name);

                if (stat(path, &sd) == -1) {
                                    perror("stat");
                                    exit(EXIT_FAILURE);         
                }                  

                    /*  Tsekarw an einai arxeio kai to bazw sthn oura   */
                        if((sd.st_mode & S_IFMT)==S_IFREG)
                            vect.push_back(path);
                    
                        ReadFiles(path,vect);              
        }
    }

    closedir(dir);
    free(path);
}


int main(int argc, char** argv)
{
    //Read arguments from user and check them
    string inputFileName,outputFileName,edgeSelection,algorithm,sort,globalopt,annealing;
    int L=0;
    double thres = 0;



    if( (strcmp(argv[1],"-i") != 0)  || (strcmp(argv[3],"-o") != 0) || (strcmp(argv[5],"-preprocess") != 0)  ){
        cout << "Wrong input" << endl;
        return -1;
    }

    if((argc < 1) || (argc > 11)){
        cout << "Wrong number of arguements" << endl;
        return -1;
    }

    FILE *fp;
    fp = fopen(argv[2], "r");
    if( fp == NULL ) {
       perror("Error: ");
       return(-1);
    }
    fclose(fp);


    const char* str = argv[2];
    outputFileName = argv[4];


    if( argc > 7){
        if ((strcmp(argv[6],"-L") == 0))
            L = atoi(argv[7]);
        else if
            (strcmp(argv[6],"-threshold") == 0)
                thres = atoi(argv[7]);
    }
    else{
         L=10;
         thres = 300000;
    }

    if   (argc > 9){
           if ((strcmp(argv[8],"-L") == 0))
                L = atoi(argv[9]);
            else if
                (strcmp(argv[8],"-threshold") == 0)
                thres = atoi(argv[9]);
    }
    else{
        if(L == 0)
            L=10;
        else
         thres = 300000;
    }


    cout << "L is " << L << ", threshold is " << thres << endl;

    if((L > 10) || (L < 0)){
        cout << "Wrong input" << endl;
        return -1;
    }


    cout << endl << "Adding  ..." << endl << endl;

    
    vector<string> vect; 

    ReadFiles(str,vect);
    std::sort( vect.begin(), vect.end() );
    for (int i = 0; i < vect.size(); i++)
            cout << vect[i] << endl;

    cout << endl << "Writing to file ..." << endl;
    
    //Print information
    ofstream myfile;
    myfile.open (outputFileName);

    myfile << "                         Incremental Algorithm                          " << endl;
    myfile << "Size   ||  min score  |   max score   |   min bound   |   max bound   ||" << endl;

    myfile.close();
    algorithm = "incremental";
    sort = "1a";
    globalopt = "local_search";

        for (int i = 0; i < vect.size(); i++){

            inputFileName = vect[i];
            Polygon *polygon = NULL;
            if (algorithm == "incremental")
                polygon = new Incremental(inputFileName, sort);

            
            //Calculate result
            polygon->calculateResult();
            polygon->getRatio();
            
            if(globalopt == "local_search"){

                        polygon->init();                  
                        polygon->LocalSearchMax(L,thres);
                        polygon->LocalSearchMin(L,thres);
            }

            polygon->printInfo(outputFileName,globalopt);

            if (polygon)
                delete polygon;
        }
    return 0;
}










