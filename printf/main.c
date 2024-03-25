int myprintf(const char *str, ...);

int main() {
    myprintf("test'%x'test", 256);

    return 0;
}
