//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//


#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegAllocRegistry.h"
//#include "llvm/Target/TargetInstrInfo.h"
//#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Compiler.h"
#include "llvm/ADT/Statistic.h"
#include <iostream>	
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/Support/raw_ostream.h"
#include "AArch64.h"
#include "AArch64InstrInfo.h"
#include "AArch64RegisterInfo.h"
#include "AArch64MachineFunctionInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBundle.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm//MC/MCInst.h"
//#include "InstPrinter/X86InstPrinter.h"
//#include "X86MCInstLower.h"
#include "llvm-c/Core.h" 
#include <sstream>
#include <map>
#include <unordered_map>
#include <string>
#include <queue>

//using namespace llvm;

namespace llvm{

	static cl::opt<bool> EnableSwiftR(
			"enable-SwiftR",
			cl::init(false),
			cl::desc("Implement Swift-R soft error detection and recovery scheme."),
			cl::Hidden);

	struct Swift : public MachineFunctionPass {
		static char ID;
		Swift() : MachineFunctionPass(ID) {
		srand(777);
		}

	std::unordered_map<unsigned int, unsigned int> registersMtoS = {{ AArch64::X0, AArch64::X8 }, { AArch64::W0, AArch64::W8 }, { AArch64::X1, AArch64::X9 }, { AArch64::W1, AArch64::W9 }, { AArch64::X2, AArch64::X10 }, { AArch64::W2, AArch64::W10 }, { AArch64::X3, AArch64::X11 }, { AArch64::W3, AArch64::W11 }, { AArch64::X4, AArch64::X12 }, { AArch64::W4, AArch64::W12 }, { AArch64::X5, AArch64::X13 }, { AArch64::W5, AArch64::W13 }, { AArch64::X6, AArch64::X14 }, { AArch64::W6, AArch64::W14 }, { AArch64::X7, AArch64::X15 }, { AArch64::W7, AArch64::W15 }, { AArch64::SP, AArch64::X16}, { AArch64::LR, AArch64::X17}};

	std::unordered_map<unsigned int, unsigned int> registersMtoS2 = {{ AArch64::X0, AArch64::X18 }, { AArch64::W0, AArch64::W18 }, { AArch64::X1, AArch64::X19 }, { AArch64::W1, AArch64::W19 }, { AArch64::X2, AArch64::X20 }, { AArch64::W2, AArch64::W20 }, { AArch64::X3, AArch64::X21 }, { AArch64::W3, AArch64::W21 }, { AArch64::X4, AArch64::X22 }, { AArch64::W4, AArch64::W22 }, { AArch64::X5, AArch64::X23 }, { AArch64::W5, AArch64::W23 }, { AArch64::X6, AArch64::X24 }, { AArch64::W6, AArch64::W24 }, { AArch64::X7, AArch64::X25 }, { AArch64::W7, AArch64::W25 }, { AArch64::SP, AArch64::X26}, { AArch64::LR, AArch64::X27}};

	std::unordered_map<unsigned int, unsigned int> registers64shadow1 = {{ AArch64::X0, AArch64::X8 }, { AArch64::X1, AArch64::X9 }, { AArch64::X2, AArch64::X10 }, { AArch64::X3, AArch64::X11 }, { AArch64::X4, AArch64::X12 }, { AArch64::X5, AArch64::X13 }, { AArch64::X6, AArch64::X14 }, { AArch64::X7, AArch64::X15 }, { AArch64::SP, AArch64::X16 }, { AArch64::LR, AArch64::X17}};

	std::unordered_map<unsigned int, unsigned int> registers64voting = {{ AArch64::X0, AArch64::X8 }, { AArch64::W0, AArch64::X8 }, { AArch64::X1, AArch64::X9 }, { AArch64::W1, AArch64::X9 }, { AArch64::X2, AArch64::X10 }, { AArch64::W2, AArch64::X10 }, { AArch64::X3, AArch64::X11 }, { AArch64::W3, AArch64::X11 }, { AArch64::X4, AArch64::X12 }, { AArch64::W4, AArch64::X12 }, { AArch64::X5, AArch64::X13 }, { AArch64::W5, AArch64::X13 }, { AArch64::X6, AArch64::X14 }, { AArch64::W6, AArch64::X14 }, { AArch64::X7, AArch64::X15 }, { AArch64::W7, AArch64::W15 }, { AArch64::SP, AArch64::X16}, { AArch64::LR, AArch64::X17}};

	std::unordered_map<unsigned int, unsigned int> registers32shadow1 = {{ AArch64::W0, AArch64::W8 }, { AArch64::W1, AArch64::W9 }, { AArch64::W2, AArch64::W10 }, { AArch64::W3, AArch64::W11 }, { AArch64::W4, AArch64::W12 }, { AArch64::W5, AArch64::W13 }, { AArch64::W6, AArch64::W14 }, { AArch64::W7, AArch64::W15 }};

	std::unordered_map<unsigned int, unsigned int> registers64shadow2 = {{ AArch64::X0, AArch64::X18 }, { AArch64::X1, AArch64::X19 }, { AArch64::X2, AArch64::X20 }, { AArch64::X3, AArch64::X21 }, { AArch64::X4, AArch64::X22 }, { AArch64::X5, AArch64::X23 }, { AArch64::X6, AArch64::X24 }, { AArch64::X7, AArch64::X25 }, { AArch64::SP, AArch64::X26 }, { AArch64::LR, AArch64::X27}};

