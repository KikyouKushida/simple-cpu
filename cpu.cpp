#include <bits/stdc++.h>

using uint = unsigned int;
class Register{
private:
  uint r[32], pc;
public:
  uint get_pc(){
    return pc;
  }
  void modify_pc(uint new_pc){
    pc = new_pc;
    return ;
  }
  void modify_register(uint id, uint val){
    if(id == 0) return ;
    r[id] = val;
    return ;
  }
  Register(){
    for(uint i = 0; i < 32; ++i) r[i] = 0;
    pc = 0;
  }
  uint get_register(uint id){
    return r[id];
  }
};

uint to_hex(char c){
  if(c >= '0' && c <= '9') return c - '0';
  else return c - 'A' + 10;
}

uint sext(uint val, uint w){
  if(val & (1 << w - 1)){
    for(int i = w; i < 32; ++i) val ^= (1 << i);
    return val;
  }
  return val;
}

class Memory{
private:
  unsigned char info[1048576]; // i do not know how big it should be
public:
  unsigned char get_info(int id){
    assert(id >= 0 && id <= 1048575);
    return info[id];
  }
  Memory(){
    unsigned int current_location = 0, value = 0, count = 0;
    bool flag = false;
    char cc;
    unsigned char c;
    while(cc = std::cin.get()){
      if(cc == -1) break;
      c = cc;
      std::cout << "about to print\n";
      std::cout << "c = " << int(c) << "\n";
      std::cout << "finish printing\n";
      count += 1;
      // if(count == 10000){
      //    exit(0);
      // }
      if(c == 255){
        std::cout << "end\n";
        break;
      }
      if(c == '@'){
        std::cin >> std::hex >> current_location;

        std::cout << "current_location = " << current_location << "\n";
        //
      }
      else if(!(c <= '9' && '0' <= c) && !(c <= 'Z' && 'A' <= c)){
        std::cout << "continue\n";
        continue;
      } 
      else {
        value = (value << 4) ^ to_hex(c);
        if(flag == true){
          if(current_location < 0 || current_location > 1048575){
            std::cout << "current_location error\n";
          }
          info[current_location] = value;
          std::cout << "store value " << value << " in location " << current_location << "\n";
          current_location += 1;
          value = 0;
        } flag = !flag;
      }
    }
    std::cout << "ok\n";
  }
  void modify_memory(unsigned int location, unsigned char val){
    info[location] = val;
    return ;
  }
};

std::map <std::string, unsigned int> type_to_uint;

void type_error(unsigned int location){
  std::cout << "Instruction Error in location " << location << "\n";
  exit(0);
}

const uint emp = -1;

unsigned int instruction_count = 0;

struct instruction{
  unsigned int opcode, imm, rd, funct3, rs1, rs2, funct7;
  std::string type;
  instruction(){
    rs1 = rs2 = emp;
  }
  void print(){
    std::cout << "opcode = " << opcode << "\n";
    std::cout << "imm = " << imm << "\n";
    std::cout << "rd = " << rd << "\n";
    std::cout << "funct3 = " << funct3 << "\n";
    std::cout << "rs1 = " << rs1 << "\n";
    std::cout << "rs2 = " << rs2 << "\n";
    std::cout << "funct7 = " << funct7 << "\n";
    std::cout << "type = " << type << "\n";
    ++instruction_count;
    if(instruction_count == 1000) exit(0);
  }
};


