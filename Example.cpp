#include <iostream>
#include <windows.h>
#include "ShadowVMT.hpp"


class ExampleClass
{
public:
	virtual void PrintMessage() // Virtual method to be hooked
    {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
        std::cout << "Original message" << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
};

std::unique_ptr<ShadowVMT>ShadowTable;

void HookedPrintMessage()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);

    std::cout << "Hooked message" << std::endl;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

	// Call original method using: ShadowTable->CallOriginal<void>(0);
}

int main()
{
	auto ExampleObject = new ExampleClass(); // Create an instance of ExampleClass

	auto pClass = (std::uintptr_t*)(ExampleObject); // Get the pointer to the instance

	ShadowTable = std::make_unique<ShadowVMT>(pClass); // Create ShadowVMT for the instance

	ExampleObject->PrintMessage(); // Call original method (should print "Original message")

	ShadowTable->HookMethod(0, (std::uintptr_t)HookedPrintMessage); // Hook the PrintMessage method

	ExampleObject->PrintMessage(); // Call hooked method (should print "Hooked message")

	ShadowTable->UnhookMethod(0); // Unhook the PrintMessage method

	ExampleObject->PrintMessage(); // should print "Original message"

	ShadowTable->HookMethod(0, (std::uintptr_t)HookedPrintMessage);

	ShadowTable->Restore(); // Restore the original VMT (should restore all the original methods as these are not changed within this hooking method)

	ExampleObject->PrintMessage(); // should print "Original message"

	delete ExampleObject; // Clean up

    return 0;
}