	std::unordered_map<unsigned int, unsigned int> registers64voting2 = {{ AArch64::X0, AArch64::X18 }, { AArch64::W0, AArch64::X18 }, { AArch64::X1, AArch64::X19 }, { AArch64::W1, AArch64::X19 }, { AArch64::X2, AArch64::X20 }, { AArch64::W2, AArch64::X20 }, { AArch64::X3, AArch64::X21 }, { AArch64::W3, AArch64::X21 }, { AArch64::X4, AArch64::X22 }, { AArch64::W4, AArch64::X22 }, { AArch64::X5, AArch64::X23 }, { AArch64::W5, AArch64::X23 }, { AArch64::X6, AArch64::X24 }, { AArch64::W6, AArch64::X24 }, { AArch64::X7, AArch64::X25 }, { AArch64::W7, AArch64::W25 }, { AArch64::SP, AArch64::X26}, { AArch64::LR, AArch64::X27}};

	std::unordered_map<unsigned int, unsigned int> registers32shadow2 = {{ AArch64::W0, AArch64::W18 }, { AArch64::W1, AArch64::W19 }, { AArch64::W2, AArch64::W20 }, { AArch64::W3, AArch64::W21 }, { AArch64::W4, AArch64::W22 }, { AArch64::W5, AArch64::W23 }, { AArch64::W6, AArch64::W24 }, { AArch64::W7, AArch64::W25 }};



	unsigned int getSlvReg(unsigned int reg){

		std::unordered_map<unsigned int, unsigned int>::const_iterator num = registersMtoS.find(reg);

		if(num == registersMtoS.end())
		{
			return reg;
		}
		else
		{
			return num->second;
		}
	}
	unsigned int getSlvReg64(unsigned int reg){

		std::unordered_map<unsigned int, unsigned int>::const_iterator num = registers64shadow1.find(reg);

		if(num == registers64shadow1.end())
		{
			return reg;
		}
		else
		{
			return num->second;
		}
	}

	unsigned int getSlvReg2(unsigned int reg){

		std::unordered_map<unsigned int, unsigned int>::const_iterator num = registersMtoS2.find(reg);

		if(num == registersMtoS2.end())
		{
			return reg;
		}
		else
		{
			return num->second;
		}
	}
	unsigned int getSlvReg642(unsigned int reg){

		std::unordered_map<unsigned int, unsigned int>::const_iterator num = registers64shadow2.find(reg);

		if(num == registers64shadow2.end())
		{
			return reg;
		}
		else
		{
			return num->second;
		}
	}
	MachineBasicBlock* makeErrorBB(MachineFunction &MF)
	{
		//const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
		MachineBasicBlock *errorMBB = MF.CreateMachineBasicBlock();
		MachineFunction::iterator It = (MF.end())->getIterator();
		MF.insert(It, errorMBB);
		errorMBB->addSuccessor(errorMBB);
/*
		MachineInstr *MIAdd = BuildMI(MF, MF.begin()->begin()->getDebugLoc() , TII->get(AArch64::ADDXrs)).addReg(It->getOperand(0).getReg()).addReg(It->getOperand(0).getReg()).addReg(It->getOperand(0).getReg());
		errorMBB->push_back(MIAdd);
*/
		return errorMBB;
	}
	//BasicBlock *splitBasicBlock(Instruction *I, const Twine &BBName = "")
	//{
	//	return splitBasicBlock(I->getIterator(), BBName);
	//}

	bool isGPR(unsigned int regNUM) {
		std::unordered_map<unsigned int, unsigned int>::const_iterator got = registers64shadow1.find(regNUM);
		if (got == registers64voting.end())
		{
			return false;
		}
		std::unordered_map<unsigned int, unsigned int>::const_iterator got2 = registers64shadow2.find(regNUM);
		if (got2 == registers64voting2.end())
		{
			return false;
		}
		return true;
	}

	bool isGPR64(unsigned int regNUM) {
		std::unordered_map<unsigned int, unsigned int>::const_iterator got = registers64shadow1.find(regNUM);
		if (got == registers64shadow1.end())
		{
			return false;
		}
		std::unordered_map<unsigned int, unsigned int>::const_iterator got2 = registers64shadow2.find(regNUM);
		if (got2 == registers64shadow2.end())
		{
			return false;
		}
		return true;
	}

	bool isGPR32(unsigned int regNUM) {
		std::unordered_map<unsigned int, unsigned int>::const_iterator got = registers32shadow1.find(regNUM);
		if (got == registers32shadow1.end())
		{
			return false;
		}
		std::unordered_map<unsigned int, unsigned int>::const_iterator got2 = registers32shadow2.find(regNUM);
		if (got2 == registers32shadow2.end())
		{
			return false;
		}
		return true;
	}


