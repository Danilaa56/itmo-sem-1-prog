int stringCopy(char* dest, const char* src){
    int length = 0;
    while(1){
        dest[length] = src[length];
        if(!src[length])
            return length;
        length++;
    }
}