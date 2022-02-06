#include <cstdint>
#include <iostream>
#include "Helpers.h"

using namespace std;

void TestRingbuffer (void)
{
    float testElement[10] = {5.0f, 2.2f, 3.4f, 1.2f, 0.0f, -4.1f, -3.14f, 1.2f, 824234.0f, 0.0f};
    float testElement2[10] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    float p_outputElement[10];

    Ringbuffer<float>* unit = new Ringbuffer<float>(3, 10);

    unit->PutElement(testElement, 10);
    unit->PutElement(testElement2, 5);

    unit->GetElement(p_outputElement);

    for(uint8_t i = 0; i < 10; i++)
        cout << p_outputElement[i] << endl;

    unit->GetElement(p_outputElement);

    for(uint8_t i = 0; i < 10; i++)
        cout << p_outputElement[i] << endl;
    
    delete unit;
}