	void duplicateInstructions(MachineFunction &MF) {
		//const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
		for(MachineFunction::iterator MBB = MF.begin(), MBE = MF.end(); MBB != MBE; ++MBB) {
			for(MachineBasicBlock::iterator I=MBB->begin(), E=MBB->end(); I != E; ++I) {
			if(!(I->isBranch()) && !(I->mayStore()) && !(I->mayLoad()) && !(I->isCall()) && /*(I->getOperand(0).getReg() != AArch64::SP) &&**/  !(I->isReturn()) && !(I->isCompare()) && !(I->getOpcode() > 96 && I->getOpcode() < 129)) {
				MachineInstr *slaveinst = MF.CloneMachineInstr (&*I);
				MachineInstr *slaveinst2 = MF.CloneMachineInstr (&*I);

				slaveinst->setFlags(0);
				slaveinst2->setFlags(0);

				for(unsigned int opcount=0; opcount<I->getNumOperands(); opcount++){
					if(I->getOperand(opcount).isReg()){
						slaveinst->getOperand(opcount).setReg(getSlvReg(I->getOperand(opcount).getReg()));
					}
				}

				for(unsigned int opcount=0; opcount<I->getNumOperands(); opcount++){
					if(I->getOperand(opcount).isReg()){
						slaveinst2->getOperand(opcount).setReg(getSlvReg2(I->getOperand(opcount).getReg()));
					}
				}
                                if(I->getOperand(0).getReg() == AArch64::SP)
                                {
                                  MBB->insertAfter(I, slaveinst);
                                  MBB->insertAfter(I, slaveinst2);
                                }
                                else if(I->getOperand(0).getReg() == AArch64::X15 || I->getOperand(0).getReg() == AArch64::X25)
                                {
                                  continue;
                                }
                                else {

                                  MBB->insert(I, slaveinst);
                                  MBB->insert(I, slaveinst2);
                                }
			}
                          /*if(!(I->isBranch()) && !(I->mayStore()) && !(I->mayLoad()) && !(I->isCall()) && (I->getOperand(0).getReg() == AArch64::SP) &&  !(I->isReturn()) && !(I->isCompare()) && !(I->getOpcode() > 96 && I->getOpcode() < 129)) {
                            MachineInstr *slaveinst = MF.CloneMachineInstr (&*I);
                            MachineInstr *slaveinst2 = MF.CloneMachineInstr (&*I);

                            slaveinst->setFlags(0);
                            slaveinst2->setFlags(0);

                            for(unsigned int opcount=0; opcount<I->getNumOperands(); opcount++){
                              if(I->getOperand(opcount).isReg()){
                                slaveinst->getOperand(opcount).setReg(getSlvReg(I->getOperand(opcount).getReg()));
                              }
                            }

                            for(unsigned int opcount=0; opcount<I->getNumOperands(); opcount++){
                              if(I->getOperand(opcount).isReg()){
                                slaveinst2->getOperand(opcount).setReg(getSlvReg2(I->getOperand(opcount).getReg()));
                              }
                            }

                            MBB->insertAfter(I, slaveinst);
                            MBB->insertAfter(I, slaveinst2);
                          }*/
				/*
				if(I->mayLoad()){
					DebugLoc DL3 = I->getDebugLoc();

					for(unsigned int opcount=0; opcount<I->getNumOperands(); opcount++){
						if(I->getOperand(opcount).isReg()){
                                                        int reg = I->getOperand(opcount).getReg();
							if(((isGPR64(reg)) || (isGPR32(reg))) && (I->getOperand(opcount).getReg() != AArch64::SP) && (I->getOperand(opcount).getReg() != AArch64::XZR) && (I->getOperand(opcount).getReg() != AArch64::WZR)) {
						
			
				

								MachineInstr *copyMoveM = BuildMI(MF, DL3, TII->get(AArch64::ADDXri)).addReg(getSlvReg(I->getOperand(opcount).getReg())).addReg(I->getOperand(opcount).getReg()).addImm(0).addImm(0);
								

								
								MachineInstr *copyMoveM2 = BuildMI(MF, DL3, TII->get(AArch64::ADDXri)).addReg(getSlvReg2(I->getOperand(opcount).getReg())).addReg(I->getOperand(opcount).getReg()).addImm(0).addImm(0);
								

								//MachineInstr *copyMoveM3 = BuildMI(MF, DL3, TII->get(AArch64::ADDXri)).addReg(getSlvReg(I->getOperand(1).getReg())).addReg(I->getOperand(1).getReg()).addImm(0).addImm(0);
								
							
								
								//MachineInstr *copyMoveM4 = BuildMI(MF, DL3, TII->get(AArch64::ADDXri)).addReg(getSlvReg2(I->getOperand(1).getReg())).addReg(I->getOperand(1).getReg()).addImm(0).addImm(0);
                                                                MBB->insertAfter(I,copyMoveM);
								MBB->insertAfter(I,copyMoveM2);
								
								//MBB->insertAfter(I,copyMoveM3);
								//MBB->insertAfter(I,copyMoveM4);
								I++;
								I++;
								I++;
								I++;	
								}
							}
						}				
					
					}

				*/
				}
			}
		}

