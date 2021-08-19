//
// Created by cjb on 7/25/21.
//

#ifndef MHSJ_VALUEGEN_H
#define MHSJ_VALUEGEN_H

#include <string>
#include <set>

namespace IR2asm {

    const int max_reg = 15;

    const int frame_ptr = 11;
    const int sp = 13;
    const int lr = 14;
    const int pc = 15;

    const std::string reg_name[] = {"r0", "r1", "r2", "r3", "r4", "r5",
                "r6", "r7", "r8", "r9", "r10", "r11",
                "r12", "sp", "lr", "pc"};

    class Value {
        public:
        virtual bool is_reg() = 0;
        virtual bool is_const() = 0;
        virtual std::string get_code() = 0;
    };

    class Location{
        public:
        virtual std::string get_code() = 0;
    };


    class Reg : public Value {
    public:
        int id;

        Reg(int i) : id(i) {}

        int get_id() { return id; }

        bool is_reg() final {return true;}
        bool is_const() final {return false;}
        std::string get_code(){ return reg_name[id]; }
    };

enum ShiftOp{
    ASR,
    LSL,
    LSR,
    NOSHIFT
};
    class Regbase: public Location{
        private:
            Reg reg_;
            int offset = 0;
            Reg offset_reg_ = IR2asm::Reg(-1);
            ShiftOp shift_op_ = NOSHIFT;

        public:
            Regbase(Reg reg, int offset): reg_(reg), offset(offset){}
            Regbase(Reg reg, Reg offset_reg): reg_(reg), offset_reg_(offset_reg){}
            Regbase(Reg reg, int offset, ShiftOp shift_op): reg_(reg), offset(offset), shift_op_(shift_op){}
            Regbase(Reg reg, Reg offset_reg, ShiftOp shift_op): reg_(reg), offset_reg_(offset_reg), shift_op_(shift_op){}
            Reg &get_reg(){return reg_;}
            int get_offset(){return offset;}
            void set_offset(int x){offset = x;}
            std::string get_code(){
                if (offset_reg_.get_id() >= 0) {
                    return "[" + reg_.get_code() + ", " + offset_reg_.get_code() + ", lsl #2" + "]";
                } else if(!offset)return "[" + reg_.get_code() + "]";
                else if (shift_op_ == LSL) {
                    return "[" + reg_.get_code() + ", #" + std::to_string(offset*4) + "]";
                } else if (shift_op_ == NOSHIFT) {
                    return "[" + reg_.get_code() + ", #" + std::to_string(offset) + "]";
                } else {
                    return "";
                }
            }
            std::string get_ofst_code(int extra_ofst = 0){
                if(!offset){
                    if(reg_.get_id() == sp && extra_ofst){
                        return "[" + reg_.get_code() + ", #" + std::to_string(extra_ofst) + "]";
                    }
                    return "[" + reg_.get_code() + "]";
                }
                else {
                    if(reg_.get_id() == sp)
                        return "[" + reg_.get_code() + ", #" + std::to_string(offset + extra_ofst) + "]";
                    else{
                        return "[" + reg_.get_code() + ", #" + std::to_string(offset) + "]";
                    }
                }
            }
    };

    class label: public Location{
        private:
            std::string label_;
            int offset = 0;
        
        public:
            explicit label(std::string labl):label_(labl){}
            explicit label(std::string labl, int offst):label_(labl), offset(offst){}
            std::string get_label(){return label_;}
            std::string get_code(){
                if(!offset)return label_;
                return label_ + std::to_string(offset);
            }
            int get_offset(){return offset;}
    };
    class constant: public Value{
        private:
            int value_;

        public:
            explicit constant(int val):value_(val){}
            ~constant(){}
            bool is_const() final {return true;}
            bool is_reg() final {return false;}
            int get_val(){return value_;}
            std::string get_code(){return "#" + std::to_string(value_);}
            std::string get_num(){return std::to_string(value_);}
    };

    class RegLoc: public Location{
    public:
        std::string get_code()override{
            if(is_const){
                return "#" + std::to_string(const_value);
            }else{
                return reg_name[reg_id];
            }
        }
        bool is_constant() const{return is_const;}
        int get_constant() const{
#ifdef DEBUG
            assert(is_const&&"not a const");
#endif
            return const_value;
        }
        RegLoc(int id,bool is_const_val=false){
            if(is_const_val){
                is_const = true;
                const_value = id;
            }else{
                is_const = false;
                reg_id = id;
            }
        }
        int get_reg_id() const{
#ifdef DEBUG
            assert(!is_const&&"not a reg");
#endif
            return reg_id;
        }
    private:
        int reg_id;
        bool is_const = false;
        int const_value;
    };

enum Operand2Type{
    RegTy,
    ConstTy,
    RegShiftConstTy,
    RegShiftRegTy
};

    class Operand2: public Value{
        private:
            Reg reg_1_;
            Reg reg_2_;
            ShiftOp shift_op_ = NOSHIFT;
            Operand2Type ty;
            int value_ = 0;

        public:
            explicit Operand2(Reg reg_1, ShiftOp shift_op, Reg reg_2):ty(RegShiftRegTy), reg_1_(reg_1), shift_op_(shift_op), reg_2_(reg_2){}
            explicit Operand2(Reg reg, ShiftOp shift_op, int val):ty(RegShiftConstTy), reg_1_(reg), shift_op_(shift_op), value_(val), reg_2_(IR2asm::Reg(-1)){}
            explicit Operand2(Reg reg):ty(RegTy), reg_1_(reg), reg_2_(IR2asm::Reg(-1)){}
            explicit Operand2(int val):ty(ConstTy), value_(val), reg_1_(IR2asm::Reg(-1)), reg_2_(IR2asm::Reg(-1)){}
            ~Operand2(){}
            bool is_const() final {return false;}
            bool is_reg() final {return false;}
            std::string get_operand2(ShiftOp shift_op) {if (shift_op == ShiftOp::ASR) return "asr";
                                                        else if (shift_op == ShiftOp::LSL) return "lsl";
                                                        else if (shift_op == ShiftOp::LSR) return "lsr";
                                                        else return "";}
            std::string get_code(){if (shift_op_ == NOSHIFT) {if (ty == RegTy) return reg_1_.get_code(); else return "#" + std::to_string(value_);}
                                    else {if (ty == RegShiftRegTy) return reg_1_.get_code() + ", " + get_operand2(shift_op_) + " " + reg_2_.get_code();
                                            else return reg_1_.get_code() + ", " + get_operand2(shift_op_) + " " + "#" + std::to_string(value_);}}
    };

}

#endif //MHSJ_VALUEGEN_H