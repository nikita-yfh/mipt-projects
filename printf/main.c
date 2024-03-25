int myprintf(const char *str, ...);

int main() {
    myprintf("%d %d %d %d %s %%", 123, 321, 222, 4354, "test");

    return 0;
}