        void copyLoadValuesSWIFTR(MachineFunction &MF){//copy for load
          for(MachineFunction::iterator MBB = MF.begin(), MBE = MF.end(); MBB != MBE; ++MBB) {
            for (MachineBasicBlock::iterator I=MBB->begin(), E=MBB->end(); I !=E ; ++I){
		if(I->isCall())
		{
			DebugLoc DL3= I->getDebugLoc();
                const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();

                /*for (unsigned int opcount=0 ; opcount < I->getNumOperands() ;opcount++){ //
                  if(I->getOperand(opcount).isReg()) {
                    int reg = I->getOperand(opcount).getReg();
		    /*if((isGPR(reg)) && (I->getOperand(opcount).getReg() != AArch64::SP) && (I->getOperand(opcount).getReg() != AArch64::WZR) && (I->getOperand(opcount).getReg() != AArch64::XZR) && (I->getOperand(opcount).getReg() == AArch64::LR)) {

		    MachineInstr *copyMoveM = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(getSlvReg64(I->getOperand(opcount).getReg())).addReg(I->getOperand(opcount).getReg()).addImm(0).addImm(0);
                      MBB->insert(I,copyMoveM);
                      MachineInstr *copyMoveD = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(getSlvReg642(I->getOperand(opcount).getReg())).addReg(I->getOperand(opcount).getReg()).addImm(0).addImm(0);
                      MBB->insert(I,copyMoveD);
			}
                    if((isGPR64(reg)) && (I->getOperand(opcount).getReg() != AArch64::SP) && (I->getOperand(opcount).getReg() != AArch64::WZR) && (I->getOperand(opcount).getReg() != AArch64::XZR)) {
			
		      MachineInstr *copyMoveM2 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(getSlvReg64(I->getOperand(opcount).getReg())).addReg(I->getOperand(opcount).getReg()).addImm(0).addImm(0);
                      MBB->insertAfter(I,copyMoveM2);
                      MachineInstr *copyMoveD2 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(getSlvReg642(I->getOperand(opcount).getReg())).addReg(I->getOperand(opcount).getReg()).addImm(0).addImm(0);
                      MBB->insertAfter(I,copyMoveD2);*/


                  MachineInstr *MIcopyX0 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X8).addReg(AArch64::X0).addImm(0).addImm(0);
                  MBB->insertAfter(I, MIcopyX0);
                  MIcopyX0 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X18).addReg(AArch64::X0).addImm(0).addImm(0);
                  MBB->insertAfter(I, MIcopyX0);
                  MachineInstr *MIcopyX1 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X9).addReg(AArch64::X1).addImm(0).addImm(0);
                  MBB->insertAfter(I, MIcopyX1);
                  MIcopyX1 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X19).addReg(AArch64::X1).addImm(0).addImm(0);
                  MBB->insertAfter(I, MIcopyX1);
                  MachineInstr *MIcopyX2 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X16).addReg(AArch64::SP).addImm(0).addImm(0);
                  MBB->insertAfter(I, MIcopyX2);
                  MIcopyX2 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X26).addReg(AArch64::SP).addImm(0).addImm(0);
                  MBB->insertAfter(I, MIcopyX2);


                 //end of for
	}
			

              if(I->getOpcode() != AArch64::INLINEASM && I->mayLoad() /*&& std::next(I) != E*/){
                /*
                errs()<< "HWISOO_DEBUG\n";
                I->dump();
                errs() << "Check\n F: "<< MF.getName() << "BB: " << I->getParent()->getName() << "Instr: " << *I << "\n";
                errs()<<I->getOpcode()<<"/"<<OR1K::NOP<<"\n";
                */
                DebugLoc DL3= I->getDebugLoc();
                const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();

                for (unsigned int opcount=0 ; opcount < I->getNumOperands() ;opcount++){ //
                  if(I->getOperand(opcount).isReg()) {
                    int reg = I->getOperand(opcount).getReg();
                    if(((isGPR64(reg)) || (isGPR32(reg))) && /*(I->getOpcode() == AArch64::LDRXpre || I->getOpcode() == AArch64::LDPXpre) && (I->getOperand(opcount).getReg() == AArch64::SP) &&**/ (I->getOperand(opcount).getReg() != AArch64::WZR) && (I->getOperand(opcount).getReg() != AArch64::XZR)) {
                      MachineInstr *copyMoveM = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(getSlvReg(I->getOperand(opcount).getReg())).addReg(I->getOperand(opcount).getReg()).addImm(0).addImm(0);
                      MBB->insertAfter(I,copyMoveM);
                      MachineInstr *copyMoveD = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(getSlvReg2(I->getOperand(opcount).getReg())).addReg(I->getOperand(opcount).getReg()).addImm(0).addImm(0);
                      MBB->insertAfter(I,copyMoveD);
                    }

                  }
                } //end of for

                }
              if(I->getOpcode() != AArch64::INLINEASM && I->mayStore() /*&& std::next(I) != E*/){
                /*
                errs()<< "HWISOO_DEBUG\n";
                I->dump();
                errs() << "Check\n F: "<< MF.getName() << "BB: " << I->getParent()->getName() << "Instr: " << *I << "\n";
                errs()<<I->getOpcode()<<"/"<<OR1K::NOP<<"\n";
                */
                DebugLoc DL3= I->getDebugLoc();
                const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();

                for (unsigned int opcount=0 ; opcount < I->getNumOperands() ;opcount++){ //
                  if(I->getOperand(opcount).isReg()) {
                    int reg = I->getOperand(opcount).getReg();
                    if(((isGPR64(reg)) || (isGPR32(reg))) && (I->getOpcode() == AArch64::STRXpre || I->getOpcode() == AArch64::STPXpre) && (I->getOperand(opcount).getReg() == AArch64::SP) && (I->getOperand(opcount).getReg() != AArch64::WZR) && (I->getOperand(opcount).getReg() != AArch64::XZR)) {
                      MachineInstr *copyMoveM = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(getSlvReg(I->getOperand(opcount).getReg())).addReg(I->getOperand(opcount).getReg()).addImm(0).addImm(0);
                      MBB->insertAfter(I,copyMoveM);
                      MachineInstr *copyMoveD = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(getSlvReg2(I->getOperand(opcount).getReg())).addReg(I->getOperand(opcount).getReg()).addImm(0).addImm(0);
                      MBB->insertAfter(I,copyMoveD);
                      break;
                    }

                  }
                } //end of for

              }

              }// end of for
            }//end of for
          }



	void checkFunctionCalls(MachineFunction &MF){
		const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
		int numBBs=0;
		//int i=0;
		for(MachineFunction::iterator MB = MF.begin(), MBE = MF.end(); (MB != MBE) && (numBBs < FUNCSIZE); ++MB, ++numBBs) {
			int i=0;
			if( MF.getName() == "main" && MB == MF.begin() ){
				for (MachineBasicBlock::iterator I=MB->begin(), E=MB->end(); I !=E ; ++I){
					if(i==1)
					{
						break;
					}
					for(unsigned int opcount=0; opcount < I->getNumOperands(); opcount++) {
					//if(I->getOperand(opcount).getReg() != AArch64::SP && I->getOperand(opcount).getReg() != AArch64::X15 && I->getOperand(opcount).getReg() != AArch64::X25){
						
					
						DebugLoc DL3= MB->begin()->getDebugLoc();
						MachineInstr *MIcopyX0 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X8).addReg(AArch64::X0).addImm(0).addImm(0);
						MB->insert(I, MIcopyX0);
						MIcopyX0 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X18).addReg(AArch64::X0).addImm(0).addImm(0);
						MB->insert(I, MIcopyX0);   
						MachineInstr *MIcopyX1 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X9).addReg(AArch64::X1).addImm(0).addImm(0);
						MB->insert(I, MIcopyX1);
						MIcopyX1 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X19).addReg(AArch64::X1).addImm(0).addImm(0);
						MB->insert(I, MIcopyX1); 
						MachineInstr *MIcopyX2 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X10).addReg(AArch64::X2).addImm(0).addImm(0);
						MB->insert(I, MIcopyX2);
						MIcopyX2 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X20).addReg(AArch64::X2).addImm(0).addImm(0);
						MB->insert(I, MIcopyX2);
						MachineInstr *MIcopyX3 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X11).addReg(AArch64::X3).addImm(0).addImm(0);
						MB->insert(I, MIcopyX3);
						MIcopyX3 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X21).addReg(AArch64::X3).addImm(0).addImm(0);
						MB->insert(I, MIcopyX3); 
						MachineInstr *MIcopyX4 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X12).addReg(AArch64::X4).addImm(0).addImm(0);
						MB->insert(I, MIcopyX4);
						MIcopyX4 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X22).addReg(AArch64::X4).addImm(0).addImm(0);
						MB->insert(I, MIcopyX4); 
						MachineInstr *MIcopyX5 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X13).addReg(AArch64::X5).addImm(0).addImm(0);
						MB->insert(I, MIcopyX5);
						MIcopyX5 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X23).addReg(AArch64::X5).addImm(0).addImm(0);
						MB->insert(I, MIcopyX5);
                                                MachineInstr *MIcopyX6 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X14).addReg(AArch64::X6).addImm(0).addImm(0);
                                                MB->insert(I, MIcopyX6);
                                                MIcopyX6 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X24).addReg(AArch64::X6).addImm(0).addImm(0);
                                                MB->insert(I, MIcopyX6);
                                                MachineInstr *MIcopyX7 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X15).addReg(AArch64::X7).addImm(0).addImm(0);
                                                MB->insert(I, MIcopyX7);
                                                MIcopyX7 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X25).addReg(AArch64::X7).addImm(0).addImm(0);
                                                MB->insert(I, MIcopyX7);
						//MachineInstr *MIcopyX29 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X16).addReg(AArch64::FP).addImm(0).addImm(0);
						//MB->insert(I, MIcopyX29);
						//MIcopyX29 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X26).addReg(AArch64::FP).addImm(0).addImm(0);
						//MB->insert(I, MIcopyX29);
						MachineInstr *MIcopyX30 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X17).addReg(AArch64::LR).addImm(0).addImm(0);
						MB->insert(I, MIcopyX30);
						MIcopyX30 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X27).addReg(AArch64::LR).addImm(0).addImm(0);
						MB->insert(I, MIcopyX30);
                                                MachineInstr *MIcopyX31 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X16).addReg(AArch64::SP).addImm(0).addImm(0);
                                                MB->insert(I, MIcopyX31);
                                                MIcopyX31 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X26).addReg(AArch64::SP).addImm(0).addImm(0);
                                                MB->insert(I, MIcopyX31);
                                                i++;
						break;
						//}
						
					}
					
 				}
				
			}
                  if( MF.getName() != "main" && MB == MF.begin() ){
                    for (MachineBasicBlock::iterator I=MB->begin(), E=MB->end(); I !=E ; ++I){
                      if(i==1)
                      {
                        break;
                      }
                      for(unsigned int opcount=0; opcount < I->getNumOperands(); opcount++) {
                        if(I->getOperand(opcount).getReg() != AArch64::SP && I->getOperand(opcount).getReg() != AArch64::X16 && I->getOperand(opcount).getReg() != AArch64::X26){


                          DebugLoc DL3= MB->begin()->getDebugLoc();
                          /*MachineInstr *MIcopyX0 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X8).addReg(AArch64::X0).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX0);
                          MIcopyX0 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X18).addReg(AArch64::X0).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX0);
                          MachineInstr *MIcopyX1 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X9).addReg(AArch64::X1).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX1);
                          MIcopyX1 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X19).addReg(AArch64::X1).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX1);
                          MachineInstr *MIcopyX2 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X10).addReg(AArch64::X2).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX2);
                          MIcopyX2 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X20).addReg(AArch64::X2).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX2);
                          MachineInstr *MIcopyX3 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X11).addReg(AArch64::X3).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX3);
                          MIcopyX3 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X21).addReg(AArch64::X3).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX3);
                          MachineInstr *MIcopyX4 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X12).addReg(AArch64::X4).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX4);
                          MIcopyX4 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X22).addReg(AArch64::X4).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX4);
                          MachineInstr *MIcopyX5 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X13).addReg(AArch64::X5).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX5);
                          MIcopyX5 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X23).addReg(AArch64::X5).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX5);
                          MachineInstr *MIcopyX29 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X16).addReg(AArch64::FP).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX29);
                          MIcopyX29 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X26).addReg(AArch64::FP).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX29);*/
                          MachineInstr *MIcopyX30 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X17).addReg(AArch64::LR).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX30);
                          MIcopyX30 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X27).addReg(AArch64::LR).addImm(0).addImm(0);
                          MB->insert(I, MIcopyX30);
                          i++;
                          break;
                        }

                      }

                    }

                  }
		}
	}

