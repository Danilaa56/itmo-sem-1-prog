int stringsCompare(const char* string1, const char* string2, int length){
    int index = -1;
    while (++index!=length){
        if(string1[index]!=string2[index]) {
            return string1[index]-string2[index];
        }
        if(string1[index]==0)
            return 0;
    }
    return 0;
}