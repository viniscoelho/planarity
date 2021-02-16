void printElapsedTime(clock_t start, clock_t stop)
{
    double elapsed = ((double)(stop - start)) / CLOCKS_PER_SEC;
    cout << fixed << setprecision(3) << "Elapsed time: " << elapsed << "s\n";
}

/*
    read is a fast input implementation.
*/
bool read(int& n)
{
    n = 0;
    register bool neg = false;
    register char c = getchar_unlocked();
    if (c == EOF) {
        n = -1;
        return false;
    }
    while (!('0' <= c && c <= '9')) {
        if (c == '-')
            neg = true;
        c = getchar_unlocked();
    }
    while ('0' <= c && c <= '9') {
        n = n * 10 + c - '0';
        c = getchar_unlocked();
    }
    n = (neg ? (-n) : (n));
    return true;
}