#include "OrderedBorderSegmentExtractor.h"
#include "Segmentation/Segmentation.h"

#include<queue>
#include<stack>

using namespace std;

OrderedBorderSegmentExtractor::OrderedBorderSegmentExtractor()
{
}


/**
 * @brief Search for neighbord pixels
 * with intensity more than searchThreshold
 *
 * @param seg
 * @param img16bits
 * @param row
 * @param col
 */
void OrderedBorderSegmentExtractor::createSegment(Segment *seg, Mat img16bits, unsigned row, unsigned col)
{
    #ifdef SEGEXTRAC_DRWING_DEBUG
    Mat colorImg;
    img16bits.convertTo(colorImg,CV_8UC1);
    cvtColor(colorImg,colorImg,CV_GRAY2BGR);
    #endif

    // Realocate memory if necsessary
    if(seg->result.rows < 3 || seg->result.cols < maxSampleSize)
    {
        #ifdef SEGMENTATION_MEMORORY_DEBUG
            cout << "createBorderSegment: Allocating memory for segments!!" << endl;
        #endif
        seg->result = Mat(3, maxSampleSize , CV_16UC1, Scalar(0));
    }

    if(seg->countor.rows < 3 || seg->countor.cols < maxSampleSize)
    {
        seg->countor = Mat(3, maxSampleSize , CV_16UC1, Scalar(0));
    }

    // Initialize segment
    seg->N = seg->NCountor = 0;
    seg->MCol = seg->MRow = 0;
    seg->mRow = seg->mCol = 99999;

    // Initialize queue of DFS
    typedef pair<unsigned, unsigned> PUU; // < row, col>
    typedef pair<PUU, unsigned> PPUUU; // < <row,col>, dir>
    stack< PPUUU > q, qBorder;

    // Add first element
    q.push(PPUUU(PUU(row,col),0));
    searchMask->at<uchar>(row,col) = 127;

    bool isBorder;

    while(!q.empty() && seg->N < maxSampleSize)
    {
        if(!qBorder.empty()) // Take board pixel
        {
            PPUUU &e = qBorder.top();
            row = e.first.first;
            col = e.first.second;
            unsigned neighbor = (e.second + 3)%8; // inverse - 1 (clockwise)

            qBorder.pop();

            #ifdef SEGEXTRAC_DRWING_DEBUG
            Vec3b &p = colorImg.at<Vec3b>(row,col);
            p[0] = 255; p[1] = 0; p[2] = 0;
            #endif

            unsigned nextRow = row + Segmentation::neighborRow[neighbor],
                     nextCol = col + Segmentation::neighborCol[neighbor];

            // If neighbor exist in the image
            if( (nextRow < img16bits.rows &&
                 nextCol < img16bits.cols)
                &&
            // If neighbor wasn't closed
               (searchMask->at<uchar>(nextRow,nextCol) <= 127)
              // 0 = not visited, 127 = visited, 255=closed
              )
            {
                if(img16bits.at<ushort>(nextRow , nextCol) >= searchThreshold)
                {
                    // Register new querry
                    q.push(PPUUU(PUU(nextRow , nextCol),(neighbor+5)%8)); // inverse + 1 (clockwise)
                }
            }
        }
        else
        {
            isBorder = false;

            PPUUU &e = q.top();
            row = e.first.first;
            col = e.first.second;
            unsigned startNeighbor = e.second;
            q.pop();

            // If the pixel is closed
            if(searchMask->at<uchar>(row,col) == 255)
                continue;

            #ifdef SEGEXTRAC_DRWING_DEBUG
            Vec3b &p = colorImg.at<Vec3b>(row,col);
            p[0] = 255; p[1] = 0; p[2] = 0;
            #endif

            bool firstIteration=true;

            // Search in horizontal and vertical axies
            for(unsigned neighbor = startNeighbor ;
                  seg->N < maxSampleSize;
                neighbor= (neighbor+2)%8)
            {
                if(neighbor == startNeighbor)
                {
                    if(!firstIteration) break;
                    firstIteration = false;
                }

                unsigned nextRow = row + Segmentation::neighborRow[neighbor],
                         nextCol = col + Segmentation::neighborCol[neighbor];

                // If neighbor do not exist in the image
                if(nextRow >= img16bits.rows ||
                   nextCol >= img16bits.cols)
                    continue;

                // If neighbor wasn't visited
                if(searchMask->at<uchar>(nextRow,nextCol) == 0)
                {
                    if(img16bits.at<ushort>(nextRow , nextCol) >= searchThreshold)
                    {
                        // Register the visit of pixel
                        searchMask->at<uchar>(nextRow,nextCol) = 127;

                        // Register new querry ( L_querry = +6 )
                        q.push(PPUUU(PUU(nextRow , nextCol),(neighbor+6)%8));
                    }else
                    {
                        isBorder = true;
                        qBorder.push(PPUUU(PUU(nextRow , nextCol),neighbor));
                    }
                }
            }

            // Close pixel acess
            searchMask->at<uchar>(row,col) = 255;

            // Register Element
            seg->result.at<ushort>(0,seg->N) = row;
            seg->result.at<ushort>(1,seg->N) = col;
            seg->result.at<ushort>(2,seg->N) = img16bits.at<ushort>(row,col);
            seg->N++;

            #ifdef SEGEXTRAC_DRWING_DEBUG
            imshow("SegDebug", colorImg);
            waitKey(100);
            #endif

            if(isBorder)
            {
                #ifdef SEGEXTRAC_DRWING_DEBUG
                p[0] = 0; p[1] = 255; p[2] = 0;
                #endif

                // Register boarder element
                seg->countor.at<ushort>(0,seg->NCountor) = row;
                seg->countor.at<ushort>(1,seg->NCountor) = col;
                seg->countor.at<ushort>(2,seg->NCountor) = img16bits.at<ushort>(row,col);
                seg->NCountor++;

                // Take min max col and row
                if(seg->MRow < row)
                    seg->MRow = row;
                if(seg->mRow > row)
                    seg->mRow = row;
                if(seg->MCol < col)
                    seg->MCol = col;
                if(seg->mCol > col)
                    seg->mCol = col;
            }
        }
    }
}

void OrderedBorderSegmentExtractor::load(ConfigLoader &config)
{
    int vi;

    if(config.getInt("General","MaxSampleSize",&vi))
    {
        maxSampleSize = vi;
    }

    if(config.getInt("OrderedBorderSegmentExtractor","searchThreshold",&vi))
    {
        searchThreshold = vi;
    }

}

void OrderedBorderSegmentExtractor::setThreshold(unsigned threshold)
{
    searchThreshold = threshold;
}
