// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
	int	type = kernel->machine->ReadRegister(2);
	int	val;
	int sumResult;
	int leftOperand;
	int rightOperand;

    switch (which) {
	case SyscallException:
	    switch(type) {
		case SC_Halt:
		    DEBUG(dbgAddr, "Shutdown, initiated by user program.\n");
   		    kernel->interrupt->Halt();
		    break;
		case SC_PrintInt:
			val=kernel->machine->ReadRegister(4);
			cout << "Print integer:" <<val << endl;
			return;
/*		case SC_Exec:
			DEBUG(dbgAddr, "Exec\n");
			val = kernel->machine->ReadRegister(4);
			kernel->StringCopy(tmpStr, retVal, 1024);
			cout << "Exec: " << val << endl;
			val = kernel->Exec(val);
			kernel->machine->WriteRegister(2, val);
			return;
*/		case SC_Exit:
			DEBUG(dbgAddr, "Program exit\n");
			val=kernel->machine->ReadRegister(4);
			cout << "return value:" << val << endl;
			kernel->currentThread->Finish();
			break;

		case SC_Sleep:
			val = kernel->machine->ReadRegister(4);
			cout << "Sleep Time: " << val << "(ms)" << endl;
			kernel->alarm->WaitUntil(val);
			return;

		case SC_Add:
			leftOperand = kernel->machine->ReadRegister(4);  // r4 (a0)
			rightOperand = kernel->machine->ReadRegister(5); // r5 (a1)
			sumResult = leftOperand + rightOperand;
			kernel->machine->WriteRegister(2, sumResult);        // r2 (v0)
			return;

		case SC_Sub:
			leftOperand = kernel->machine->ReadRegister(4);  // r4 (a0)
			rightOperand = kernel->machine->ReadRegister(5); // r5 (a1)
			sumResult = leftOperand - rightOperand;
			kernel->machine->WriteRegister(2, sumResult);        // r2 (v0)
			
			return;
		case SC_Mul:
			leftOperand = kernel->machine->ReadRegister(4);  // r4 (a0)
			rightOperand = kernel->machine->ReadRegister(5); // r5 (a1)
			sumResult = leftOperand * rightOperand;
			kernel->machine->WriteRegister(2, sumResult);        // r2 (v0)
			return;

		case SC_Div:
			leftOperand = kernel->machine->ReadRegister(4);  // r4 (a0)
			rightOperand = kernel->machine->ReadRegister(5); // r5 (a1)
			
			if (rightOperand == 0) {
			cout << "Error: Divide by zero" << endl;
				sumResult = 11217022;
				kernel->machine->WriteRegister(2, sumResult);
				return;
			}
			else {
				sumResult = leftOperand / rightOperand;
				kernel->machine->WriteRegister(2, sumResult);        // r2 (v0)
				return;
			}

		case SC_Mod:
			leftOperand = kernel->machine->ReadRegister(4);  // r4 (a0)
			rightOperand = kernel->machine->ReadRegister(5); // r5 (a1)
			sumResult = leftOperand % rightOperand;
			kernel->machine->WriteRegister(2, sumResult);        // r2 (v0)
			return;
		
		case SC_Print: 
			int addr = kernel->machine->ReadRegister(4); // user 傳來的字串位址
			int val;
			char ch;
			std::string buffer;
			// read until '\0'
			while (true) {
				kernel->machine->ReadMem(addr, 1, &val);
				ch = (char)val;
				if (ch == '\0') break;
				buffer.push_back(ch);
				addr++;
			}
		
			// ====== landmine ======
			int sid = 11217022; 
			int landmine = (sid % 100) % 26;
			char big = 'A' + landmine;
			char small = 'a' + landmine;
		
			for (auto &c : buffer) {
				if (c == big || c == small) c = '*';
			}
		
			// ====== output ======
			std::cout << "[B11217022_Print] " << buffer << std::endl;
		
			// return string length (without '\0')
			kernel->machine->WriteRegister(2, buffer.length());
			return;
	    }
	    break;
	default:
	    cerr << "Unexpected user mode exception" << which << "\n";
	    break;
    }
    ASSERTNOTREACHED();
}
