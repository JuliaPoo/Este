#include "Este\bb.hpp"
#include "Este\proc.hpp"
#include "Este\utils.hpp"
#include "Este\image.hpp"
#include "Este\rtn.hpp"
#include "Este\errors.hpp"

#ifdef TARGET_IA32E
#define XED_REG_INSTPTR XED_REG_RIP
#elif (defined TARGET_IA32)
#define XED_REG_INSTPTR XED_REG_EIP
#else
#error "Architecture not defined!"
#endif

using namespace Ctx;

Bb::Bb() {};

Bb::Bb(const CONTEXT* pinctx, const Proc* proc, const ADDRINT low_addr, const uint32_t size)
{
	this->idx = proc->getNumBb();
	this->addr = low_addr;
	this->size = size;
	this->bytes.assign(reinterpret_cast<char*>(low_addr), reinterpret_cast<char*>(low_addr + size));

	const auto img = proc->getImageExecutable(low_addr);
	if (img) {
		this->image_idx = img->getIdx();
		this->section_idx = img->getExecutableSectionIdx(low_addr);
	}
	else {
		const auto img2 = proc->getImage(low_addr);
		if (img2) {
			this->image_idx = img2->getIdx();
			this->section_idx = -1;
		}
		else {
			this->image_idx = -1;
			this->section_idx = -1;
		}
	}

	this->build();
}

std::ostream& Ctx::operator<<(std::ostream& out, const Bb& bb)
{
	out << bb.idx << "," // idx
		<< bb.addr << "," // addr
		<< bb.size << "," // size
		<< EsteUtils::toHex(bb.bytes) << "," // bytes
		<< bb.image_idx << "," // image_idx
		<< bb.section_idx // section_idx
		<< "\n"; // End entry.
	// TODO: Add routines
	return out;
}

const int32_t Bb::getIdx() const
{
	return this->idx;
}

const ADDRINT Bb::getAddr() const
{
	return this->addr;
}

const uint32_t Bb::getSize() const
{
	return this->size;
}

const xed_decoded_inst_t Bb::disassemble(const ADDRINT addr, uint32_t& out_size)
{
	xed_state_t dstate;
	xed_decoded_inst_t ret = { 0 };

	xed_state_init(&dstate,
#if defined TARGET_IA32E
		XED_MACHINE_MODE_LONG_64, XED_ADDRESS_WIDTH_64b, XED_ADDRESS_WIDTH_64b
#elif (defined TARGET_IA32)
		XED_MACHINE_MODE_LEGACY_32, XED_ADDRESS_WIDTH_32b, XED_ADDRESS_WIDTH_32b
#else
#error "Architecture not supported!"
#endif
	);

	out_size = 0;
	while (true) {

		// Loop needed instead of straight up feeding 15 bytes
		// Because of the possibility of indexing into unreadable
		// memory (Page boundaries)

		xed_decoded_inst_zero_set_mode(&ret, &dstate);
		auto xed_error = xed_decode(&ret, reinterpret_cast<unsigned char*>(addr), (uint)out_size);

		switch (xed_error) {
		case XED_ERROR_BUFFER_TOO_SHORT:
			out_size += 1; break;
		case XED_ERROR_NONE:
			goto end;
		default:
			RAISE_EXCEPTION(
				"`xed_decode` Unhandled error code %s on address `%p`",
				xed_error_enum_t2str(xed_error), (void*)addr);
		}
	}

end:
	return ret;
}

const std::string Bb::inst_to_str(const ADDRINT addr, const xed_decoded_inst_t& inst)
{
	char ret[64] = { 0 };

	// Get string disassembly
	xed_print_info_t pinfo = { 0 };
	pinfo.blen = 64;
	pinfo.buf = ret;
	pinfo.p = &inst;
	pinfo.runtime_address = addr;
	pinfo.syntax = XED_SYNTAX_INTEL;
	xed_format_generic(&pinfo);
	ret[63] = 0;

	std::string ret_str; ret_str += ret;

	return ret_str;
}

