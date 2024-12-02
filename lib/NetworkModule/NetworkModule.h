
#ifndef NETWORK_MODULE_H
#define NETWORK_MODULE_H

class NetworkModule
{
public:
    void initConnect();
    void disConnect();
    void reConnect();
    bool isConnected();
};

#endif