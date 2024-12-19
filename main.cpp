#include <iostream>
#include <string>
#include <vector>


class Order
{
public:
    Order();
    void PlaceOrder();
    void CancelOrder();
    void ModifyOrder();
    void GetOrder();
    void  ViewCurrentPositions ();
    void DataStreaming();
    ~Order();
};

int main()
{
    return 0;
}