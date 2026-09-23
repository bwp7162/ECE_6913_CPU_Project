#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;

#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space reason, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
	bitset<32>  PC;
	bool        nop;
};

struct IDStruct {
	bitset<32>  Instr;
	bool        nop;
};

struct EXStruct {
	bitset<32>  Read_data1;
	bitset<32>  Read_data2;
	bitset<20>	Imm20;
	bitset<16>  Imm16;
	bitset<12>	Imm12;
	bitset<5>   Rs;
	bitset<5>   Rt;
	bitset<5>   Wrt_reg_addr;
	char		Instruction;
	bool		jbflag = 0;
	bool		riwflag = 0;
	bool        is_I_type;
	bool        rd_mem;
	bool        wrt_mem;
	bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
	bool        wrt_enable;
	bool        nop;
};

struct MEMStruct {
	bitset<32>  ALUresult;
	bitset<32>  Store_data;
	bitset<5>   Rs;
	bitset<5>   Rt;
	bitset<5>   Wrt_reg_addr;
	char		Instruction;
	bool		jbflag = 0;
	bool		riwflag = 0;
	bool        rd_mem;
	bool        wrt_mem;
	bool        wrt_enable;
	bool        nop;
};

struct WBStruct {
	bitset<32>  Wrt_data;
	bitset<5>   Rs;
	bitset<5>   Rt;
	bitset<5>   Wrt_reg_addr;
	bool		jbflag = 0;
	bool		riwflag = 0;
	bool        wrt_enable;
	bool        nop;
};

struct stateStruct {
	IFStruct    IF;
	IDStruct    ID;
	EXStruct    EX;
	MEMStruct   MEM;
	WBStruct    WB;
};

class InsMem
{
public:
	string id, ioDir;
	InsMem(string name, string ioDir) {
		id = name;
		IMem.resize(MemSize);
		ifstream imem;
		string line;
		int i = 0;
		imem.open(ioDir + "\\imem.txt");
		if (imem.is_open())
		{
			while (getline(imem, line))
			{
				IMem[i] = bitset<8>(line);
				i++;
			}
		}
		else cout << "Unable to open IMEM input file.";
		imem.close();
	}

	bitset<32> readInstr(bitset<32> ReadAddress) {
		// read instruction memory
		// return bitset<32> val
		int PCCount;
		vector <bitset<8> > Ins_emv;
		Ins_emv.resize(4);
		PCCount = (int)(ReadAddress.to_ulong());
		for (int i = 0; i < 4; i++)
		{
			Ins_emv[i] = IMem[i + PCCount];
		}
		string bitsets = Ins_emv[0].to_string() + Ins_emv[1].to_string() + Ins_emv[2].to_string() + Ins_emv[3].to_string();
		bitset<32>Ins_extmem(bitsets);
		return Ins_extmem;
	}

private:
	vector<bitset<8> > IMem;
};

class DataMem
{
public:
	string id, opFilePath, ioDir;
	DataMem(string name, string ioDir) : id{ name }, ioDir{ ioDir } {
		DMem.resize(MemSize);
		opFilePath = ioDir + "\\" + name + "_DMEMResult.txt";
		ifstream dmem;
		string line;
		int i = 0;
		dmem.open(ioDir + "\\dmem.txt");
		if (dmem.is_open())
		{
			while (getline(dmem, line))
			{
				DMem[i] = bitset<8>(line);
				i++;
			}
		}
		else cout << "Unable to open DMEM input file.";
		dmem.close();
	}

	bitset<32> readDataMem(bitset<32> Address) {
		// read data memory
		// return bitset<32> val
		int Addnum;
		vector <bitset<8> > Ins_demv;
		Ins_demv.resize(4);
		Addnum = (int)(Address.to_ulong());
		for (int i = 0; i < 4; i++)
		{
			Ins_demv[i] = DMem[i + Addnum];
		}
		string bitsets = Ins_demv[0].to_string() + Ins_demv[1].to_string() + Ins_demv[2].to_string() + Ins_demv[3].to_string();
		bitset<32>Ins_dextmem(bitsets);
		return Ins_dextmem;
	}

	void writeDataMem(bitset<32> Address, bitset<32> WriteData) {
		// write into memory
		int Addnum;
		vector <bitset<8> > Ins_demv;
		Ins_demv.resize(4);
		Addnum = (int)(Address.to_ulong());
		string set1, set2, set3, set4, DataString;
		DataString = WriteData.to_string();
		set1 = DataString.substr(0, 8);
		set2 = DataString.substr(8, 8);
		set3 = DataString.substr(16, 8);
		set4 = DataString.substr(24, 8);
		Ins_demv[0] = bitset<8>(set1);
		Ins_demv[1] = bitset<8>(set2);
		Ins_demv[2] = bitset<8>(set3);
		Ins_demv[3] = bitset<8>(set4);

		for (int i = 0; i < 4; i++)
		{
			DMem[i + Addnum] = Ins_demv[i];
		} 
	}

	void outputDataMem() {
		ofstream dmemout;
		dmemout.open(opFilePath, std::ios_base::trunc);
		if (dmemout.is_open()) {
			for (int j = 0; j < 1000; j++)
			{
				dmemout << DMem[j] << endl;
			}

		}
		else cout << "Unable to open " << id << " DMEM result file." << endl;
		dmemout.close();
	}

private:
	vector<bitset<8> > DMem;
};

