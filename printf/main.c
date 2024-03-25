int myprintf(const char *str, ...);

int main() {
    myprintf("test'%b'test", 256);

    return 0;
}