class decoder{
private:
  static unsigned int get_value(unsigned int value, unsigned char l, unsigned char r){
    assert(l >= 0 && l <= r && r <= 31);
    unsigned char tmp = ((1 << r + 1) - 1) & value;
    return tmp >> l;
  }
  Memory *m;
public:
  decoder(Memory *m_){
    m = m_;
  }
  instruction decode(unsigned int location){
    unsigned char tmp[4];
    instruction ans;
    for(int i = 0; i <= 3; ++i) tmp[i] = m->get_info(location + i);
    ans.opcode = get_value(tmp[0], 0, 6);
    if(ans.opcode == 0x33){
      ans.rd = get_value(tmp[0], 7, 7) ^ (get_value(tmp[1], 0, 3) << 1),
      ans.funct3 = get_value(tmp[1], 4, 6),
      ans.rs1 = get_value(tmp[1], 7, 7) ^ (get_value(tmp[2], 0, 3) << 1),
      ans.rs2 = get_value(tmp[2], 4, 7) ^ (get_value(tmp[3], 0, 0) << 4),
      ans.funct7 = get_value(tmp[3], 1, 7);
      if(ans.funct3 == 0x0){
        if(ans.funct7 == 0x0) ans.type = "add";
        else if(ans.funct7 == 0x20) ans.type = "sub";
        else type_error(location);
      }
      else if(ans.funct3 == 0x7) ans.type = "and";
      else if(ans.funct3 == 0x6) ans.type = "or";
      else if(ans.funct3 == 0x4) ans.type = "xor";
      else if(ans.funct3 == 0x1) ans.type = "sll";
      else if(ans.funct3 == 0x5){
        if(ans.funct7 == 0x0) ans.type = "srl";
        else if(ans.funct7 == 0x20) ans.type = "sra";
        else type_error(location);
      }
      else if(ans.funct3 == 0x2) ans.type = "slt";
      else if(ans.funct3 == 0x3) ans.type = "sltu";
      else {
        type_error(location);
        assert(0);
      }
    }//R
    else if(ans.opcode == 0x13){
      ans.rd = get_value(tmp[0], 7, 7) ^ (get_value(tmp[1], 0, 3) << 1),
      ans.funct3 = get_value(tmp[1], 4, 6),
      ans.rs1 = get_value(tmp[1], 7, 7) ^ (get_value(tmp[2], 0, 3) << 1);
      if(ans.funct3 == 0x1 || ans.funct3 == 0x5){
        ans.imm = get_value(tmp[2], 4, 7) ^ (get_value(tmp[3], 0, 0) << 4),
        ans.funct7 = get_value(tmp[3], 1, 7);
      }
      else {
        ans.imm = get_value(tmp[2], 4, 7) ^ (get_value(tmp[3], 0, 7) << 4);
      }
      if(ans.funct3 == 0x0) ans.type = "addi";
      else if(ans.funct3 == 0x7) ans.type = "andi";
      else if(ans.funct3 == 0x6) ans.type = "ori";
      else if(ans.funct3 == 0x4) ans.type = "xori";
      else if(ans.funct3 == 0x1 && ans.funct7 == 0x0) ans.type = "slli";
      else if(ans.funct3 == 0x5 && ans.funct7 == 0x0) ans.type = "srli";
      else if(ans.funct3 == 0x5 && ans.funct7 == 0x20) ans.type = "srai";
      else if(ans.funct3 == 0x2) ans.type = "slti";
      else if(ans.funct3 == 0x3) ans.type = "sltiu";
      else {
        type_error(location);
        assert(0);
      }
    }//I + I*
    else if(ans.opcode == 0x3){
      ans.rd = get_value(tmp[0], 7, 7) ^ (get_value(tmp[1], 0, 3) << 1),
      ans.funct3 = get_value(tmp[1], 4, 6),
      ans.rs1 = get_value(tmp[1], 7, 7) ^ (get_value(tmp[2], 0, 3) << 1);
      ans.imm = get_value(tmp[2], 4, 7) ^ (get_value(tmp[3], 0, 7) << 4);
      if(ans.funct3 == 0x0) ans.type = "lb";
      else if(ans.funct3 == 0x4) ans.type = "lbu";
      else if(ans.funct3 == 0x1) ans.type = "lh";
      else if(ans.funct3 == 0x5) ans.type = "lhu";
      else if(ans.funct3 == 0x2) ans.type = "lw";
      else {
        type_error(location);
        assert(0);
      }
    }//I
    else if(ans.opcode == 0x23){  
      ans.funct3 = get_value(tmp[1], 4, 6),
      ans.rs1 = get_value(tmp[1], 7, 7) ^ (get_value(tmp[2], 0, 3) << 1),
      ans.rs2 = get_value(tmp[2], 4, 7) ^ (get_value(tmp[3], 0, 0) << 4);
      ans.imm = get_value(tmp[0], 7, 7) ^ (get_value(tmp[1], 0, 3) << 1) ^ (get_value(tmp[3], 1, 7) << 5);
      if(ans.funct3 == 0x0) ans.type = "sb";
      else if(ans.funct3 == 0x1) ans.type = "sh";
      else if(ans.funct3 == 0x2) ans.type = "sw";
      else {
        type_error(location);
        assert(0);
      }
    }//S
    else if(ans.opcode == 0x63){
      ans.funct3 = get_value(tmp[1], 4, 6),
      ans.rs1 = get_value(tmp[1], 7, 7) ^ (get_value(tmp[2], 0, 3) << 1),
      ans.rs2 = get_value(tmp[2], 4, 7) ^ (get_value(tmp[3], 0, 0) << 4);
      ans.imm = (get_value(tmp[1], 0, 3) << 1) ^ (get_value(tmp[3], 1, 6) << 5) ^ (get_value(tmp[0], 7, 7) << 11) ^ (get_value(tmp[3], 7, 7) << 12);              
      if(ans.funct3 == 0x0) ans.type = "beq";
      else if(ans.funct3 == 0x5) ans.type = "bge";
      else if(ans.funct3 == 0x7) ans.type = "bgeu";
      else if(ans.funct3 == 0x4) ans.type = "blt";
      else if(ans.funct3 == 0x6) ans.type = "bltu";
      else if(ans.funct3 == 0x1) ans.type = "bne";
      else {
        type_error(location);
        assert(0);
      }
    }//B
    else if(ans.opcode == 0x6f){
      ans.rd = get_value(tmp[0], 7, 7) ^ (get_value(tmp[1], 0, 3) << 1);
      ans.imm = (get_value(tmp[2], 5, 7) << 1) ^ (get_value(tmp[3], 0, 6) << 4) ^ (get_value(tmp[2], 4, 4) << 11) ^ (get_value(tmp[1], 4, 7) << 12) ^ (get_value(tmp[2], 0, 3) << 16) ^ (get_value(tmp[3], 7, 7) << 20);
      ans.type = "jal";
    }//J
    else if(ans.opcode == 0x67){
      ans.rd = get_value(tmp[0], 7, 7) ^ (get_value(tmp[1], 0, 3) << 1),
      ans.funct3 = get_value(tmp[1], 4, 6),
      ans.rs1 = get_value(tmp[1], 7, 7) ^ (get_value(tmp[2], 0, 3) << 1);
      ans.imm = get_value(tmp[2], 4, 7) ^ (get_value(tmp[3], 0, 7) << 4);
      ans.type = "jalr";
    }//I
    else if(ans.opcode == 0x17){
      ans.rd = get_value(tmp[0], 7, 7) ^ (get_value(tmp[1], 0, 3) << 1);
      ans.imm = (get_value(tmp[1], 4, 7) << 12) ^ (get_value(tmp[2], 0, 7) << 16) ^ (get_value(tmp[3], 0, 7) << 24);
      ans.type = "auipc";
    }//U
    else if(ans.opcode == 0x37){
      ans.rd = get_value(tmp[0], 7, 7) ^ (get_value(tmp[1], 0, 3) << 1);
      ans.imm = (get_value(tmp[1], 4, 7) << 12) ^ (get_value(tmp[2], 0, 7) << 16) ^ (get_value(tmp[3], 0, 7) << 24);
      ans.type = "lui";
    }
    else if(ans.opcode == 0x73){
      ans.rd = get_value(tmp[0], 7, 7) ^ (get_value(tmp[1], 0, 3) << 1);
      ans.imm = (get_value(tmp[1], 4, 7) << 12) ^ (get_value(tmp[2], 0, 7) << 16) ^ (get_value(tmp[3], 0, 7) << 24);
      if(ans.imm == 0x0) ans.type = "ebreak";
      else if(ans.imm == 0x1) ans.type = "ecall";
      else {
        type_error(location);
        assert(0);
      }
    }
    else {
      std::cout << "Instruction Error in location " << location << "\n";
      exit(0);
    }
    return ans;
  }
  
};

