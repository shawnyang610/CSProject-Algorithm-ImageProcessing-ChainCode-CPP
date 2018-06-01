//Project 6
//Shawn Yang


#include <iostream>
#include <fstream>
using namespace std;

class Image {
public:
    ifstream *infile;
    ofstream *outfile1;
    ofstream *outfile2;
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    int ** imgAry;
    Image(ifstream *in_infile, ofstream *in_outfile1, ofstream *in_outfile2, int in_numRows,
          int in_numCols, int in_minVal, int in_maxVal){
        numRows=in_numRows;
        numCols=in_numCols;
        minVal=in_minVal;
        maxVal=in_maxVal;
        infile = in_infile;
        outfile1 = in_outfile1;
        outfile2 = in_outfile2;
        imgAry = new int*[numRows+2];
        for (int i=0; i<numRows+2; i++){
            imgAry[i]= new int[numCols+2];
            for (int j=0; j<numCols+2; j++){
                imgAry[i][j]=0;
            }
        }
    }
    void loadImage(){
        for (int r=1; r<numRows+1; r++)
            for (int c=1; c<numCols+1; c++)
                *infile>>imgAry[r][c];
    }
    void zeroFramed(){
        for (int r=0; r<numRows+2;r++){
            imgAry[r][0]=0;
            imgAry[r][numCols+1]=0;
        }
        for (int c=0; c<numCols+2; c++){
            imgAry[0][c]=0;
            imgAry[numRows+1][c]=0;
        }
    }
    
};

class ChainCode{
    int **imgAry;
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    ifstream *infile;
    ofstream *outfile1;
    ofstream *outfile2;
public:
    class Point{
    public:
        int row;
        int col;
        Point(){
            row=-1;
            col=-1;
        }
        Point(int i, int j){
            row=i;
            col=j;
        }
        ~Point(){
        }
    };
    Point *neighborCoord[8];
    Point *startP;
    Point *currentP;
    Point *nextP;
    int lastQ;
    int nextQ;
    int nextDirTable[8];
    int nextDir;
    int pChainDir;
    ChainCode(ifstream *in_infile, ofstream *in_outfile1, ofstream *in_outfile2, int in_numRows,
              int in_numCols, int in_minVal, int in_maxVal, int** in_imgAry){
        numRows=in_numRows;
        numCols=in_numCols;
        minVal=in_minVal;
        maxVal=in_maxVal;
        infile=in_infile;
        outfile1=in_outfile1;
        outfile2=in_outfile2;
        
        imgAry = in_imgAry;
        nextDirTable[0]=6;
        nextDirTable[1]=0;
        nextDirTable[2]=0;
        nextDirTable[3]=2;
        nextDirTable[4]=2;
        nextDirTable[5]=4;
        nextDirTable[6]=4;
        nextDirTable[7]=6;
    }
    
    void getChainCode(){
        int PP_CodeCount=0;
        //step1 scan from L->R T->B
        for (int r=1; r<numRows+1; r++){
            for (int c=1; c< numCols+1; c++){
                //step2
                if (imgAry[r][c]>0){
                    *outfile1 << r-1 <<" "<<c-1<<" "<<imgAry[r][c]<<" ";
                    *outfile2<< r-1<< " "<<c-1<<" "<<imgAry[r][c]<<" "<<endl;
                    startP = new Point(r, c);
                    currentP = startP;
                    lastQ = 4;
                    goto next;
                }
            }
        }
    next:
        //step 3 to 6
        do {
            //step3
            nextQ = (lastQ+1)%8;
            //step4 returns chainCode, nextP gets updated
            pChainDir = findNextP(currentP, nextQ);
            //step5
            *outfile1<<pChainDir<<" ";
            prettyPrint(outfile2, PP_CodeCount);
            //step6
            if (pChainDir==0)
                lastQ = nextDirTable[7];
            else
                lastQ = nextDirTable[pChainDir-1];
            currentP->~Point(); //destruct
            currentP = nextP;
            destructNeighborsCoord(); //destruct neighbors
        } while ((currentP->row!=startP->row) || (currentP->col != startP->col));
    };
    void loadNeighborsCoord(){
        neighborCoord[0] =new Point(currentP->row, currentP->col+1);
        neighborCoord[1] =new Point(currentP->row-1, currentP->col+1);
        neighborCoord[2] = new Point(currentP->row-1, currentP->col);
        neighborCoord[3] = new Point(currentP->row-1, currentP->col-1);
        neighborCoord[4] = new Point(currentP->row, currentP->col-1);
        neighborCoord[5] = new Point(currentP->row+1, currentP->col-1);
        neighborCoord[6] = new Point(currentP->row+1, currentP->col);
        neighborCoord[7] = new Point(currentP->row+1, currentP->col+1);
    }
    void destructNeighborsCoord(){
        for (int i=0; i<8; i++){
            neighborCoord[i]->~Point();
        }
    }
    int findNextP(Point * in_currentP, int in_nextQ){
        int chainDir;
        loadNeighborsCoord();
        chainDir=getChainDir(in_currentP, in_nextQ);
        nextP = neighborCoord[chainDir];
        return chainDir;
    }
    int getChainDir(Point* in_currentP, int in_nextQ){
        Point *temp;
        for (int i=0; i<8; i++){
            temp =neighborCoord[((in_nextQ+i))%8];
            if (imgAry[temp->row][temp->col]>0){
                return (in_nextQ+i)%8;
            }
        }
        return 0;
    }
    void prettyPrint(ofstream *in_outfile, int &in_codeCount){
        *in_outfile<<pChainDir<<" ";
        in_codeCount=(in_codeCount+1)%15;
        if (in_codeCount==0)
            *in_outfile<<endl;
    }
        
};


int main(int argc, const char * argv[]) {
    ifstream infile;
    ofstream outfile1;
    ofstream outfile2;
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    if (argc <4){
        cout<<"Please specify 1 input and 2 output files"<<endl;
        exit(1);
    }
    try {
        infile.open(argv[1]);
        outfile1.open(argv[2]);
        outfile2.open(argv[3]);
        infile >> numRows >> numCols >> minVal >> maxVal;
        outfile1<<numRows<<" " << numCols<<" "<< minVal<<" "<< maxVal<<endl;
        outfile2<<numRows<<" " <<numCols <<" "<< minVal<<" " <<maxVal<<endl;
    } catch (exception &e) {
        cout<<"problem opening files"<<endl;
        exit(1);
    }
    //step1
    Image *image = new Image(&infile, &outfile1, &outfile2, numRows, numCols, minVal, maxVal);
    image->zeroFramed();
    image->loadImage();
    ChainCode *chainCode = new ChainCode(&infile, &outfile1, &outfile2, numRows, numCols,
                                         minVal, maxVal, image->imgAry);
    chainCode->getChainCode();
    
    infile.close();
    outfile1.close();
    outfile2.close();
    return 0;
};
