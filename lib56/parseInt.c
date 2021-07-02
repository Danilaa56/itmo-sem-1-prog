int parseInt(const char* str){
    char isNegative = 0;
    if(str[0]=='-'){
        isNegative = 1;
        str++;
    }
    int num = 0;
    while('0' <= *str && *str <= '9')
        num = num*10 + *str++-48;
    return isNegative ? -num : num;
}