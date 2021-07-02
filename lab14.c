#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <CL/cl.h>

#include "lib56/files.h"
#include "lib56/parseInt.h"
#include "lib56/stringsCompare.h"
#include "lib56/file/bmp.h"
#include "lib56/stringCopy.h"

char scale = 4;

BITMAP56* bitmap = NULL;

// Window
HWND hwnd;
HDC hdc;
int screenWidth;
int screenHeight;
int windowWidth;
int windowHeight;
int CONTINUE = 1;
BITMAPINFO bitmapInfo;

cl_kernel processLifeKernel;
cl_command_queue queue;

void save();

struct sCLBuffers{
    unsigned char* byteOutputImage;
    cl_mem cells;
    cl_mem outputCells;
    cl_mem outputImage;
} CLBuffers;

cl_device_id getOpenCLGpuDevice(){
    cl_platform_id platformIds[4];
    cl_uint platformsCount;
    clGetPlatformIDs(4, platformIds, &platformsCount);

    if(platformsCount==0){
        printf("OpenCL platforms count = 0");
        exit(1);
    }

    for (int i = 0; i < platformsCount; ++i) {
        cl_device_id deviceIds[4];
        cl_uint devicesCount;
        clGetDeviceIDs(platformIds[i], CL_DEVICE_TYPE_GPU, 4, deviceIds, &devicesCount);
        if(devicesCount!=0){
            return deviceIds[0];
        }
    }
    printf("OpenCL GPU devices count = 0");
    exit(1);
}
const char* getErrorString(cl_int error){
    switch(error){
        case 0: return "CL_SUCCESS";
        case -1: return "CL_DEVICE_NOT_FOUND";
        case -2: return "CL_DEVICE_NOT_AVAILABLE";
        case -3: return "CL_COMPILER_NOT_AVAILABLE";
        case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case -5: return "CL_OUT_OF_RESOURCES";
        case -6: return "CL_OUT_OF_HOST_MEMORY";
        case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
        case -8: return "CL_MEM_COPY_OVERLAP";
        case -9: return "CL_IMAGE_FORMAT_MISMATCH";
        case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        case -11: return "CL_BUILD_PROGRAM_FAILURE";
        case -12: return "CL_MAP_FAILURE";
        case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
        case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
        case -15: return "CL_COMPILE_PROGRAM_FAILURE";
        case -16: return "CL_LINKER_NOT_AVAILABLE";
        case -17: return "CL_LINK_PROGRAM_FAILURE";
        case -18: return "CL_DEVICE_PARTITION_FAILED";
        case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
            // compile-time errors
        case -30: return "CL_INVALID_VALUE";
        case -31: return "CL_INVALID_DEVICE_TYPE";
        case -32: return "CL_INVALID_PLATFORM";
        case -33: return "CL_INVALID_DEVICE";
        case -34: return "CL_INVALID_CONTEXT";
        case -35: return "CL_INVALID_QUEUE_PROPERTIES";
        case -36: return "CL_INVALID_COMMAND_QUEUE";
        case -37: return "CL_INVALID_HOST_PTR";
        case -38: return "CL_INVALID_MEM_OBJECT";
        case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        case -40: return "CL_INVALID_IMAGE_SIZE";
        case -41: return "CL_INVALID_SAMPLER";
        case -42: return "CL_INVALID_BINARY";
        case -43: return "CL_INVALID_BUILD_OPTIONS";
        case -44: return "CL_INVALID_PROGRAM";
        case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
        case -46: return "CL_INVALID_KERNEL_NAME";
        case -47: return "CL_INVALID_KERNEL_DEFINITION";
        case -48: return "CL_INVALID_KERNEL";
        case -49: return "CL_INVALID_ARG_INDEX";
        case -50: return "CL_INVALID_ARG_VALUE";
        case -51: return "CL_INVALID_ARG_SIZE";
        case -52: return "CL_INVALID_KERNEL_ARGS";
        case -53: return "CL_INVALID_WORK_DIMENSION";
        case -54: return "CL_INVALID_WORK_GROUP_SIZE";
        case -55: return "CL_INVALID_WORK_ITEM_SIZE";
        case -56: return "CL_INVALID_GLOBAL_OFFSET";
        case -57: return "CL_INVALID_EVENT_WAIT_LIST";
        case -58: return "CL_INVALID_EVENT";
        case -59: return "CL_INVALID_OPERATION";
        case -60: return "CL_INVALID_GL_OBJECT";
        case -61: return "CL_INVALID_BUFFER_SIZE";
        case -62: return "CL_INVALID_MIP_LEVEL";
        case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
        case -64: return "CL_INVALID_PROPERTY";
        case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
        case -66: return "CL_INVALID_COMPILER_OPTIONS";
        case -67: return "CL_INVALID_LINKER_OPTIONS";
        case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

            // extension errors
        case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
        case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
        case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
        case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
        case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
        case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
        default: return "Unknown OpenCL error";
    }
}
void checkGPUError(cl_int error_code){
    if(error_code==0)
        return;
    printf("%s\n", getErrorString(error_code));
    exit(1);
}
cl_kernel loadKernel(const char* fileName, const char* kernelName, cl_device_id deviceId, cl_context context, const char* options){
    cl_int error_code = 0;
    int length;
    char* kernelData = readFile(fileName, &length);
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&kernelData, (const size_t*)&length, &error_code);
    checkGPUError(error_code);
    error_code = clBuildProgram(program, 1, &deviceId, options, NULL, NULL);
    checkGPUError(error_code);
    cl_kernel kernel = clCreateKernel(program, kernelName, &error_code);
    checkGPUError(error_code);
    free(kernelData);
    return kernel;
}

