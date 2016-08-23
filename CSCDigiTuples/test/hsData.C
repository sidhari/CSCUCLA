{
    vector<int> dataBuf;
    vector<int> layerBuf;

    dataBuf.push_back(23);
    layerBuf.push_back(0);
    dataBuf.push_back(24);
    layerBuf.push_back(1);
    dataBuf.push_back(23);
    layerBuf.push_back(2);
    dataBuf.push_back(24);
    layerBuf.push_back(3);
    dataBuf.push_back(23);
    layerBuf.push_back(4);
    dataBuf.push_back(24);
    layerBuf.push_back(5);

    hsData myD(dataBuf,layerBuf,3);
    myD.print();
    cout << endl;
    cout << myD.getCount() << endl;
}