class RegisterFile
{
public:
	string outputFile;
	RegisterFile(string ioDir) : outputFile{ ioDir + "RFResult.txt" } {
		Registers.resize(32);
		Registers[0] = bitset<32>(0);
	}

	bitset<32> readRF(bitset<5> Reg_addr) {
		// Fill in
		int Index;
		Index = (int)(Reg_addr.to_ulong());
		return Registers[Index];
	}

	void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data) {
		// Fill in
		int Index;
		Index = (int)(Reg_addr.to_ulong());
		Registers[Index] = Wrt_reg_data;
	}

	void outputRF(int cycle) {
		ofstream rfout;
		if (cycle == 0)
			rfout.open(outputFile, std::ios_base::trunc);
		else
			rfout.open(outputFile, std::ios_base::app);
		if (rfout.is_open())
		{
			rfout << "State of RF after executing cycle:\t" << cycle << endl;
			for (int j = 0; j < 32; j++)
			{
				rfout << Registers[j] << endl;
			}
		}
		else cout << "Unable to open RF output file." << endl;
		rfout.close();
	}

private:
	vector<bitset<32> >Registers;
};

class Core {
public:
	RegisterFile myRF;
	uint32_t cycle = 0;
	bool halted = false;
	string ioDir;
	struct stateStruct state, nextState;
	InsMem ext_imem;
	DataMem ext_dmem;

	Core(string ioDir, InsMem& imem, DataMem& dmem) : myRF(ioDir), ioDir{ ioDir }, ext_imem{ imem }, ext_dmem{ dmem } {}

	virtual void step() {}

	virtual void printState() {}
};

class SingleStageCore : public Core {
public:
	SingleStageCore(string ioDir, InsMem& imem, DataMem& dmem) : Core(ioDir + "\\SS_", imem, dmem), opFilePath(ioDir + "\\StateResult_SS.txt") {}
	
	bool haltflag = 0;