LRESULT HandleMessage(HWND Hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message) {
        case WM_DESTROY:
            CONTINUE = 0;
            PostQuitMessage(0);
            break;
    }
    return DefWindowProcA(Hwnd, message, wParam, lParam);
}
void GetScreenSize(){
    LPRECT rect = malloc(sizeof(*rect));
    GetClientRect(GetDesktopWindow(), rect);
    screenWidth = rect->right;
    screenHeight = rect->bottom;
    free(rect);
}
void initMyWindow(){
    WNDCLASSA wcl;
    memset(&wcl, 0, sizeof(WNDCLASSA));
    wcl.lpszClassName = "Life";
    wcl.lpfnWndProc = HandleMessage;
    wcl.hCursor = LoadCursor(0, IDC_ARROW);
    RegisterClassA(&wcl);

    GetScreenSize();

    scale = 8;
    char tmpScale = 1920/bitmap->bitmapInfo.width;
    if(tmpScale<scale)
        scale = tmpScale;
    tmpScale = 1080/bitmap->bitmapInfo.height;
    if(tmpScale<scale)
        scale = tmpScale;
    scale = scale<1 ? 1 : scale;

    windowWidth = bitmap->bitmapInfo.width*scale;
    windowHeight = bitmap->bitmapInfo.height*scale;
    printf("%dx%d\n", windowWidth, windowHeight);

    hwnd = CreateWindow("Life", "Life Name", (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), (screenWidth-windowWidth)/2, (screenHeight-windowHeight)/2, windowWidth, windowHeight, NULL, NULL, NULL, NULL);
    ShowWindow(hwnd, SW_SHOWNORMAL);
    hdc = GetDC(hwnd);
}
int isPause = 1;
int renderFrame = 0;
int renderDelay = 16;
int iter = -1;
void CheckInput(){

    if(GetAsyncKeyState(VK_OEM_PLUS))
        renderDelay = renderDelay>0 ? renderDelay-1 : 0;
    if(GetAsyncKeyState(VK_OEM_MINUS))
        renderDelay++;

    static int lastEnterState = 0;
    if(!GetAsyncKeyState('\r')&&lastEnterState)
        renderFrame++;
    lastEnterState = GetAsyncKeyState('\r');

    static int lastSpaceState = 0;
    if(!GetAsyncKeyState(' ')&&lastSpaceState)
        isPause = !isPause;
    lastSpaceState = GetAsyncKeyState(' ');

    static int lastSaveState = 0;
    if(!(GetAsyncKeyState('S')&&GetAsyncKeyState(VK_CONTROL))&&lastSaveState)
        save();
    lastSaveState = GetAsyncKeyState('S')&&GetAsyncKeyState(VK_CONTROL);

    if(GetAsyncKeyState(VK_ESCAPE)){
        CONTINUE = 0;
        PostQuitMessage(0);
    }
}

