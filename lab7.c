#include <stdio.h>

typedef enum Day{
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    SUNDAY
} Day;

typedef struct Rect{
    double x, y, width, height, angle;
} Rect;

double getRectSquare(Rect* rect){
    return rect->width*rect->height;
}
double getRectPerimeter(Rect* rect){
    return (rect->width+rect->height)*2;
}

typedef struct StateField{
    unsigned isPlaying : 1;
    unsigned isPaused : 1;
    unsigned isRecording : 1;
} StateField;

typedef union State {
    StateField field;
    char value;
} State;

int main() {
    //Task 1
    Day day = MONDAY;
    printf("%d\n",day);

    //Task 2
    Rect rectangle = {1,1,10,5,0};
    printf("Rectangle square is:\t %lf\n",getRectSquare(&rectangle));
    printf("Rectangle perimeter is:\t %lf\n",getRectPerimeter(&rectangle));

    //Task 3
    State state;
    scanf("%x",&state.value);

    char* answers[2] = {"No","Yes"};
    printf("MP3-Player-Recorder state:\n");
    printf("\tIs playing: %s\n", answers[state.field.isPlaying]);
    printf("\tIs paused: %s\n", answers[state.field.isPaused]);
    printf("\tIs recording: %s\n", answers[state.field.isRecording]);

    return 0;
}