	void step() {
		/* Your implementation*/
		
		//Instruction Fetch
		state.ID.Instr = ext_imem.readInstr(state.IF.PC);

		//Instruction Decode 
		char instruction = 'z';
		string Inst = state.ID.Instr.to_string();
		bool riwflag = 0;
		bool jbflag = 0;

		//R - Type
		if (Inst.substr(25, 7) == "0110011")
		{
			riwflag = 1;
			string rs1 = Inst.substr(12, 5);
			string rs2 = Inst.substr(7, 5);
			string rd = Inst.substr(20, 5);
			state.EX.Rs = bitset<5>(rs1);
			state.EX.Rt = bitset<5>(rs2);
			state.EX.Wrt_reg_addr = bitset<5>(rd);
			state.EX.Read_data1 = myRF.readRF(state.EX.Rs);
			state.EX.Read_data2 = myRF.readRF(state.EX.Rt);
			cout << "R - Type" << endl;
			if (Inst.substr(17, 3) == "000" && Inst.substr(0, 7) == "0000000")
			{
				cout << "Add" << endl;
				instruction = 'a';

			}
			else if (Inst.substr(17, 3) == "000" && Inst.substr(0, 7) == "0100000")
			{
				cout << "Sub" << endl;
				instruction = 'b';

			}
			else if (Inst.substr(17, 3) == "100" && Inst.substr(0, 7) == "0000000")
			{
				cout << "Xor" << endl;
				instruction = 'c';

			}
			else if (Inst.substr(17, 3) == "110" && Inst.substr(0, 7) == "0000000")
			{
				cout << "Or" << endl;
				instruction = 'd';
			}
			else if (Inst.substr(17, 3) == "111" && Inst.substr(0, 7) == "0000000")
			{
				cout << "And" << endl;
				instruction = 'e';
			}

		}
		//I - Type
		else if (Inst.substr(25, 7) == "0010011")
		{
			riwflag = 1;
			string rs1 = Inst.substr(12, 5);
			string rd = Inst.substr(20, 5);
			string imm = Inst.substr(0, 12);
			state.EX.Rs = bitset<5>(rs1);
			state.EX.Wrt_reg_addr = bitset<5>(rd);
			state.EX.Imm12 = bitset<12>(imm);
			state.EX.Read_data1 = myRF.readRF(state.EX.Rs);
			state.EX.Read_data2 = SignEx12to32(state.EX.Imm12);
			cout << "I - Type" << endl;
			if (Inst.substr(17, 3) == "000")
			{
				instruction = 'f';
			}
			else if (Inst.substr(17, 3) == "100")
			{
				instruction = 'g';
			}
			else if (Inst.substr(17, 3) == "110")
			{
				instruction = 'h';
			}
			else if (Inst.substr(17, 3) == "111")
			{
				instruction = 'i';
			}

		}
		//J - Type
		else if (Inst.substr(25, 7) == "1101111")
		{
			string rd = Inst.substr(20, 5);
			string imm = Inst.substr(0, 1) + Inst.substr(12, 8) + Inst.substr(11, 1) + Inst.substr(1, 10);
			state.EX.Wrt_reg_addr = bitset<5>(rd);
			state.EX.Imm20 = bitset<20>(imm);
			state.EX.Read_data1 = SignEx20to32(state.EX.Imm20);
			state.EX.Read_data1 = state.EX.Read_data1 << 1;
			cout << "J - Type" << endl;
			instruction = 'j';
		}
		//B - Type
		else if (Inst.substr(25, 7) == "1100011")
		{
			string rs1 = Inst.substr(12, 5);
			string rs2 = Inst.substr(7, 5);
			string imm = Inst.substr(0, 1) + Inst.substr(24, 1) + Inst.substr(1, 6) + Inst.substr(20, 4);
			state.EX.Rs = bitset<5>(rs1);
			state.EX.Rt = bitset<5>(rs2);
			state.EX.Imm12 = bitset<12>(imm);
			state.EX.Read_data1 = myRF.readRF(state.EX.Rs);
			state.EX.Read_data2 = myRF.readRF(state.EX.Rt);
			state.MEM.Store_data = SignEx12to32(state.EX.Imm12);
			state.MEM.Store_data = state.MEM.Store_data << 1;
			cout << "B - Type" << endl;
			if (Inst.substr(17, 3) == "000")
			{
				instruction = 'k';
			}
			else if (Inst.substr(17, 3) == "001")
			{
				instruction = 'l';
			}

		}
		//I - Type lw
		else if (Inst.substr(25, 7) == "0000011")
		{
			string rs1 = Inst.substr(12, 5);
			string rd = Inst.substr(20, 5);
			string imm = Inst.substr(0, 12);
			state.EX.Rs = bitset<5>(rs1);
			state.EX.Wrt_reg_addr = bitset<5>(rd);
			state.EX.Imm12 = bitset<12>(imm);
			state.EX.Read_data1 = myRF.readRF(state.EX.Rs);
			state.EX.Read_data2 = SignEx12to32(state.EX.Imm12);
			cout << "I - Type LW" << endl;
			if (Inst.substr(17, 3) == "000")
			{
				instruction = 'm';
			}

		}
		//S - Type sw
		else if (Inst.substr(25, 7) == "0100011")
		{
			string rs1 = Inst.substr(12, 5);
			string rs2 = Inst.substr(7, 5);
			string imm = Inst.substr(0, 7) + Inst.substr(20, 5);
			state.EX.Rs = bitset<5>(rs1);
			state.EX.Rt = bitset<5>(rs2);
			state.EX.Imm12 = bitset<12>(imm);
			state.EX.Read_data1 = myRF.readRF(state.EX.Rs);
			state.EX.Read_data2 = SignEx12to32(state.EX.Imm12);
			cout << "S - Type SW" << endl;
			if (Inst.substr(17, 3) == "010")
			{
				instruction = 'n';
			}

		}
		//Halt
		else if (Inst.substr(25, 7) == "1111111")
		{
			instruction = 'o';

		}

		//Execute 
		switch (instruction)
		{
		case 'a': {
			unsigned long int x, y, z;
			x = state.EX.Read_data1.to_ulong();
			y = state.EX.Read_data2.to_ulong();
			z = x + y;
			state.MEM.ALUresult = bitset<32>(z);
			break;
		}
		case 'b': {
			unsigned long int x, y, z;
			x = state.EX.Read_data1.to_ulong();
			y = state.EX.Read_data2.to_ulong();
			z = x - y;
			state.MEM.ALUresult = bitset<32>(z);
			break;
		}

		case 'c': {
			state.MEM.ALUresult = state.EX.Read_data1 ^ state.EX.Read_data2;
			break;
		}
		case 'd': {
			state.MEM.ALUresult = state.EX.Read_data1 | state.EX.Read_data2;
			break;
		}
		case 'e': {
			state.MEM.ALUresult = state.EX.Read_data1 & state.EX.Read_data2;
			break;
		}
		case 'f': {
			unsigned long int x;
			int32_t y;
			signed long int z;
			x = state.EX.Read_data1.to_ulong();
			y = Bin32toSign(state.EX.Read_data2);
			z = x + y;
			state.MEM.ALUresult = bitset<32>(z);
			break;
		}
		case 'g': {
			state.MEM.ALUresult = state.EX.Read_data1 ^ state.EX.Read_data2;
			break;
		}
		case 'h': {
			state.MEM.ALUresult = state.EX.Read_data1 | state.EX.Read_data2;
			break;
		}
		case 'i': {
			state.MEM.ALUresult = state.EX.Read_data1 & state.EX.Read_data2;
			break;
		}
		case 'j': { // JAL
			int PCCount;
			int32_t immval;
			signed long int result;
			PCCount = (int)(state.IF.PC.to_ulong());
			immval = Bin32toSign(state.EX.Read_data1);
			result = PCCount + immval;
			state.IF.PC = bitset<32>(result);
			state.WB.Wrt_data = bitset<32>(PCCount + 4);
			state.WB.Wrt_reg_addr = state.EX.Wrt_reg_addr;
			myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
			jbflag = 1;
			break;
		}
		case 'k': { // BEQ
			if (state.EX.Read_data1 == state.EX.Read_data2)
			{
				int PCCount;
				int32_t immval;
				signed long int result;
				PCCount = (int)(state.IF.PC.to_ulong());
				immval = Bin32toSign(state.MEM.Store_data);
				result = PCCount + immval;
				state.IF.PC = bitset<32>(result);
				jbflag = 1;
			}
			break;
		}
		case 'l': { // BNEQ
			if (state.EX.Read_data1 != state.EX.Read_data2)
			{
				int PCCount;
				int32_t immval;
				signed long int result;
				PCCount = (int)(state.IF.PC.to_ulong());
				immval = Bin32toSign(state.MEM.Store_data);
				result = PCCount + immval;
				state.IF.PC = bitset<32>(result);
				jbflag = 1;
			}

			break;
		}
		case 'm': { // MEM Load lw
			unsigned long int x;
			int32_t y;
			signed long int z;
			x = state.EX.Read_data1.to_ulong();
			y = Bin32toSign(state.EX.Read_data2);
			z = x + y;
			state.MEM.ALUresult = bitset<32>(z);
			state.WB.Wrt_reg_addr = state.EX.Wrt_reg_addr;
			state.WB.Wrt_data = ext_dmem.readDataMem(state.MEM.ALUresult);
			myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
			break;
		}
		case 'n': { // MEM Store sw
			unsigned long int x;
			int32_t y;
			signed long int z;
			x = state.EX.Read_data1.to_ulong();
			y = Bin32toSign(state.EX.Read_data2);
			z = x + y;
			state.MEM.ALUresult = bitset<32>(z);
			state.MEM.Store_data = myRF.readRF(state.EX.Rt);
			ext_dmem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);

			break;
		}
		case 'o': { // HALT
			if (state.IF.nop == 1) {
				haltflag = 1;
			}
			state.IF.nop = 1;
			break;
		}
		default:
			cout << "NOP" << endl;
		}