void initOpenCL(){
    cl_int error_code = 0;
    cl_device_id deviceId = getOpenCLGpuDevice();
    cl_context context = clCreateContext(NULL, 1, &deviceId, NULL, NULL, &error_code);
    queue = clCreateCommandQueue(context, deviceId, 0, &error_code);

    size_t frameBufferSize = windowWidth*windowHeight*4;
    CLBuffers.byteOutputImage = malloc(frameBufferSize);
    CLBuffers.outputCells = malloc(bitmap->bitmapInfo.height*bitmap->bitmapInfo.width*4);

    CLBuffers.cells = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, bitmap->bitmapInfo.width*bitmap->bitmapInfo.height*4, bitmap->data, &error_code);
    CLBuffers.outputCells = clCreateBuffer(context, CL_MEM_READ_WRITE, bitmap->bitmapInfo.width*bitmap->bitmapInfo.height*4, NULL, &error_code);
    CLBuffers.outputImage = clCreateBuffer(context, CL_MEM_READ_WRITE, frameBufferSize, NULL, &error_code);

    processLifeKernel = loadKernel("lifeKernel.cl", "processLife", deviceId, context, NULL);
}
void ProcessOpenCL() {
    cl_int error_code;
    cl_event eventToWait;

    error_code = clEnqueueWriteBuffer(queue, CLBuffers.cells, CL_FALSE, 0, bitmap->bitmapInfo.width*bitmap->bitmapInfo.height*4, bitmap->data, 0, NULL, &eventToWait); checkGPUError(error_code);

    size_t global_work_size[1] = {bitmap->bitmapInfo.width*bitmap->bitmapInfo.height};

    error_code = clSetKernelArg(processLifeKernel, 0, sizeof(int), &bitmap->bitmapInfo.width); checkGPUError(error_code);
    error_code = clSetKernelArg(processLifeKernel, 1, sizeof(int), &bitmap->bitmapInfo.height); checkGPUError(error_code);
    error_code = clSetKernelArg(processLifeKernel, 2, sizeof(cl_mem *), &CLBuffers.cells); checkGPUError(error_code);
    error_code = clSetKernelArg(processLifeKernel, 3, sizeof(cl_mem *), &CLBuffers.outputCells); checkGPUError(error_code);
    error_code = clSetKernelArg(processLifeKernel, 4, sizeof(cl_mem *), &CLBuffers.outputImage); checkGPUError(error_code);
    error_code = clSetKernelArg(processLifeKernel, 5, sizeof(int), &iter); checkGPUError(error_code);
    error_code = clSetKernelArg(processLifeKernel, 6, sizeof(char), &scale); checkGPUError(error_code);

    error_code = clEnqueueNDRangeKernel(queue, processLifeKernel, 1, NULL, global_work_size, NULL, 1, &eventToWait, &eventToWait); checkGPUError(error_code);
    error_code = clEnqueueReadBuffer(queue, CLBuffers.outputCells, CL_FALSE, 0, bitmap->bitmapInfo.width * bitmap->bitmapInfo.height * 4, bitmap->data, 1, &eventToWait, NULL); checkGPUError(error_code);
    error_code = clEnqueueReadBuffer(queue, CLBuffers.outputImage, CL_FALSE, 0, windowWidth*windowHeight*4, CLBuffers.byteOutputImage, 1, &eventToWait, NULL); checkGPUError(error_code);
    clFinish(queue);
}
void initScreenBufferInfo(){
    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    bitmapInfo.bmiHeader.biClrUsed = 0;
    bitmapInfo.bmiHeader.biClrImportant = 0;
    bitmapInfo.bmiHeader.biWidth = windowWidth;
    bitmapInfo.bmiHeader.biHeight = windowHeight;
}
void countFPS(){
    static int frames = 0;
    static int lastTime = 0;
    if(lastTime==0)
        lastTime = time(NULL);
    frames++;
    if(time(NULL)-lastTime>=1) {
        printf("FPS: %d\n", frames/1);
        lastTime+=1;
        frames = 0;
    }
}

