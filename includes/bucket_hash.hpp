uint64 bucket[81];

/*
    find checks if a value was added to the hash.
*/
bool find(int val)
{
    int pos = val / 62;
    return bucket[pos] & (1LL << (val % 63));
}

/*
    insert inserts a value into the hash.
*/
void insert(int val)
{
    bucket[val / 62] |= (1LL << (val % 63));
}