		//Writeback
		//R - Type and I - Type
		if (riwflag == 1) 
		{
			state.WB.wrt_enable = 1;
			state.WB.Wrt_reg_addr = state.EX.Wrt_reg_addr;
			state.WB.Wrt_data = state.MEM.ALUresult;
			myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
		}
		if (!state.IF.nop) {
			if (jbflag == 0) {
				int PCint = (int)(state.IF.PC.to_ulong());
				PCint = PCint + 4;
				state.IF.PC = bitset<32>(PCint);
			}
		}

		//halted = true;
		if (state.IF.nop && haltflag)
			halted = true;
			
			myRF.outputRF(cycle); // dump RF
			printState(state, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
			ext_dmem.outputDataMem(); // dump data mem

		//state = nextState; // The end of the cycle and updates the current state with the values calculated in this cycle
		cycle++;
	}

	bitset<32> SignEx12to32(bitset<12> Imm)
	{
		int16_t num16;
		if (Imm.test(Imm.size() - 1))
		{
			num16 = -static_cast<int16_t>((~Imm).to_ulong() + 1);

		}
		else
		{
			num16 = static_cast<int16_t>(Imm.to_ulong());
		}

		int32_t num32 = num16;
		return bitset<32>(num32);
	}

	bitset<32> SignEx20to32(bitset<20> Imm)
	{
		signed long int num;
		if (Imm.test(Imm.size() - 1))
		{
			num = -static_cast<signed long int>((~Imm).to_ulong() + 1);

		}
		else
		{
			num = static_cast<signed long int>(Imm.to_ulong());
		}

		return bitset<32>(num);
	}

	int32_t Bin32toSign(bitset<32> bits)
	{
		int32_t num32;
		if (bits.test(bits.size() - 1))
		{
			num32 = -static_cast<int32_t>((~bits).to_ulong() + 1);

		}
		else
		{
			num32 = static_cast<int32_t>(bits.to_ulong());
		}

		return num32;
	}

	void printState(stateStruct state, int cycle) {
		ofstream printstate;
		if (cycle == 0)
			printstate.open(opFilePath, std::ios_base::trunc);
		else
			printstate.open(opFilePath, std::ios_base::app);
		if (printstate.is_open()) {
			printstate << "State after executing cycle:\t" << cycle << endl;

			printstate << "IF.PC:\t" << state.IF.PC.to_ulong() << endl;
			printstate << "IF.nop:\t" << state.IF.nop << endl;
		}
		else cout << "Unable to open SS StateResult output file." << endl;
		printstate.close();
	}
private:
	string opFilePath;
};

class FiveStageCore : public Core {
public:

	FiveStageCore(string ioDir, InsMem& imem, DataMem& dmem) : Core(ioDir + "\\FS_", imem, dmem), opFilePath(ioDir + "\\StateResult_FS.txt") {}
		
	void init() {
		state.IF.nop = 0;
		state.ID.nop = 1;
		state.EX.nop = 1;
		state.MEM.nop = 1;
		state.WB.nop = 1;
	}
	
	bool hf = 0;

