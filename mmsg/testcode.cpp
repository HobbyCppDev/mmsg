// dll test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "mmsg.h"
#include <thread>
using namespace std;

Dialog a;

void listener(int a) {
    std::cout << a << std::endl;
}

int main()
{
    std::cout << "Hello World!\n" << std::endl;

    for (int i = 0; i < 100; i++) {
        Sleep(i * 2);
        std::cout << i << std::endl;
    }

    thread t([]() {
        a = Dialog::NewDialog(L"Compiling Vulkan Shaders", L"Setup", NONE, INFO_ICON, listener);
        });
    t.detach();

    for (int i = 0; i < 100; i++) {
        Sleep(i * 2);
        std::cout << i << std::endl;
    }

    a.CloseDialog();
}

