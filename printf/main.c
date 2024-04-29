int myprintf(const char *str, ...);

#define RED     "\e[1;31m"
#define NONE    "\e[0m"
#define PURPLE  "\e[1;35m"
int main() {
    myprintf(RED"Printf test:\n"NONE
             "\t1) "PURPLE"Characters."NONE" '%c', '%c', '%c'.\n"
             "\t2) "PURPLE"Strings."NONE" str1=\"%s\"\n"
             "\t3) "PURPLE"Numbers."NONE" n1=(bin) %b=(oct) %o=(dec) %d=(hex) %x\n"
             "\t           n2=(bin) %b=(oct) %o=(dec) %d=(hex) %x\n"
             "\t4) "PURPLE"Escape percent:"NONE" %d %%.\n",
             'D', 'e', 'd', "teststring", 42, 42, 42, 42, 1024, 1024, 1024, 1024, 35);

    return 0;
}