ADDRINT Bb::get_rtn_addr_call_jmp_from_operand(
	const CONTEXT* pinctx, const xed_decoded_inst_t& inst,
	const ADDRINT inst_addr, const uint32_t op_idx, const uint32_t memop_idx)
{
	auto xi = *xed_decoded_inst_inst(&inst);
	const xed_operand_t* op = xed_inst_operand(&xi, op_idx);
	xed_operand_enum_t op_name = xed_operand_name(op);

	ADDRINT ret = 0;

	switch (op_name)
	{
	case XED_OPERAND_AGEN:
	case XED_OPERAND_MEM0:
	case XED_OPERAND_MEM1:
	{
		auto xed_base = xed_decoded_inst_get_base_reg(&inst, memop_idx);
		if (xed_base == XED_REG_INSTPTR)
			ret += inst_addr + xed_decoded_inst_get_length(&inst);
		else if (xed_base != XED_REG_INVALID) {
			ADDRINT val = 0;
			PIN_GetContextRegval(pinctx, INS_XedExactMapToPinReg(xed_base), reinterpret_cast<uint8_t*>(&val));
			ret += val;
		}
		auto xed_index = xed_decoded_inst_get_index_reg(&inst, memop_idx);
		if (xed_index != XED_REG_INVALID) {
			ADDRINT val = 0;
			PIN_GetContextRegval(pinctx, INS_XedExactMapToPinReg(xed_index), reinterpret_cast<uint8_t*>(&val));
			xed_uint_t scale = xed_decoded_inst_get_scale(&inst, memop_idx);
			ret += val * scale;
		}
		if (xed_operand_values_has_memory_displacement(&inst))
			ret = static_cast<ADDRINT>(ret + xed_decoded_inst_get_memory_displacement(&inst, memop_idx));
		ret = *reinterpret_cast<ADDRINT*>(ret);
		break;
	}
	case XED_OPERAND_PTR:
	case XED_OPERAND_RELBR:
	{
		ret += inst_addr
			+ xed_decoded_inst_get_length(&inst)
			+ xed_decoded_inst_get_branch_displacement(&inst);
		break;
	}
	case XED_OPERAND_REG0:
	case XED_OPERAND_REG1:
	case XED_OPERAND_REG2:
	case XED_OPERAND_REG3:
	case XED_OPERAND_REG4:
	case XED_OPERAND_REG5:
	case XED_OPERAND_REG6:
	case XED_OPERAND_REG7:
	case XED_OPERAND_REG8:
	case XED_OPERAND_REG9:
	case XED_OPERAND_BASE0:
	case XED_OPERAND_BASE1:
	{
		xed_reg_enum_t xed_reg = xed_decoded_inst_get_reg(&inst, op_name);
		ADDRINT val = 0;
		PIN_GetContextRegval(pinctx, INS_XedExactMapToPinReg(xed_reg), reinterpret_cast<uint8_t*>(&val));
		ret += val;
		break;
	}

	default:
		RAISE_EXCEPTION("`get_addr_of_memop` operand isn't a memop! %s:%d:%d:%s",
			xed_operand_enum_t2str(op_name), op_idx, memop_idx, Bb::inst_to_str(inst_addr, inst).c_str());
	}

	return ret;
}

ADDRINT Bb::get_target_addr_from_call_jmp(
	const CONTEXT* pinctx, const Proc* proc,
	const ADDRINT inst_addr, const xed_decoded_inst_t& inst)
{
	auto opcode = xed_decoded_inst_get_iclass(&inst);
	ADDRINT rtn_addr = 0;

	switch (opcode) {
	case XED_ICLASS_CALL_NEAR: // Exclude CALL_FAR and JMP_FAR
	case XED_ICLASS_JMP:
		rtn_addr = get_rtn_addr_call_jmp_from_operand(pinctx, inst, inst_addr, 0, 0);
	}

	return rtn_addr;
}

int32_t Bb::build()
{
	uint32_t ptr = 0;
	uint32_t sz;
	ADDRINT rtn_addr = 0;
	int32_t rtn_idx = -1;

	//LOGGING("%p:", (void*)this->addr);
	while (ptr < this->size) {
		auto inst = this->disassemble(this->addr + ptr, sz);
		//LOGGING("--  %s", Bb::_inst_to_str(this->addr + ptr, inst).c_str());
		ptr += sz;
	}
	//LOGGING("");

	// Update size (because PIN sometimes gets it wrong)
	if (this->size != ptr) {
		this->size = ptr;
		this->bytes.assign(reinterpret_cast<char*>(this->addr), reinterpret_cast<char*>(this->addr + ptr));
	}

	return rtn_idx;
}


BbExecuted::BbExecuted(uint32_t bb_idx, OS_THREAD_ID os_tid, THREADID pin_tid, int32_t rtn_called_idx)
{
	this->bb_idx = bb_idx;
	this->os_tid = os_tid;
	this->pin_tid = pin_tid;
	this->rtn_called_idx = rtn_called_idx;
}

std::ostream& Ctx::operator<<(std::ostream& out, const BbExecuted& bbe)
{
	out << bbe.bb_idx << ',' // bb idx
		<< bbe.os_tid << ',' // tid
		<< bbe.pin_tid << ',' // pin tid
		<< bbe.rtn_called_idx
		<< "\n"; // End entry

	return out;
}