/*

	MachineBasicBlock* insertVoting(MachineFunction &MF, MachineBasicBlock::iterator CI, unsigned int reg, MachineBasicBlock* lastCheck) {
		lastCheck = CI->getParent();
		DebugLoc DL3 = CI->getDebugLoc();
		const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();

		MachineBasicBlock *NewBB = MF.CreateMachineBasicBlock();
		NewBB->addSuccessor(lastCheck);
		NewBB->setHasAddressTaken();
		MachineFunction::iterator It = lastCheck->getIterator();
		MF.insert(It, NewBB);

		MachineInstr *cmpInst = BuildMI(MF, DL3, TII->get(AArch64::SUBSXrs)).addReg(AArch64::XZR).addReg(getSlvReg(reg)).addReg(reg);
		lastCheck->insert(CI,cmpInst);

		MachineInstr *MItest = BuildMI(MF, DL3, TII->get(AArch64::Bcc)).addDef(AArch64CC::NE).addMBB(ErrorBB);
		lastCheck->insert(CI,MItest);

		MachineInstr *cmpInst2 = BuildMI(MF, DL3, TII->get(AArch64::SUBSXrs)).addReg(AArch64::XZR).addReg(getSlvReg(reg)).addReg(getSlvReg2(reg));
		lastCheck->insert(CI,cmpInst2);

		MachineInstr *MItest2 = BuildMI(MF, DL3, TII->get(AArch64::Bcc)).addDef(AArch64CC::EQ).addMBB(ErrorBB);
		lastCheck->insert(CI,MItest2);

		MachineInstr *movInst = BuildMI(MF, DL3, TII->get(AArch64::ADDXri)).addReg(getSlvReg2(reg)).addReg(reg).addImm(0);
		lastCheck->insert(CI,movInst);	

		return NULL;
}

	MachineBasicBlock* splitBlockAfterInstr(MachineInstr *MI, MachineFunction &MF) {
		MachineBasicBlock *NewBB = MF.CreateMachineBasicBlock((MI->getParent())->getBasicBlock());
		MachineFunction::iterator MBBI = (MI->getParent())->getIterator();
		(MI->getParent())->addSuccessor(NewBB);

		++MBBI;
		MF.insert(MBBI, NewBB);

		MachineBasicBlock::iterator T = (MI->getParent())->instr_end(), E= (MI->getParent())->instr_end();
		if(T != (MI->getParent())->instr_begin()) T--;
		if(MI != E && MI != T) {
			NewBB->splice(NewBB->end(), (MI->getParent()), MI, (MI->getParent())->end());
		}
		else
		{
			errs() << "FIX it by hand. I can not split this basicblock\n F: " << MF.getName() << "BB: " << MI->getParent()->getName() << "Instr: " << *MI << "\n";
			NewBB->splice(NewBB->end(), (MI->getParent()), MI, (MI->getParent())->end());
		}
		
	

		return NewBB;
	}

	void printInstruction (MachineBasicBlock::iterator I)
			{
				errs() << "Instruction is: " << *I << " and it has operands: " << I->getNumOperands() <<'\n';
				for (int k=0; k< I->getNumOperands(); k++)
				{
					if (I->getOperand(k).isReg() ) errs() << "op("<<k<<") is register: "<<I->getOperand(k).getReg()<<'\n';
					else if (I->getOperand(k).isImm()){ errs() <<" op("<<k<<") is Immediate: "<<I->getOperand(k).getImm()<<'\n';}
					else if (I->getOperand(k).isCImm()){	errs() << "op("<<k<<") is Constant Immediate: "<<I->getOperand(k).getCImm()<<'\n';}
					else if (I->getOperand(k).isMBB()) {errs() << "op("<<k<<") is Machine BasicBlock: "<<I->getOperand(k).getMBB()->getFullName() <<" Number("<<I->getOperand(k).getMBB()->getNumber()<< "), AND TYPE IS : "<<I->getOperand(k).getType() << '\n';}
					else if (I->getOperand(k).isSymbol()) {errs() << "op("<<k<<") is symbol: "<<I->getOperand(k).getSymbolName() <<" , AND TYPE IS : "<<I->getOperand(k).getType() << '\n';}
					else if (I->getOperand(k).isMCSymbol()) {errs() << "op("<<k<<") is machine symbol: "<<I->getOperand(k).getSymbolName() <<" , AND TYPE IS : "<<I->getOperand(k).getSymbolName() << '\n';}

					else if (I->getOperand(k).isFPImm()) {errs() << "op("<<k<<") is FT Imm: "<<I->getOperand(k).getFPImm() <<" , AND TYPE IS : "<<I->getOperand(k).getType() << '\n';}
					else if (I->getOperand(k).isBlockAddress()) {errs() << "op("<<k<<") is Block Address: "<<I->getOperand(k).getBlockAddress() <<" , AND TYPE IS : "<<I->getOperand(k).getType() << '\n';}

					else if (I->getOperand(k).isGlobal()) {errs() << "op("<<k<<") is Global: "<<I->getOperand(k).getGlobal () <<" , AND TYPE IS : "<<I->getOperand(k).getType() << '\n';}

					else if (I->getOperand(k).isMetadata()) {errs() << "op("<<k<<") is Metadata: "<<I->getOperand(k).getMetadata() <<" , AND TYPE IS : "<<I->getOperand(k).getType() << '\n';}

					else
						errs() << "op("<<k<<") Type is: " << I->getOperand(k).getType() << '\n';



				}
			}			
**/

