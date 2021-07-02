int countNeighbours(int x, int y, int width, int height, __global int* data){
     #define getPixel(x, y) !data[(y)*width + x]
     int count = 0;
     if(0 < x){
         if(getPixel(x-1,y))
             count++;
         if(0 < y){
             if(getPixel(x-1,y-1))
                 count++;
         }
         if(y+1 < height){
             if(getPixel(x-1,y+1))
                 count++;
         }
     }
     if(x+1 < width){
         if(getPixel(x+1,y))
             count++;
         if(0 < y){
             if(getPixel(x+1,y-1))
                 count++;
         }
         if(y+1 < height){
             if(getPixel(x+1,y+1))
                 count++;
         }
     }
     if(0 < y){
         if(getPixel(x,y-1))
             count++;
     }
     if(y+1 < height){
         if(getPixel(x,y+1)){
             count++;
         }
     }
     return count;
 }

__kernel void processLife(int width, int height, __global int* cells, __global int* outputCells, __global int* outputImage, int iter, char scale)
{
    int i = get_global_id(0);
    int x = i%width;
    int y = i/width;

    if(!iter){
        outputCells[i] = cells[i];
    } else {
        int count = countNeighbours(x, y, width, height, cells);
        if(count==3){
            outputCells[i] = 0;
        } else if(count==2 && !cells[i]){
            outputCells[i] = 0;
        } else {
            outputCells[i] = 1;
        }
    }
    int color = outputCells[i] ? 0xff3388ee : 0xff112277;
    for(int j=0; j<scale; j++){
        for(int i=0; i<scale; i++){
           outputImage[(y*scale+j)*width*scale+(x*scale+i)] = color;
        }
    }
}

//int countNeighbours(int x,int y,int width,int height,__global int* data){#define getPixel(x,y) !data[(y)*width+x]
//int count=0;if(0<x){if(getPixel(x-1,y))count++;if(0<y){if(getPixel(x-1,y-1))count++;}if(y+1<height){if(getPixel(x-1,y+1))count++;}}if(x+1<width){if(getPixel(x+1,y))count++;if(0<y){if(getPixel(x+1,y-1))count++;}if(y+1<height){if(getPixel(x+1,y+1))count++;}}if(0<y){if(getPixel(x,y-1))count++;}if(y+1<height){if(getPixel(x,y+1))count++;}return count;}
//__kernel void processLife(int width, int height,__global int*cells,__global int*outputCells,__global int*outputImage){int i=get_global_id(0);int x=i%width;int y=i/width;int count=countNeighbours(x,y,width,height,cells);if(count==3)outputCells[i]=0;else if(count==2&&!cells[i])outputCells[i]=0;else outputCells[i]=1;int color=outputCells[i]?0xff3388ee:0xff112277;int scale=8;for(int j=0;j<scale;j++){for(int i=0;i<scale;i++){outputImage[(y*scale+j)*width*scale+(x*scale+i)]=color;}}}