	void step() {
		/* Your implementation */
		string Inst;
		/* --------------------- WB stage --------------------- */
		if (state.WB.nop == 0) 
		{
			if (state.WB.riwflag == 1)
			{
				state.WB.wrt_enable = 1;
				myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
			}
		}



		/* --------------------- MEM stage -------------------- */
		nextState.WB.nop = state.MEM.nop;
		if (state.MEM.nop == 0)
		{
			switch (state.MEM.Instruction)
			{
			case 'm': { // MEM Load lw
				state.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
				state.WB.Wrt_data = ext_dmem.readDataMem(state.MEM.ALUresult);
				myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
				break;
			}
			case 'n': { // MEM Store sw
				state.MEM.Store_data = myRF.readRF(state.MEM.Rt);
				ext_dmem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);

				break;
			}
			default:
				cout << "No MEM instruct" << endl;
				nextState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
				nextState.WB.Wrt_data = state.MEM.ALUresult;
			}

			nextState.WB.wrt_enable = state.MEM.wrt_enable;
			nextState.WB.riwflag = state.MEM.riwflag;
			nextState.WB.jbflag = state.MEM.jbflag;
		}

		/* --------------------- EX stage --------------------- */
		nextState.MEM.nop = state.EX.nop;
		if (state.EX.nop == 0)
		{
			switch (state.EX.Instruction)
			{
			case 'a': {
				unsigned long int x, y, z;
				x = state.EX.Read_data1.to_ulong();
				y = state.EX.Read_data2.to_ulong();
				z = x + y;
				nextState.MEM.ALUresult = bitset<32>(z);
				nextState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
				break;
			}
			case 'b': {
				unsigned long int x, y, z;
				x = state.EX.Read_data1.to_ulong();
				y = state.EX.Read_data2.to_ulong();
				z = x - y;
				nextState.MEM.ALUresult = bitset<32>(z);
				nextState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
				break;
			}

			case 'c': {
				nextState.MEM.ALUresult = state.EX.Read_data1 ^ state.EX.Read_data2;
				nextState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
				break;
			}
			case 'd': {
				nextState.MEM.ALUresult = state.EX.Read_data1 | state.EX.Read_data2;
				nextState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
				break;
			}
			case 'e': {
				nextState.MEM.ALUresult = state.EX.Read_data1 & state.EX.Read_data2;
				nextState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
				break;
			}
			case 'f': {
				unsigned long int x;
				int32_t y;
				signed long int z;
				x = state.EX.Read_data1.to_ulong();
				y = Bin32toSign(state.EX.Read_data2);
				z = x + y;
				nextState.MEM.ALUresult = bitset<32>(z);
				nextState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
				break;
			}
			case 'g': {
				nextState.MEM.ALUresult = state.EX.Read_data1 ^ state.EX.Read_data2;
				nextState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
				break;
			}
			case 'h': {
				nextState.MEM.ALUresult = state.EX.Read_data1 | state.EX.Read_data2;
				nextState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
				break;
			}
			case 'i': {
				nextState.MEM.ALUresult = state.EX.Read_data1 & state.EX.Read_data2;
				nextState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
				break;
			}
			case 'j': { // JAL WIP FOR 5 STAGE
				int PCCount;
				int32_t immval;
				signed long int result;
				PCCount = (int)(state.IF.PC.to_ulong());
				immval = Bin32toSign(state.EX.Read_data1);
				result = PCCount + immval;
				state.IF.PC = bitset<32>(result);
				state.WB.Wrt_data = bitset<32>(PCCount + 4);
				state.WB.Wrt_reg_addr = state.EX.Wrt_reg_addr;
				myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
				nextState.MEM.jbflag = 1;
				break;
			}
			case 'k': { // BEQ
				if (state.EX.Read_data1 == state.EX.Read_data2)
				{
					nextState.MEM.Store_data = SignEx12to32(nextState.EX.Imm12);
					nextState.MEM.Store_data = nextState.MEM.Store_data << 1;
					int PCCount;
					int32_t immval;
					signed long int result;
					PCCount = (int)(state.IF.PC.to_ulong());
					immval = Bin32toSign(state.MEM.Store_data);
					result = PCCount + immval;
					state.IF.PC = bitset<32>(result);
					nextState.MEM.jbflag = 1;
				}
				break;
			}
			case 'l': { // BNEQ
				if (state.EX.Read_data1 != state.EX.Read_data2)
				{
					nextState.MEM.Store_data = SignEx12to32(nextState.EX.Imm12);
					nextState.MEM.Store_data = nextState.MEM.Store_data << 1;
					int PCCount;
					int32_t immval;
					signed long int result;
					PCCount = (int)(state.IF.PC.to_ulong());
					immval = Bin32toSign(state.MEM.Store_data);
					result = PCCount + immval;
					state.IF.PC = bitset<32>(result);
					nextState.MEM.jbflag = 1;
				}

				break;
			}
			case 'm': { // MEM Load lw
				unsigned long int x;
				int32_t y;
				signed long int z;
				x = state.EX.Read_data1.to_ulong();
				y = Bin32toSign(state.EX.Read_data2);
				z = x + y;
				nextState.MEM.ALUresult = bitset<32>(z);
				nextState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
				break;
			}
			case 'n': { // MEM Store sw
				unsigned long int x;
				int32_t y;
				signed long int z;
				x = state.EX.Read_data1.to_ulong();
				y = Bin32toSign(state.EX.Read_data2);
				z = x + y;
				nextState.MEM.ALUresult = bitset<32>(z);
				nextState.MEM.Store_data = myRF.readRF(state.EX.Rt);
				break;
			}
			case 'o': { // HALT
				state.IF.nop = 1;
				break;
			}
			default:
				cout << "Error" << endl;
			}
			nextState.MEM.rd_mem = state.EX.rd_mem;
			nextState.MEM.wrt_mem = state.EX.wrt_mem;
			nextState.MEM.wrt_enable = state.EX.wrt_enable;
			nextState.MEM.Instruction = state.EX.Instruction;
			nextState.MEM.riwflag = state.EX.riwflag;

		}