int max_iter = 0;
int dump_freq = 0;

void Draw(){
    if(renderFrame){
        renderFrame--;
    } else {
        if(isPause && iter>-1){
            return;
        }
    }
    iter++;

    ProcessOpenCL();
    SetStretchBltMode(hdc, COLORONCOLOR);
    StretchDIBits(hdc,0, 0, windowWidth, windowHeight,0, 0, windowWidth, windowHeight, CLBuffers.byteOutputImage, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
    countFPS();

    if(iter && dump_freq && (iter%dump_freq == 0)){
        save();
    }
}
void StartMainCircle(){
    printf("Press SPACE to start\n");
    MSG msg;
    while(CONTINUE){
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
            DispatchMessage(&msg);
        }
        CheckInput();
        Draw();
        if(max_iter && iter >= max_iter){
            break;
        }
        Sleep(renderDelay);
    }
}

char outputFile[60];
int outputFolderLength;
int main(int argc, char** args) {
    char* input = NULL;
    char* output = NULL;

    for(int i=1; i<argc; i++){
        if(i+1 == argc)
            break;
        char* nextString = args[i+1];
        if(!stringsCompare(args[i],"--input",7))
            input = nextString;
        else if(!stringsCompare(args[i],"--output",6))
            output = nextString;
        else if(!stringsCompare(args[i],"--max_iter",5)){
            max_iter = parseInt(nextString);
            if(max_iter<0) {
                printf("Wrong max iteration\n");
                printf("It will be set to 0: infinite\n");
                max_iter = 0;
            }
        }else if(!stringsCompare(args[i],"--dump_freq",7)){
            dump_freq = parseInt(nextString);
            if(dump_freq<0) {
                printf("Wrong dump frequency\n");
                printf("It will be set to 0: no dumps\n");
                dump_freq = 0;
            }
        }
    }

    if(!input){
        printf("Input file was not defined\n");
        printf("    --input file.bmp\n");
        exit(1);
    }

    int length;
    char* buffer = readFile(input, &length);
    if(!buffer) {
        printf("Failed to read file: %s\n", input);
        exit(1);
    }

    bitmap = readBmp(buffer);
    if(!bitmap){
        printf("Failed to read bmp file: %s\n", input);
        printf("Error: %s\n", getLastBmpError());
        exit(1);
    }

    if(output){
        outputFolderLength = stringCopy(outputFile, output);
        outputFile[outputFolderLength] = '/';
    }
    else
        outputFolderLength = -1;
    outputFile[outputFolderLength+6] = '.';
    outputFile[outputFolderLength+7] = 'b';
    outputFile[outputFolderLength+8] = 'm';
    outputFile[outputFolderLength+9] = 'p';
    outputFile[outputFolderLength+10] = 0;

    initMyWindow();
    initScreenBufferInfo();
    initOpenCL();
    StartMainCircle();

    return 0;
}

void save() {
    int length;
    char* outputBmpDataBuffer = writeBmp(bitmap, &length);
    int tmpIterNum = iter;
    for(int i=5;i>0;i--){
        outputFile[outputFolderLength+i] = (tmpIterNum%10) + '0';
        tmpIterNum /= 10;
    }
    int error = writeFile(outputFile, length, outputBmpDataBuffer);
    if(error){
        printf("Couldn't write file: %s\n", outputFile);
    } else {
        printf("Saved: %s\n", outputFile);
    }
    free(outputBmpDataBuffer);
}