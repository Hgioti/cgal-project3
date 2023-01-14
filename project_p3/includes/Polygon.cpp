#include <fstream>
#include "Polygon.h"
#include <sstream>
#include <string>
#include <time.h>
#include <chrono>
#include <ctime>
#include <sys/time.h>

Polygon::Polygon(string inputFileName)
{

    std::ifstream infile(inputFileName);
    
    string name;
    int a,b,d;

    string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);

        if(line.at(0) != '#'){

            iss >> d;
            iss >> a >> b;
            initialPoints.push_back(Point_2(a,b));
        }
    }
}

void Polygon::printInfo(string output,string algorithm)
{
    ofstream myfile;
    myfile.open (output,ios::app);
    if(count_max < 5)
        score_max = 1;
    if(count_min < 5)
        score_min = 1;

    if(score_max == 0)
        score_max = 1;
    if(score_min == 0)
        score_min = 1;
    if(max_bound == 10)
        max_bound = 1;
    if(min_bound == 0)
        min_bound = 1;



    myfile << result.size() ;
    if(result.size()<100)
        myfile << "     ";
    else if(result.size()<1000)
        myfile << "    ";
    else if(result.size()<10000)
        myfile << "   ";
    else if(result.size()<100000)
        myfile << "  ";

    myfile << "||  " << score_min;
    if(score_min == 1)
            myfile << "      ";
    myfile << "    |   " << score_max;
    if(score_max == 1)
              myfile << "      ";
    myfile <<"     |   " << min_bound;
    if(min_bound == 1)
        myfile << "       ";
    myfile << "    |   " << max_bound;
    if(max_bound == 1)
        myfile << "       "; 
    myfile << "    ||" << endl;
    myfile.close();
}    





bool edgeVisible(const Segment_2& edge, Point_2 point,Polygon_2 pol)
{
    //Returns true if edge is visible from point:

    Segment_2 s1(point,edge.source());
    Segment_2 s2(point,edge.target());
    Segment_2 s3(point,CGAL::midpoint(edge));


    for(const Segment_2& e  : pol.edges()){

        if(e!=edge){

            //if edge1 DOESN'T intersect with e OR if edge1 intersects with e and edge.source is not common point
            const auto inter1 = intersection(e, s1);
            if (inter1){

                if(   (s1.target() != e.source()) && (s1.target() != e.target())      ){
                    //  std::cout << "intersection at start " << edge.source() << std::endl;
                    return false;
                }
            }

            //if edge2 DOESN'T intersect with e OR if edge2 intersects with e and edge.target is not common point
            const auto inter2 = intersection(e, s2);
            if (inter2){

                if(     (s2.target() != e.source()) && (s2.target() != e.target())   ){
                    //  std::cout  << "intersection at end " << edge.target() << std::endl;
                    return false;
                }
            }

            //if edge3 DOESN'T intersect with e
            const auto inter3 = intersection(e, s3);
            if (inter3){

                //  std::cout << "intersection at mid " << std::endl;
                return false;
            }
        }
    }

    return true;
}

string Polygon::simpleOrNot()
{
    if (result.is_simple())
        return "Simple Polygon";
    else
        return "Not Simple Polygon";
}


void Polygon::LocalSearchMax(int L,double thres)
{

        srand (time(NULL));
        vector<K::Point_2> kvert;
        Polygon_2 local,bestP;

        int skip;

        if(result.size() < 100)
            skip = 1;
        else if(result.size() < 1000)
            skip = result.size()/10;
        else if(result.size() < 2000)
            skip = result.size()/6;
        else if(result.size() < 10000)
            skip = result.size()/5;


        bestP = result;
        double A,bestA = thres + 1;

        //Start timer
        timeval start;
        gettimeofday(&start, NULL);

        while(bestA > thres){

                //Stop timer
                timeval stop;
                gettimeofday(&stop, NULL);
                double elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0 + (stop.tv_usec - start.tv_usec) / 1000.0;
                if(( (int)elapsedTime) > (500*result.size() ))
                    return;

                result = bestP ;
                bestA=0;

                //For every edge e
                for(const Segment_2& e : result.edges()){

                      if(rand()%skip == 0) {

                                //For every path
                                std::vector<Point_2>::iterator iter;
                                for (iter=result.begin(); iter!=result.end(); ++iter){

                                        //For every k
                                        for(int k=L;k>0;k--){

                                                local=result;
                                                kvert.clear();
                                                
                                                //Push k elements into vector
                                                std::vector<Point_2>::iterator lades;
                                                lades=iter;
                                                for(int i=0;i<k;i++){

                                                        if(lades == result.end())
                                                            lades=result.begin();


                                                        if((*lades == e.source()) ||  (*lades == e.target())){

                                                            kvert.clear();
                                                            break;
                                                        }

                                                        kvert.push_back(*lades);                                                
                                                        lades++;
                                                }


                                               if(!kvert.empty()){

                                                        kvert.insert(kvert.begin(),e.target());

                                                        //Erasing selected elements
                                                        std::vector<Point_2>::iterator iter2;
                                                        for (iter2=local.begin(); iter2!=local.end();){

                                                                if( std::find(kvert.begin(), kvert.end(), *iter2) != kvert.end()    )
                                                                    local.erase(iter2);                   
                                                                else
                                                                    iter2++;
                                                        }


                                                        //Inserting the elements in the right order
                                                        iter2 = std::find(local.begin(), local.end(), e.source());
                                                        for(int i = kvert.size() - 1; i >= 0; i--){

                                                                        iter2++;
                                                                        local.insert(iter2,kvert[i]);                                                                  
                                                        }

                                                        //Checking for best solution
                                                        if( local.is_simple() ){

                                                                A = local.area()-result.area();
                                                                if( (  A > thres ) && (A > bestA)   ){                

                                                                            bestP = local;
                                                                            bestA = A;

                                                                            this->getRatio();
                                                                          //  cout << "ratio is " << ratio << endl;
                                                                            if(ratio < max_bound)
                                                                                max_bound = ratio;     

                                                                            score_max += ratio;

                                                                            count_max++;
                                                                            if(count_max == 5)
                                                                                return;

                                                                 }
                                                    }     
                                            }
                                    }
                                }        
                        }
                }
    }

    
    result = original;
}  