uint run(instruction &ins, Register *rsts, Memory *m, uint pc){
  uint new_pc;
  if(ins.type == "add"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    rsts->modify_register(ins.rd, v1 + v2);
    new_pc = pc + 4;
  }
  else if(ins.type == "sub"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    rsts->modify_register(ins.rd, v1 - v2);
    new_pc = pc + 4;
  }
  else if(ins.type == "and"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    rsts->modify_register(ins.rd, v1 & v2);
    new_pc = pc + 4;
  }
  else if(ins.type == "or"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    rsts->modify_register(ins.rd, v1 | v2);
    new_pc = pc + 4;
  }
  else if(ins.type == "xor"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    rsts->modify_register(ins.rd, v1 ^ v2);
    new_pc = pc + 4;
  }
  else if(ins.type == "sll"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    rsts->modify_register(ins.rd, v1 << v2);
    new_pc = pc + 4;
  }
  else if(ins.type == "srl"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    rsts->modify_register(ins.rd, v1 >> v2);
    new_pc = pc + 4;
  }
  else if(ins.type == "sra"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    uint v3 = v1 >> v2;
    if((v1 >> 31) & 1){
      for(int i = 32 - v2; i <= 31; ++i) 
        v3 ^= (1 << i);
    }
    rsts->modify_register(ins.rd, v3);
    new_pc = pc + 4;
  }
  else if(ins.type == "slt"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    rsts->modify_register(ins.rd, int(v1) < int(v2) ? 1 : 0);
    new_pc = pc + 4;
  }
  else if(ins.type == "sltu"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    rsts->modify_register(ins.rd, v1 < v2 ? 1 : 0);
    new_pc = pc + 4;
  }
  else if(ins.type == "addi"){
    uint v1 = rsts->get_register(ins.rs1), imm = ins.imm;
    rsts->modify_register(ins.rd, v1 + imm);
    new_pc = pc + 4;
  }
  else if(ins.type == "andi"){
    uint v1 = rsts->get_register(ins.rs1), imm = ins.imm;
    rsts->modify_register(ins.rd, v1 & imm);
    new_pc = pc + 4;
  }
  else if(ins.type == "ori"){
    uint v1 = rsts->get_register(ins.rs1), imm = ins.imm;
    rsts->modify_register(ins.rd, v1 | imm);
    new_pc = pc + 4;
  }
  else if(ins.type == "xori"){
    uint v1 = rsts->get_register(ins.rs1), imm = ins.imm;
    rsts->modify_register(ins.rd, v1 ^ imm);
    new_pc = pc + 4;
  }
  else if(ins.type == "slli"){
    uint v1 = rsts->get_register(ins.rs1), imm = ins.imm;
    rsts->modify_register(ins.rd, v1 << imm);
    new_pc = pc + 4;
  }
  else if(ins.type == "srli"){
    uint v1 = rsts->get_register(ins.rs1), imm = ins.imm;
    rsts->modify_register(ins.rd, v1 >> imm);
    new_pc = pc + 4;
  }
  else if(ins.type == "srai"){
    uint v1 = rsts->get_register(ins.rs1), imm = ins.imm;
    uint v3 = v1 >> imm;
    if((v1 >> 31) & 1){
      for(int i = 32 - imm; i <= 31; ++i) 
        v3 ^= (1 << i);
    }
    rsts->modify_register(ins.rd, v3);
    new_pc = pc + 4;
  }
  else if(ins.type == "slti"){
    uint v1 = rsts->get_register(ins.rs1), imm = sext(ins.imm, 12);
    rsts->modify_register(ins.rd, (int(v1) < int(imm)) ? 1 : 0);
    new_pc = pc + 4;
  } // ? 
  else if(ins.type == "sltiu"){
    uint v1 = rsts->get_register(ins.rs1), imm = sext(ins.imm, 12);
    rsts->modify_register(ins.rd, (v1 < imm) ? 1 : 0);
    new_pc = pc + 4;
  }
  else if(ins.type == "lb"){
    uint location = rsts->get_register(ins.rs1) + sext(ins.imm, 12);
    rsts->modify_register(ins.rd, m->get_info(location));
    new_pc = pc + 4;
  }
  else if(ins.type == "lbu"){
    uint location = rsts->get_register(ins.rs1) + ins.imm;
    rsts->modify_register(ins.rd, m->get_info(location));
    new_pc = pc + 4;
  }
  else if(ins.type == "lh"){
    uint location = rsts->get_register(ins.rs1) + sext(ins.imm, 12);
    rsts->modify_register(ins.rd, m->get_info(location) + m->get_info(location + 1) * 256);
    new_pc = pc + 4;
  }
  else if(ins.type == "lhu"){
    uint location = rsts->get_register(ins.rs1) + ins.imm;
    rsts->modify_register(ins.rd, m->get_info(location) + m->get_info(location + 1) * 256);
    new_pc = pc + 4;
  }
  else if(ins.type == "lw"){
    uint location = rsts->get_register(ins.rs1) + sext(ins.imm, 12);
    rsts->modify_register(ins.rd, m->get_info(location) + m->get_info(location + 1) * (1u << 8) + m->get_info(location + 2) * (1u << 16) + m->get_info(location + 3) * (1u << 24));
    new_pc = pc + 4;
  }
  else if(ins.type == "sb"){
    uint location = rsts->get_register(ins.rs1) + sext(ins.imm, 12);
    uint v2 = rsts->get_register(ins.rs2) % 256;
    m->modify_memory(location, v2);
    new_pc = pc + 4;
  }
  else if(ins.type == "sh"){
    uint location = rsts->get_register(ins.rs1) + sext(ins.imm, 12);
    uint v2 = rsts->get_register(ins.rs2) % 256, v3 = (rsts->get_register(ins.rs2) >> 8) % 256;
    m->modify_memory(location, v2);
    m->modify_memory(location + 1, v3);
    new_pc = pc + 4;
  }
  else if(ins.type == "sw"){
    uint location = rsts->get_register(ins.rs1) + sext(ins.imm, 12);
    uint v2 = rsts->get_register(ins.rs2) % 256, v3 = (rsts->get_register(ins.rs2) >> 8) % 256,
         v4 = (rsts->get_register(ins.rs2) >> 16) % 256, v5 = (rsts->get_register(ins.rs2) >> 24) % 256;
    m->modify_memory(location, v2);
    m->modify_memory(location + 1, v3);
    m->modify_memory(location + 2, v4);
    m->modify_memory(location + 3, v5);
    new_pc = pc + 4;
  }
  else if(ins.type == "beq"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    if(v1 == v2) new_pc = pc + sext(ins.imm, 13);
    else new_pc = pc + 4;
  }
  else if(ins.type == "bge"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    if(int(v1) >= int(v2)) new_pc = pc + sext(ins.imm, 13);
    else new_pc = pc + 4;
  }
  else if(ins.type == "bgeu"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    if(v1 >= v2) new_pc = pc + sext(ins.imm, 13);
    else new_pc = pc + 4;
  }
  else if(ins.type == "blt"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    if(int(v1) < int(v2)) new_pc = pc + sext(ins.imm, 13);
    else new_pc = pc + 4;
  }
  else if(ins.type == "bltu"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    if(v1 < v2) new_pc = pc + sext(ins.imm, 13);
    else new_pc = pc + 4;
  }
  else if(ins.type == "bne"){
    uint v1 = rsts->get_register(ins.rs1), v2 = rsts->get_register(ins.rs2);
    if(v1 != v2) new_pc = pc + sext(ins.imm, 13);
    else new_pc = pc + 4;
  }
  else if(ins.type == "jal"){
    rsts->modify_register(ins.rd, pc + 4);
    new_pc = pc + sext(ins.imm, 21);
  }
  else if(ins.type == "jalr"){
    rsts->modify_register(ins.rd, pc + 4);
    new_pc = rsts->get_register(ins.rs1) + sext(ins.imm, 12);
  }
  else if(ins.type == "auipc"){
    uint imm = ins.imm;
    rsts->modify_register(ins.rd, pc + imm);
    new_pc = pc + 4;
  }
  else if(ins.type == "lui"){
    uint imm = ins.imm << 12;
    rsts->modify_register(ins.rd, imm);
    new_pc = pc + 4;
  }
  else if(ins.type == "ebreak"){
    std::cout << "debug" << "\n";
    exit(0);
  }
  else if(ins.type == "ecall"){
    std::cout << "os" << "\n";
    exit(0);
  }
  else {
    std::cout << "instruction error, current pc = " << pc << "\n";
    exit(0);
  }
  return new_pc;
}

int main(){
  //freopen("basicopt1.data", "r", stdin);
  //freopen("basicopt1.out", "w", stdout);
  std::cout << "begin\n";
  Memory m;
  std::cout << "okk\n";
  decoder d(&m);
  std::cout << "begin1\n";
  Register rsts;
  std::cout << "begin2\n";
  while(true){
    std::cout << "pc = " << rsts.get_pc() << "\n";
    instruction ins = d.decode(rsts.get_pc());
    ins.print();
    uint new_pc = run(ins, &rsts, &m, rsts.get_pc());
    if(new_pc == -1) break; 
    rsts.modify_pc(new_pc);
  }
  std::cout << std::dec << rsts.get_register(10) << "\n";
  return 0;
}
