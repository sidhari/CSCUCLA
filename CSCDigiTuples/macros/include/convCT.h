int getCT(int s, int r)
{
    return 2*(4-s)+(r-1);
}

int getRI(int ct)
{
    return (1 - (ct/6) )*( (ct%2) + 1 ) + (ct/6)*(ct - 5);
}

int getST(int ct)
{
    return (1 - (ct/6) )*( 4 - (ct/2) ) + (ct/6);
}