void Polygon::LocalSearchMin(int L,double thres)
{

        srand (time(NULL));
        vector<K::Point_2> kvert;
        Polygon_2 local,bestP;

        int skip;


        if(result.size() < 100)
            skip = 1;
        else if(result.size() < 1000)
            skip = result.size()/10;
        else if(result.size() < 2000)
            skip = result.size()/6;
        else if(result.size() < 10000)
            skip = result.size()/5;


        bestP = result;
        double A,bestA = thres - 1;

        timeval start;
        gettimeofday(&start, NULL);

        while(bestA < thres){

                timeval stop;
                gettimeofday(&stop, NULL);
                double elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0 + (stop.tv_usec - start.tv_usec) / 1000.0;
                if(( (int)elapsedTime) > (500*result.size() ))
                    return;
                result = bestP ;

                bestA=thres + 1;

                //For every edge e
                for(const Segment_2& e : result.edges()){

                      if(rand()%skip == 0) {

                                //For every path
                                std::vector<Point_2>::iterator iter;
                                for (iter=result.begin(); iter!=result.end(); ++iter){

                                        //For every k
                                        for(int k=L;k>0;k--){

                                                local=result;
                                                kvert.clear();
                                                
                                                //Push k elements into vector
                                                std::vector<Point_2>::iterator lades;
                                                lades=iter;
                                                for(int i=0;i<k;i++){

                                                        if(lades == result.end())
                                                            lades=result.begin();


                                                        if((*lades == e.source()) ||  (*lades == e.target())){

                                                            kvert.clear();
                                                            break;
                                                        }

                                                        kvert.push_back(*lades);                                                
                                                        lades++;
                                                }


                                               if(!kvert.empty()){

                                                        kvert.insert(kvert.begin(),e.target());

                                                        //Erasing selected elements
                                                        std::vector<Point_2>::iterator iter2;
                                                        for (iter2=local.begin(); iter2!=local.end();){

                                                                if( std::find(kvert.begin(), kvert.end(), *iter2) != kvert.end()    )
                                                                    local.erase(iter2);                   
                                                                else
                                                                    iter2++;
                                                        }


                                                        //Inserting the elements in the right order
                                                        iter2 = std::find(local.begin(), local.end(), e.source());
                                                        for(int i = kvert.size() - 1; i >= 0; i--){

                                                                        iter2++;
                                                                        local.insert(iter2,kvert[i]);                                                                  
                                                        }

                                                        //Checking for best solution
                                                        if( local.is_simple() ){

                                                                A = local.area()-result.area();
                                                                if( (  A > 0 ) && (  A < thres ) && (A < bestA)   ){                

                                                                            bestP = local;
                                                                            bestA = A;

                                                                            this->getRatio();
                                                                           // cout << "ratio is " << ratio << endl;
                                                                            score_min += ratio;
                                                                            if(ratio > min_bound)
                                                                                min_bound = ratio;     

                                                                            count_min++;
                                                                            if(count_min == 5)
                                                                                return;                                                            
                                                                 }
                                                    }     
                                            }
                                    }

                                }        
                        }
                }
    }

    result = original;
}

void Polygon:: getRatio()
{

        area = result.area();
        const Polygon_2::Vertices& range = result.vertices();
        vector<K::Point_2> cvector;

        Polygon_2 chull;
        CGAL::convex_hull_2(range.begin(), range.end(), std::back_inserter(cvector));
        for(auto it = cvector.begin(); it!= cvector.end();++it)
            chull.push_back(*it);
        ratio = (double)(result.area()/chull.area());

}

void Polygon:: init()
{
    score_max=0;
    score_min=0;
    min_bound = 0;
    max_bound = 10;
    count_min = 0;
    count_max = 0;
    original = result;
}