void insertSWIFTDelay(MachineFunction &MF) {
		const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
		for(MachineFunction::iterator MBB = MF.begin(), MBE = MF.end(); MBB != MBE; ++MBB) {
			//MachineBasicBlock* lastCheck = NULL;
			//MachineInstr* splitpoint=NULL;
			for(MachineBasicBlock::instr_iterator I=MBB->instr_begin(), E=MBB->instr_end(); I != E; ++I) {

				DebugLoc DL3 = I->getDebugLoc();
				if(I->mayStore() || I->mayLoad()) {
					for(unsigned int opcount=0; opcount < I->getNumOperands(); opcount++) {
						if(I->getOperand(opcount).isReg()) {
							int reg = I->getOperand(opcount).getReg();
							if(isGPR64(reg) && !(I->mayLoad() && !opcount)) {

								MachineInstr *nopInst = BuildMI(MF, DL3, TII->get(AArch64::ADDXri)).addReg(reg).addReg(reg).addImm(0).addImm(0);
								MBB->insert(I,nopInst);

								
								}

							if(isGPR32(reg) && !(I->mayLoad() && !opcount)) {

								MachineInstr *nopInst = BuildMI(MF, DL3, TII->get(AArch64::ADDXri)).addReg(reg).addReg(reg).addImm(0).addImm(0);
								MBB->insert(I,nopInst);

								
								}
							}
						}
					}
				}
			}
		}




	void insertSWIFTErrorDetectors(MachineFunction &MF) {
		const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
		for(MachineFunction::iterator MBB = MF.begin(), MBE = MF.end(); MBB != MBE; ++MBB) {
			//MachineBasicBlock* lastCheck = NULL;
			//MachineInstr* splitpoint=NULL;
			for(MachineBasicBlock::instr_iterator I=MBB->instr_begin(), E=MBB->instr_end(); I != E; ++I) {

				DebugLoc DL3 = I->getDebugLoc();
				

				if(I->mayStore() || I->mayLoad() || I->isCall()) {
					for(unsigned int opcount=0; opcount < I->getNumOperands(); opcount++) {
						if(I->getOperand(opcount).isReg()) {
							int reg = I->getOperand(opcount).getReg();
							if(isGPR64(reg) && !(I->mayLoad() && !opcount) && (I->getOperand(opcount).getReg() != AArch64::SP) && (I->getOperand(opcount).getReg() != AArch64::WZR) && (I->getOperand(opcount).getReg() != AArch64::XZR)) {

								//MachineInstr *nopInst = BuildMI(MF, DL3, TII->get(AArch64::ADDXri)).addReg(reg).addReg(reg).addImm(0).addImm(0);
								//MBB->insert(I,nopInst);
								//MachineInstr *extInst = BuildMI(MF, DL3, TII->get(AArch64::CSINCXr)).addReg(AArch64::X6).addUse(AArch64::XZR).addUse(AArch64::XZR).addImm(AArch64CC::NE);
								//MBB->insert(I,extInst);

								MachineInstr *cmpInst = BuildMI(MF, DL3, TII->get(AArch64::SUBSXrs)).addReg(AArch64::XZR).addReg(reg).addReg(getSlvReg(reg)).addImm(0);
								MBB->insert(I,cmpInst);
						
								MachineInstr *MItest = BuildMI(MF, DL3, TII->get(AArch64::CSELXr)).addReg(getSlvReg2(reg)).addReg(getSlvReg(reg)).addReg(getSlvReg2(reg)).addDef(AArch64CC::EQ);
								MBB->insert(I,MItest);

								MachineInstr *cmpInst2 = BuildMI(MF, DL3, TII->get(AArch64::SUBSXrs)).addReg(AArch64::XZR).addReg(reg).addReg(getSlvReg2(reg)).addImm(0);
								MBB->insert(I,cmpInst2);

								MachineInstr *MItest2 = BuildMI(MF, DL3, TII->get(AArch64::CSELXr)).addReg(getSlvReg(reg)).addReg(getSlvReg2(reg)).addReg(getSlvReg(reg)).addDef(AArch64CC::EQ);
								MBB->insert(I,MItest2);

								MachineInstr *cmpInst3 = BuildMI(MF, DL3, TII->get(AArch64::SUBSXrs)).addReg(AArch64::XZR).addReg(getSlvReg(reg)).addReg(getSlvReg2(reg)).addImm(0);
								MBB->insert(I,cmpInst3);

								MachineInstr *MItest3 = BuildMI(MF, DL3, TII->get(AArch64::CSELXr)).addReg(reg).addReg(getSlvReg(reg)).addReg(reg).addDef(AArch64CC::EQ);
								MBB->insert(I,MItest3);

								//MachineInstr *extInst2 = BuildMI(MF, DL3, TII->get(AArch64::SUBSXri)).addReg(AArch64::XZR).addReg(AArch64::X6).addImm(1).addImm(0);
								//MBB->insert(I,extInst2);
								//splitpoint=movInst;

								//MachineBasicBlock* newBB = splitBlockAfterInstr(splitpoint, MF);
								//newBB->setHasAddressTaken();

								//lastCheck = InsertVoting(MF, I, reg, lastCheck);
								//MBB->splitSuccessor(MachineBasicBlock *I, MachineBasicBlock* New, bool NormalizeSuccProbs=false);
								
								}

							if(isGPR32(reg) && !(I->mayLoad() && !opcount) && (I->getOperand(opcount).getReg() != AArch64::SP) && (I->getOperand(opcount).getReg() != AArch64::WZR) && (I->getOperand(opcount).getReg() != AArch64::XZR)) {

								//MachineInstr *nopInst = BuildMI(MF, DL3, TII->get(AArch64::ADDXri)).addReg(reg).addReg(reg).addImm(0).addImm(0);
								//MBB->insert(I,nopInst);

								//MachineInstr *extInst = BuildMI(MF, DL3, TII->get(AArch64::CSINCWr)).addReg(AArch64::W6).addUse(AArch64::WZR).addUse(AArch64::WZR).addImm(AArch64CC::NE);
								//MBB->insert(I,extInst);

								MachineInstr *cmpInst = BuildMI(MF, DL3, TII->get(AArch64::SUBSXrs)).addReg(AArch64::WZR).addReg(reg).addReg(getSlvReg(reg)).addImm(0);
								MBB->insert(I,cmpInst);
						
								MachineInstr *MItest = BuildMI(MF, DL3, TII->get(AArch64::CSELXr)).addReg(getSlvReg2(reg)).addReg(getSlvReg(reg)).addReg(getSlvReg2(reg)).addDef(AArch64CC::EQ);
								MBB->insert(I,MItest);

								MachineInstr *cmpInst2 = BuildMI(MF, DL3, TII->get(AArch64::SUBSXrs)).addReg(AArch64::WZR).addReg(reg).addReg(getSlvReg2(reg)).addImm(0);
								MBB->insert(I,cmpInst2);

								MachineInstr *MItest2 = BuildMI(MF, DL3, TII->get(AArch64::CSELXr)).addReg(getSlvReg(reg)).addReg(getSlvReg2(reg)).addReg(getSlvReg(reg)).addDef(AArch64CC::EQ);
								MBB->insert(I,MItest2);

								MachineInstr *cmpInst3 = BuildMI(MF, DL3, TII->get(AArch64::SUBSXrs)).addReg(AArch64::WZR).addReg(getSlvReg(reg)).addReg(getSlvReg2(reg)).addImm(0);
								MBB->insert(I,cmpInst3);

								MachineInstr *MItest3 = BuildMI(MF, DL3, TII->get(AArch64::CSELXr)).addReg(reg).addReg(getSlvReg(reg)).addReg(reg).addDef(AArch64CC::EQ);
								MBB->insert(I,MItest3);

								//MachineInstr *extInst2 = BuildMI(MF, DL3, TII->get(AArch64::SUBSXri)).addReg(AArch64::WZR).addReg(AArch64::W6).addImm(1).addImm(0);
								//MBB->insert(I,extInst2);
								//splitpoint=movInst;

								//MachineBasicBlock* newBB = splitBlockAfterInstr(splitpoint, MF);
								//newBB->setHasAddressTaken();

								//lastCheck = InsertVoting(MF, I, reg, lastCheck);
								//MBB->splitSuccessor(MachineBasicBlock *I, MachineBasicBlock* New, bool NormalizeSuccProbs=false);
								
								}
                                                        if(isGPR64(reg) && !(I->mayLoad() && !opcount) && (I->getOperand(opcount).getReg() == AArch64::SP) && (I->getOperand(opcount).getReg() != AArch64::WZR) && (I->getOperand(opcount).getReg() != AArch64::XZR)) {

                                                          MachineInstr *cmpInst = BuildMI(MF, DL3, TII->get(AArch64::SUBSXrs)).addReg(AArch64::XZR).addReg(reg).addReg(getSlvReg(reg)).addImm(0);
                                                          MBB->insert(I,cmpInst);

                                                          MachineInstr *MItest = BuildMI(MF, DL3, TII->get(AArch64::CSELXr)).addReg(getSlvReg2(reg)).addReg(getSlvReg(reg)).addReg(getSlvReg2(reg)).addDef(AArch64CC::EQ);
                                                          MBB->insert(I,MItest);

                                                          MachineInstr *cmpInst2 = BuildMI(MF, DL3, TII->get(AArch64::SUBSXrs)).addReg(AArch64::XZR).addReg(reg).addReg(getSlvReg2(reg)).addImm(0);
                                                          MBB->insert(I,cmpInst2);

                                                          MachineInstr *MItest2 = BuildMI(MF, DL3, TII->get(AArch64::CSELXr)).addReg(getSlvReg(reg)).addReg(getSlvReg2(reg)).addReg(getSlvReg(reg)).addDef(AArch64CC::EQ);
                                                          MBB->insert(I,MItest2);

                                                          MachineInstr *cmpInst3 = BuildMI(MF, DL3, TII->get(AArch64::SUBSXrs)).addReg(AArch64::XZR).addReg(getSlvReg(reg)).addReg(getSlvReg2(reg)).addImm(0);
                                                          MBB->insert(I,cmpInst3);

                                                          MachineInstr *MIcopyX0 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(AArch64::X28).addReg(reg).addImm(0).addImm(0);
                                                          MBB->insert(I, MIcopyX0);

                                                          MachineInstr *MItest3 = BuildMI(MF, DL3, TII->get(AArch64::CSELXr)).addReg(AArch64::X28).addReg(getSlvReg(reg)).addReg(AArch64::X28).addDef(AArch64CC::EQ);
                                                          MBB->insert(I,MItest3);

                                                          MachineInstr *MIcopyX1 = BuildMI(MF, DL3 , TII->get(AArch64::ADDXri)).addReg(reg).addReg(AArch64::X28).addImm(0).addImm(0);
                                                          MBB->insert(I, MIcopyX1);
                                                        }
							}
						}
					}
				}
			}
		}

	
          


	int FUNCSIZE;	
	MachineBasicBlock* ErrorBB=NULL;
	//MachineBasicBlock* insert=NULL;
	bool runOnMachineFunction(MachineFunction &MF) {

		if(EnableSwiftR) {

		FUNCSIZE=100000;
		//ErrorBB=makeErrorBB(MF);
		duplicateInstructions(MF);
		//insertVotingOperations(MF);
		//insertSWIFTDelay(MF);
                copyLoadValuesSWIFTR(MF);
		checkFunctionCalls(MF);
		insertSWIFTErrorDetectors(MF);
	}
		return true;
        }


        };



char Swift::ID = 0;
FunctionPass *createSwiftPass() 
{
	/*
	ZDC *newZDC= new ZDC();
	newZDC->addPZDCMF(newMF);
	return newZDC;
	*/
	return new Swift();
}

}
		  