		/* --------------------- ID stage --------------------- */
		nextState.EX.nop = state.ID.nop;
		if (state.ID.nop == 0)
		{
			Inst = state.ID.Instr.to_string();

			//R - Type
			if (Inst.substr(25, 7) == "0110011")
			{
				nextState.EX.is_I_type = 0;
				nextState.EX.rd_mem = 0;
				nextState.EX.wrt_mem = 0;
				nextState.EX.alu_op= 1;
				nextState.EX.wrt_enable = 1;
				nextState.EX.riwflag = 1;
				string rs1 = Inst.substr(12, 5);
				string rs2 = Inst.substr(7, 5);
				string rd = Inst.substr(20, 5);
				nextState.EX.Rs = bitset<5>(rs1);
				nextState.EX.Rt = bitset<5>(rs2);
				nextState.EX.Wrt_reg_addr = bitset<5>(rd);
				nextState.EX.Read_data1 = myRF.readRF(nextState.EX.Rs);
				nextState.EX.Read_data2 = myRF.readRF(nextState.EX.Rt);
				cout << "R - Type" << endl;
				if (Inst.substr(17, 3) == "000" && Inst.substr(0, 7) == "0000000")
				{
					cout << "Add" << endl;
					nextState.EX.Instruction = 'a';

				}
				else if (Inst.substr(17, 3) == "000" && Inst.substr(0, 7) == "0100000")
				{
					cout << "Sub" << endl;
					nextState.EX.Instruction = 'b';
					nextState.EX.alu_op = 0;

				}
				else if (Inst.substr(17, 3) == "100" && Inst.substr(0, 7) == "0000000")
				{
					cout << "Xor" << endl;
					nextState.EX.Instruction = 'c';

				}
				else if (Inst.substr(17, 3) == "110" && Inst.substr(0, 7) == "0000000")
				{
					cout << "Or" << endl;
					nextState.EX.Instruction = 'd';
				}
				else if (Inst.substr(17, 3) == "111" && Inst.substr(0, 7) == "0000000")
				{
					cout << "And" << endl;
					nextState.EX.Instruction = 'e';
				}

			}
			//I - Type
			else if (Inst.substr(25, 7) == "0010011")
			{
				nextState.EX.is_I_type = 1;
				nextState.EX.rd_mem = 0;
				nextState.EX.wrt_mem = 0;
				nextState.EX.alu_op = 1;
				nextState.EX.wrt_enable = 1;
				nextState.EX.riwflag = 1;
				string rs1 = Inst.substr(12, 5);
				string rd = Inst.substr(20, 5);
				string imm = Inst.substr(0, 12);
				nextState.EX.Rs = bitset<5>(rs1);
				nextState.EX.Wrt_reg_addr = bitset<5>(rd);
				nextState.EX.Imm12 = bitset<12>(imm);
				nextState.EX.Read_data1 = myRF.readRF(nextState.EX.Rs);
				nextState.EX.Read_data2 = SignEx12to32(nextState.EX.Imm12);
				cout << "I - Type" << endl;
				if (Inst.substr(17, 3) == "000")
				{
					nextState.EX.Instruction = 'f';
				}
				else if (Inst.substr(17, 3) == "100")
				{
					nextState.EX.Instruction = 'g';
				}
				else if (Inst.substr(17, 3) == "110")
				{
					nextState.EX.Instruction = 'h';
				}
				else if (Inst.substr(17, 3) == "111")
				{
					nextState.EX.Instruction = 'i';
				}

			}
			//J - Type
			else if (Inst.substr(25, 7) == "1101111")
			{
				nextState.EX.is_I_type = 0;
				nextState.EX.rd_mem = 0;
				nextState.EX.wrt_mem = 0;
				nextState.EX.alu_op = 0;
				nextState.EX.wrt_enable = 0;
				string rd = Inst.substr(20, 5);
				string imm = Inst.substr(0, 1) + Inst.substr(12, 8) + Inst.substr(11, 1) + Inst.substr(1, 10);
				nextState.EX.Wrt_reg_addr = bitset<5>(rd);
				nextState.EX.Imm20 = bitset<20>(imm);
				nextState.EX.Read_data1 = SignEx20to32(nextState.EX.Imm20);
				nextState.EX.Read_data1 = nextState.EX.Read_data1 << 1;
				cout << "J - Type" << endl;
				nextState.EX.Instruction = 'j';
			}
			//B - Type
			else if (Inst.substr(25, 7) == "1100011")
			{
				nextState.EX.is_I_type = 0;
				nextState.EX.rd_mem = 0;
				nextState.EX.wrt_mem = 0;
				nextState.EX.alu_op = 0;
				nextState.EX.wrt_enable = 0;
				string rs1 = Inst.substr(12, 5);
				string rs2 = Inst.substr(7, 5);
				string imm = Inst.substr(0, 1) + Inst.substr(24, 1) + Inst.substr(1, 6) + Inst.substr(20, 4);
				nextState.EX.Rs = bitset<5>(rs1);
				nextState.EX.Rt = bitset<5>(rs2);
				nextState.EX.Imm12 = bitset<12>(imm);
				nextState.EX.Read_data1 = myRF.readRF(nextState.EX.Rs);
				nextState.EX.Read_data2 = myRF.readRF(nextState.EX.Rt);
				cout << "B - Type" << endl;
				if (Inst.substr(17, 3) == "000")
				{
					nextState.EX.Instruction = 'k';
				}
				else if (Inst.substr(17, 3) == "001")
				{
					nextState.EX.Instruction = 'l';
				}

			}
			//I - Type lw
			else if (Inst.substr(25, 7) == "0000011")
			{
				nextState.EX.is_I_type = 1;
				nextState.EX.rd_mem = 1;
				nextState.EX.wrt_mem = 0;
				nextState.EX.alu_op = 1;
				nextState.EX.wrt_enable = 1;
				//nextState.EX.riwflag = 1;
				string rs1 = Inst.substr(12, 5);
				string rd = Inst.substr(20, 5);
				string imm = Inst.substr(0, 12);
				nextState.EX.Rs = bitset<5>(rs1);
				nextState.EX.Wrt_reg_addr = bitset<5>(rd);
				nextState.EX.Imm12 = bitset<12>(imm);
				nextState.EX.Read_data1 = myRF.readRF(nextState.EX.Rs);
				nextState.EX.Read_data2 = SignEx12to32(nextState.EX.Imm12);
				cout << "I - Type LW" << endl;
				if (Inst.substr(17, 3) == "000")
				{
					nextState.EX.Instruction = 'm';
				}

			}
			//S - Type sw
			else if (Inst.substr(25, 7) == "0100011")
			{
				nextState.EX.is_I_type = 0;
				nextState.EX.rd_mem = 0;
				nextState.EX.wrt_mem = 1;
				nextState.EX.alu_op = 1;
				nextState.EX.wrt_enable = 0;
				string rs1 = Inst.substr(12, 5);
				string rs2 = Inst.substr(7, 5);
				string imm = Inst.substr(0, 7) + Inst.substr(20, 5);
				nextState.EX.Rs = bitset<5>(rs1);
				nextState.EX.Rt = bitset<5>(rs2);
				nextState.EX.Imm12 = bitset<12>(imm);
				nextState.EX.Read_data1 = myRF.readRF(nextState.EX.Rs);
				nextState.EX.Read_data2 = SignEx12to32(nextState.EX.Imm12);
				cout << "S - Type SW" << endl;
				if (Inst.substr(17, 3) == "010")
				{
					nextState.EX.Instruction = 'n';
				}

			}
			//Halt
			else if (Inst.substr(25, 7) == "1111111")
			{
				nextState.EX.is_I_type = 0;
				nextState.EX.rd_mem = 0;
				nextState.EX.wrt_mem = 0;
				nextState.EX.alu_op = 0;
				nextState.EX.wrt_enable = 0;
				nextState.EX.Instruction = 'o';
				cout << "Halt" << endl;

			}
			/*if (nextState.MEM.Wrt_reg_addr == nextState.EX.Rs)
			{
				nextState.EX.Read_data1 = nextState.MEM.ALUresult;
			}
			if (nextState.MEM.Wrt_reg_addr == nextState.EX.Rt)
			{
				nextState.EX.Read_data2 = nextState.MEM.ALUresult;
			}*/

		}


		/* --------------------- IF stage --------------------- */
		nextState.ID.nop = state.IF.nop;
		if (state.IF.nop == 0)
		{
			nextState.ID.Instr = ext_imem.readInstr(state.IF.PC);
			int PCint = (int)(state.IF.PC.to_ulong());
			PCint = PCint + 4;
			nextState.IF.PC = bitset<32>(PCint);
			if (nextState.ID.Instr.to_string().substr(25, 7) == "1111111")
			{
				nextState.IF.PC = state.IF.PC;
				nextState.IF.nop = 1;
				nextState.ID.nop = 1;
			}
		}

		if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop) {
			if (hf == 1)
			{
				halted = true;
			}
			hf = 1;
		}


		myRF.outputRF(cycle); // dump RF
		printState(nextState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 

		state = nextState; //The end of the cycle and updates the current state with the values calculated in this cycle
		cycle++;
	}

	bitset<32> SignEx12to32(bitset<12> Imm)
	{
		int16_t num16;
		if (Imm.test(Imm.size() - 1))
		{
			num16 = -static_cast<int16_t>((~Imm).to_ulong() + 1);

		}
		else
		{
			num16 = static_cast<int16_t>(Imm.to_ulong());
		}

		int32_t num32 = num16;
		return bitset<32>(num32);
	}

	bitset<32> SignEx20to32(bitset<20> Imm)
	{
		signed long int num;
		if (Imm.test(Imm.size() - 1))
		{
			num = -static_cast<signed long int>((~Imm).to_ulong() + 1);

		}
		else
		{
			num = static_cast<signed long int>(Imm.to_ulong());
		}

		return bitset<32>(num);
	}

	int32_t Bin32toSign(bitset<32> bits)
	{
		int32_t num32;
		if (bits.test(bits.size() - 1))
		{
			num32 = -static_cast<int32_t>((~bits).to_ulong() + 1);

		}
		else
		{
			num32 = static_cast<int32_t>(bits.to_ulong());
		}

		return num32;
	}

	void printState(stateStruct state, int cycle) {
		ofstream printstate;
		if (cycle == 0)
			printstate.open(opFilePath, std::ios_base::trunc);
		else
			printstate.open(opFilePath, std::ios_base::app);
		if (printstate.is_open()) {
			printstate << "State after executing cycle:\t" << cycle << endl;

			printstate << "IF.PC:\t" << state.IF.PC.to_ulong() << endl;
			printstate << "IF.nop:\t" << state.IF.nop << endl;

			printstate << "ID.Instr:\t" << state.ID.Instr << endl;
			printstate << "ID.nop:\t" << state.ID.nop << endl;

			printstate << "EX.Read_data1:\t" << state.EX.Read_data1 << endl;
			printstate << "EX.Read_data2:\t" << state.EX.Read_data2 << endl;
			printstate << "EX.Imm20:\t" << state.EX.Imm20 << endl;
			printstate << "EX.Imm16:\t" << state.EX.Imm16 << endl;
			printstate << "EX.Imm12:\t" << state.EX.Imm12 << endl;
			printstate << "EX.Rs:\t" << state.EX.Rs << endl;
			printstate << "EX.Rt:\t" << state.EX.Rt << endl;
			printstate << "EX.Wrt_reg_addr:\t" << state.EX.Wrt_reg_addr << endl;
			printstate << "EX.is_I_type:\t" << state.EX.is_I_type << endl;
			printstate << "EX.rd_mem:\t" << state.EX.rd_mem << endl;
			printstate << "EX.wrt_mem:\t" << state.EX.wrt_mem << endl;
			printstate << "EX.alu_op:\t" << state.EX.alu_op << endl;
			printstate << "EX.wrt_enable:\t" << state.EX.wrt_enable << endl;
			printstate << "EX.nop:\t" << state.EX.nop << endl;

			printstate << "MEM.ALUresult:\t" << state.MEM.ALUresult << endl;
			printstate << "MEM.Store_data:\t" << state.MEM.Store_data << endl;
			printstate << "MEM.Rs:\t" << state.MEM.Rs << endl;
			printstate << "MEM.Rt:\t" << state.MEM.Rt << endl;
			printstate << "MEM.Wrt_reg_addr:\t" << state.MEM.Wrt_reg_addr << endl;
			printstate << "MEM.rd_mem:\t" << state.MEM.rd_mem << endl;
			printstate << "MEM.wrt_mem:\t" << state.MEM.wrt_mem << endl;
			printstate << "MEM.wrt_enable:\t" << state.MEM.wrt_enable << endl;
			printstate << "MEM.nop:\t" << state.MEM.nop << endl;

			printstate << "WB.Wrt_data:\t" << state.WB.Wrt_data << endl;
			printstate << "WB.Rs:\t" << state.WB.Rs << endl;
			printstate << "WB.Rt:\t" << state.WB.Rt << endl;
			printstate << "WB.Wrt_reg_addr:\t" << state.WB.Wrt_reg_addr << endl;
			printstate << "WB.wrt_enable:\t" << state.WB.wrt_enable << endl;
			printstate << "WB.nop:\t" << state.WB.nop << endl;
		}
		else cout << "Unable to open FS StateResult output file." << endl;
		printstate.close();
	}
private:
	string opFilePath;
};


int main(int argc, char* argv[]) {

	string ioDir = "";
	if (argc == 1) {
		cout << "Enter path containing the memory files: ";
		cin >> ioDir;
	}
	else if (argc > 2) {
		cout << "Invalid number of arguments. Machine stopped." << endl;
		return -1;
	}
	else {
		ioDir = argv[1];
		cout << "IO Directory: " << ioDir << endl;
	}

	InsMem imem = InsMem("Imem", ioDir);
	DataMem dmem_ss = DataMem("SS", ioDir);
	DataMem dmem_fs = DataMem("FS", ioDir);

	SingleStageCore SSCore(ioDir, imem, dmem_ss);
	FiveStageCore FSCore(ioDir, imem, dmem_fs);
	
	FSCore.init(); //Initialize the nops so IF is 0 and rest are 1

	while (1) {
		if (!SSCore.halted)
			//SSCore.step();
			SSCore.halted = 1;

		if (!FSCore.halted)
			FSCore.step();

		if (SSCore.halted && FSCore.halted)
			break;
	}

	// dump SS and FS data mem.
	//dmem_ss.outputDataMem();
	dmem_fs.outputDataMem();

	return 0;
}