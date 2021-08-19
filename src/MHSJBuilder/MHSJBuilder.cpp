#include "MHSJBuilder.hpp"

#define CONST_INT(num) ConstantInt::get(num, module.get())

// You can define global variables here
// to store state

// store temporary value
Value *tmp_val = nullptr;
// whether require lvalue
bool require_lvalue = false;
// function that is being built
Function *cur_fun = nullptr;
// detect scope pre-enter (for elegance only)
bool pre_enter_scope = false;

// types
Type *VOID_T;
Type *INT1_T;
Type *INT32_T;
Type *INT32PTR_T;

/* Global Variable */

// used for backpatching
struct true_false_BB {
  BasicBlock *trueBB = nullptr;
  BasicBlock *falseBB = nullptr;
};

std::list<true_false_BB> IF_While_And_Cond_Stack; // used for Cond
std::list<true_false_BB> IF_While_Or_Cond_Stack; // used for Cond
std::list<true_false_BB> While_Stack;    // used for break and continue
// used for backpatching
std::vector<BasicBlock*> cur_basic_block_list;
std::vector<SyntaxTree::FuncParam> func_fparams;
std::vector<int> array_bounds;
std::vector<int> array_sizes;
int cur_pos;
int cur_depth;
std::map<int, Value *> initval;
std::vector<Constant *> init_val;

//ret BB
BasicBlock *ret_BB;
Value *ret_addr;
//

/* Global Variable */

void MHSJBuilder::visit(SyntaxTree::Assembly &node) {
  VOID_T = Type::get_void_type(module.get());
  INT1_T = Type::get_int1_type(module.get());
  INT32_T = Type::get_int32_type(module.get());
  INT32PTR_T = Type::get_int32_ptr_type(module.get());
  for (const auto& def : node.global_defs) {
    def->accept(*this);
  }
}

void MHSJBuilder::visit(SyntaxTree::InitVal &node) {
  if (node.isExp) {
    node.expr->accept(*this);
    initval[cur_pos] = tmp_val;
    init_val.push_back(dynamic_cast<Constant *>(tmp_val));
    cur_pos++;
  }
  else {
    if (cur_depth!=0){
        while (cur_pos % array_sizes[cur_depth] != 0) {
        init_val.push_back(CONST_INT(0));
        cur_pos++;
      }
    }
    int cur_start_pos = cur_pos;
    for (const auto& elem : node.elementList) {
      cur_depth++;
      elem->accept(*this);
      cur_depth--;
    }
    if (cur_depth!=0){
      while (cur_pos < cur_start_pos + array_sizes[cur_depth]) {
        init_val.push_back(CONST_INT(0));
        cur_pos++;
      }
    }
    if (cur_depth==0){
      while (cur_pos < array_sizes[0]){
        init_val.push_back(CONST_INT(0));
        cur_pos++;
      }
    }
  }
}

void MHSJBuilder::visit(SyntaxTree::FuncDef &node) {
  FunctionType *fun_type;
  Type *ret_type;
  if (node.ret_type == SyntaxTree::Type::INT)
    ret_type = INT32_T;
  else
    ret_type = VOID_T;

  std::vector<Type *> param_types;
  std::vector<SyntaxTree::FuncParam>().swap(func_fparams);
  node.param_list->accept(*this);
  for (const auto& param : func_fparams) {
    if (param.param_type == SyntaxTree::Type::INT) {
      if (param.array_index.empty()) {
        param_types.push_back(INT32_T);
      }
      else {
        param_types.push_back(INT32PTR_T);
      }
    }
  }
  fun_type = FunctionType::get(ret_type, param_types);
  auto fun = Function::create(fun_type, node.name, module.get());
  scope.push_func(node.name, fun);
  cur_fun = fun;
  auto funBB = BasicBlock::create(module.get(), "entry", fun);
  builder->set_insert_point(funBB);
  cur_basic_block_list.push_back(funBB);
  scope.enter();
  pre_enter_scope = true;
  std::vector<Value *> args;
  for (auto arg = fun->arg_begin(); arg != fun->arg_end(); arg++) {
    args.push_back(*arg);
  }
  int param_num = func_fparams.size();
  for (int i = 0; i < param_num; i++) {
    if (func_fparams[i].array_index.empty()) {
      Value *alloc;
      alloc = builder->create_alloca(INT32_T);
      builder->create_store(args[i], alloc);
      scope.push(func_fparams[i].name, alloc);
    }
    else {
      Value *alloc_array;
      alloc_array = builder->create_alloca(INT32PTR_T);
      builder->create_store(args[i], alloc_array);
      scope.push(func_fparams[i].name, alloc_array);
      array_bounds.clear();
      array_sizes.clear();
      for (auto bound_expr : func_fparams[i].array_index) {
        int bound;
        if (bound_expr==nullptr){
          bound = 1;
        }
        else{
          bound_expr->accept(*this);
          auto bound_const = dynamic_cast<ConstantInt *>(tmp_val);
          bound = bound_const->get_value();
        }
        array_bounds.push_back(bound);
      }
      int total_size = 1;
      for (auto iter = array_bounds.rbegin(); iter != array_bounds.rend();iter++) {
        array_sizes.insert(array_sizes.begin(), total_size);
        total_size *= (*iter);
      }
      array_sizes.insert(array_sizes.begin(), total_size);
      scope.push_size(func_fparams[i].name, array_sizes);
    }
  }
  //ret BB
  if (ret_type == INT32_T){
    ret_addr = builder->create_alloca(INT32_T);
  }
  ret_BB = BasicBlock::create(module.get(), "ret", fun);

  node.body->accept(*this);

  if (builder->get_insert_block()->get_terminator() == nullptr) {
    if (cur_fun->get_return_type()->is_void_type()){
      builder->create_br(ret_BB);
    }
    else {
      builder->create_store(CONST_INT(0), ret_addr);
      builder->create_br(ret_BB);
    }
  }
  scope.exit();
  cur_basic_block_list.pop_back();

  //ret BB
  builder->set_insert_point(ret_BB);
  if (fun->get_return_type()== VOID_T){
    builder->create_void_ret();
  }
  else{
    auto ret_val = builder->create_load(ret_addr);
    builder->create_ret(ret_val);
  }
}

void MHSJBuilder::visit(SyntaxTree::FuncFParamList &node) {
  for (const auto& Param : node.params) {
    Param->accept(*this);
  }
}

void MHSJBuilder::visit(SyntaxTree::FuncParam &node) {
  func_fparams.push_back(node);
}

void MHSJBuilder::visit(SyntaxTree::VarDef &node) {
  Type *var_type;
  BasicBlock *cur_fun_entry_block;
  BasicBlock *cur_fun_cur_block;
  if (scope.in_global() == false) {
    cur_fun_entry_block = cur_fun->get_entry_block();   // entry block
    cur_fun_cur_block = cur_basic_block_list.back();                // current block
  }
  if (node.is_constant) {
    // constant
    Value *var;
    if (node.array_length.empty()){
      node.initializers->accept(*this);
      auto initializer = dynamic_cast<ConstantInt *>(tmp_val)->get_value();
      var = ConstantInt::get(initializer, module.get());
      scope.push(node.name, var);
    }
    else {
      //array
      array_bounds.clear();
      array_sizes.clear();
      for (const auto& bound_expr : node.array_length) {
        bound_expr->accept(*this);
        auto bound_const = dynamic_cast<ConstantInt *>(tmp_val);
        auto bound = bound_const->get_value();
        array_bounds.push_back(bound);
      }
      int total_size = 1;
      for (auto iter = array_bounds.rbegin(); iter != array_bounds.rend();
            iter++) {
        array_sizes.insert(array_sizes.begin(), total_size);
        total_size *= (*iter);
      }
      array_sizes.insert(array_sizes.begin(), total_size);

      var_type = INT32_T;
      auto *array_type = ArrayType::get(var_type, total_size);

      initval.clear();
      init_val.clear();
      cur_depth = 0;
      cur_pos = 0;
      node.initializers->accept(*this);
      auto initializer = ConstantArray::get(array_type, init_val);

      if (scope.in_global()){
        var = GlobalVariable::create(node.name, module.get(), array_type, true, initializer);
        scope.push(node.name, var);
        scope.push_size(node.name, array_sizes);
        scope.push_const(node.name, initializer);
      }
      else {
        auto tmp_terminator = cur_fun_entry_block->get_terminator();
        if (tmp_terminator != nullptr) {
          cur_fun_entry_block->get_instructions().pop_back();
        }
        var = builder->create_alloca(array_type);
        cur_fun_cur_block->get_instructions().pop_back();
        cur_fun_entry_block->add_instruction(dynamic_cast<Instruction*>(var));
        dynamic_cast<Instruction*>(var)->set_parent(cur_fun_entry_block);
        if (tmp_terminator != nullptr) {
          cur_fun_entry_block->add_instruction(tmp_terminator);
        }
        for (int i = 0; i < array_sizes[0]; i++) {
          if (initval[i]) {
            builder->create_store(initval[i], builder->create_gep(var, {CONST_INT(0), CONST_INT(i)}));
          }
          else {
            builder->create_store(CONST_INT(0), builder->create_gep(var, {CONST_INT(0), CONST_INT(i)}));
          }
        }
        scope.push(node.name, var);
        scope.push_size(node.name, array_sizes);
        scope.push_const(node.name, initializer);
      }
    }
  }
  else {
    var_type = INT32_T;
    if (node.array_length.empty()) {
      Value *var;
      if (scope.in_global()) {
        if (node.is_inited) {
          node.initializers->accept(*this);
          auto initializer = dynamic_cast<ConstantInt *>(tmp_val);
          var = GlobalVariable::create(node.name, module.get(), var_type, false, initializer);
          scope.push(node.name, var);
        }
        else{
          auto initializer = ConstantZero::get(var_type, module.get());
          var = GlobalVariable::create(node.name, module.get(), var_type, false, initializer);
          scope.push(node.name, var);
        }
      }
      else {
        auto tmp_terminator = cur_fun_entry_block->get_terminator();
        if (tmp_terminator != nullptr) {
          cur_fun_entry_block->get_instructions().pop_back();
        }
        var = builder->create_alloca(var_type);
        cur_fun_cur_block->get_instructions().pop_back();
        cur_fun_entry_block->add_instruction(dynamic_cast<Instruction*>(var));
        if (tmp_terminator != nullptr) {
          cur_fun_entry_block->add_instruction(tmp_terminator);
        }
        if (node.is_inited) {
          node.initializers->accept(*this);
          builder->create_store(tmp_val, var);
        }
        scope.push(node.name, var);
      }
    }
    else {
      // array
      array_bounds.clear();
      array_sizes.clear();
      for (const auto& bound_expr : node.array_length) {
        bound_expr->accept(*this);
        auto bound_const = dynamic_cast<ConstantInt *>(tmp_val);
        auto bound = bound_const->get_value();
        array_bounds.push_back(bound);
      }
      int total_size = 1;
      for (auto iter = array_bounds.rbegin(); iter != array_bounds.rend();
            iter++) {
        array_sizes.insert(array_sizes.begin(), total_size);
        total_size *= (*iter);
      }
      array_sizes.insert(array_sizes.begin(), total_size);
      auto *array_type = ArrayType::get(var_type, total_size);

      Value *var;
      if (scope.in_global()) {
        if (node.is_inited ){
          cur_pos = 0;
          cur_depth = 0;
          initval.clear();
          init_val.clear();
          node.initializers->accept(*this);
          auto initializer = ConstantArray::get(array_type, init_val);
          var = GlobalVariable::create(node.name, module.get(), array_type, false, initializer);
          scope.push(node.name, var);
          scope.push_size(node.name, array_sizes);
        }
        else {
          auto initializer = ConstantZero::get(array_type, module.get());
          var = GlobalVariable::create(node.name, module.get(), array_type, false, initializer);
          scope.push(node.name, var);
          scope.push_size(node.name, array_sizes);
        }
      }
      else {
        auto tmp_terminator = cur_fun_entry_block->get_terminator();
        if (tmp_terminator != nullptr) {
          cur_fun_entry_block->get_instructions().pop_back();
        }
        var = builder->create_alloca(array_type);
        cur_fun_cur_block->get_instructions().pop_back();
        cur_fun_entry_block->add_instruction(dynamic_cast<Instruction*>(var));
        if (tmp_terminator != nullptr) {
          cur_fun_entry_block->add_instruction(tmp_terminator);
        }
        if (node.is_inited) {
          cur_pos = 0;
          cur_depth = 0;
          initval.clear();
          init_val.clear();
          node.initializers->accept(*this);
          for (int i = 0; i < array_sizes[0]; i++) {
            if (initval[i]) {
              builder->create_store(initval[i], builder->create_gep(var, {CONST_INT(0), CONST_INT(i)}));
            }
            else {
              builder->create_store(CONST_INT(0), builder->create_gep(var, {CONST_INT(0), CONST_INT(i)}));
            }
          }
        }
        scope.push(node.name, var);
        scope.push_size(node.name, array_sizes);
      }//if of global check
    }
  }
}

void MHSJBuilder::visit(SyntaxTree::AssignStmt &node) {
  node.value->accept(*this);
  auto result = tmp_val;
  require_lvalue = true;
  node.target->accept(*this);
  auto addr = tmp_val;
  builder->create_store(result, addr);
  tmp_val = result;
}

void MHSJBuilder::visit(SyntaxTree::LVal &node) {
  // FIXME:may have bug
  auto var = scope.find(node.name);
  bool should_return_lvalue = require_lvalue;
  require_lvalue = false;
  if (node.array_index.empty()) {
    if (should_return_lvalue) {
      if (var->get_type()->get_pointer_element_type()->is_array_type()) {
        tmp_val = builder->create_gep(var, {CONST_INT(0), CONST_INT(0)});
      }
      else if (var->get_type()->get_pointer_element_type()->is_pointer_type()) {
        tmp_val = builder->create_load(var);
      }
      else {
        tmp_val = var;
      }
      require_lvalue = false;
    }
    else {
      auto val_const = dynamic_cast<ConstantInt *>(var);
      if (val_const != nullptr){
        tmp_val = val_const;
      }
      else{
        tmp_val = builder->create_load(var);
      }
    }
  }
  else {
    auto var_sizes = scope.find_size(node.name);
    std::vector<Value *>all_index;
    Value *var_index = nullptr;
    int index_const = 0;
    bool const_check = true;

    auto const_array = scope.find_const(node.name);
    if (const_array == nullptr){
      const_check = false;
    }

    for (int i = 0; i < node.array_index.size(); i++){
      node.array_index[i]->accept(*this);
      all_index.push_back(tmp_val);
      if (const_check == true){
        auto tmp_const = dynamic_cast<ConstantInt *>(tmp_val);
        if (tmp_const == nullptr){
          const_check = false;
        }
        else{
          index_const = var_sizes[i + 1] * tmp_const->get_value() + index_const;
        }
      }
    }

    if (should_return_lvalue==false && const_check){
      ConstantInt *tmp_const = dynamic_cast<ConstantInt *>(const_array->get_element_value(index_const));
      tmp_val = CONST_INT(tmp_const->get_value());
    }
    else{
      for (int i = 0; i < all_index.size(); i++) {
        auto index_val = all_index[i];
        Value *one_index;
        if (var_sizes[i + 1] > 1){
          one_index = builder->create_imul(CONST_INT(var_sizes[i + 1]), index_val);
        }
        else{
          one_index = index_val;
        }
        if (var_index == nullptr) {
          var_index = one_index;
        }
        else {
          var_index = builder->create_iadd(var_index, one_index);
        }
      } // end for
      if (node.array_index.size() > 1 || 1) {
        Value * tmp_ptr;
        if (var->get_type()->get_pointer_element_type()->is_pointer_type()) {
          auto tmp_load = builder->create_load(var);
          tmp_ptr = builder->create_gep(tmp_load, {var_index});
        }
        else {
          tmp_ptr = builder->create_gep(var, {CONST_INT(0), var_index});
        }
        if (should_return_lvalue) {
          tmp_val = tmp_ptr;
          require_lvalue = false;
        }
        else {
          tmp_val = builder->create_load(tmp_ptr);
        }
      }
    }
  }
}

void MHSJBuilder::visit(SyntaxTree::Literal &node) {
  tmp_val = CONST_INT(node.int_const);
}

void MHSJBuilder::visit(SyntaxTree::ReturnStmt &node) {
  if (node.ret == nullptr) {
    builder->create_br(ret_BB);
  }
  else {
    node.ret->accept(*this);
    builder->create_store(tmp_val, ret_addr);
    builder->create_br(ret_BB);
  }
}

void MHSJBuilder::visit(SyntaxTree::BlockStmt &node) {
  bool need_exit_scope = !pre_enter_scope;
  if (pre_enter_scope) {
    pre_enter_scope = false;
  }
  else {
    scope.enter();
  }
  for (auto &decl : node.body) {
    decl->accept(*this);
    if (builder->get_insert_block()->get_terminator() != nullptr)
      break;
  }
  if (need_exit_scope) {
    scope.exit();
  }
}

void MHSJBuilder::visit(SyntaxTree::EmptyStmt &node) { tmp_val = nullptr; }

void MHSJBuilder::visit(SyntaxTree::ExprStmt &node) { node.exp->accept(*this); }

void MHSJBuilder::visit(SyntaxTree::UnaryCondExpr &node) {
  if (node.op == SyntaxTree::UnaryCondOp::NOT) {
    node.rhs->accept(*this);
    auto r_val = tmp_val;
    tmp_val = builder->create_icmp_eq(r_val, CONST_INT(0));
  }
}

void MHSJBuilder::visit(SyntaxTree::BinaryCondExpr &node) {
  CmpInst *cond_val;
  if (node.op == SyntaxTree::BinaryCondOp::LAND) {
    auto trueBB = BasicBlock::create(module.get(), "", cur_fun);
    IF_While_And_Cond_Stack.push_back({trueBB, IF_While_Or_Cond_Stack.back().falseBB});
    node.lhs->accept(*this);
    IF_While_And_Cond_Stack.pop_back();
    auto ret_val = tmp_val;
    cond_val = dynamic_cast<CmpInst *>(ret_val);
    if (cond_val == nullptr) {
      cond_val = builder->create_icmp_ne(tmp_val, CONST_INT(0));
    }
    builder->create_cond_br(cond_val, trueBB, IF_While_Or_Cond_Stack.back().falseBB);
    builder->set_insert_point(trueBB);
    node.rhs->accept(*this);
  }
  else if (node.op == SyntaxTree::BinaryCondOp::LOR) {
    auto falseBB = BasicBlock::create(module.get(), "", cur_fun);
    IF_While_Or_Cond_Stack.push_back({IF_While_Or_Cond_Stack.back().trueBB, falseBB});
    node.lhs->accept(*this);
    IF_While_Or_Cond_Stack.pop_back();
    auto ret_val = tmp_val;
    cond_val = dynamic_cast<CmpInst *>(ret_val);
    if (cond_val == nullptr) {
      cond_val = builder->create_icmp_ne(tmp_val, CONST_INT(0));
    }
    builder->create_cond_br(cond_val, IF_While_Or_Cond_Stack.back().trueBB, falseBB);
    builder->set_insert_point(falseBB);
    node.rhs->accept(*this);
  }
  else {
    node.lhs->accept(*this);
    auto l_val = tmp_val;
    node.rhs->accept(*this);
    auto r_val = tmp_val;
    Value *cmp;
    switch (node.op) {
    case SyntaxTree::BinaryCondOp::LT:
      cmp = builder->create_icmp_lt(l_val, r_val);
      break;
    case SyntaxTree::BinaryCondOp::LTE:
      cmp = builder->create_icmp_le(l_val, r_val);
      break;
    case SyntaxTree::BinaryCondOp::GTE:
      cmp = builder->create_icmp_ge(l_val, r_val);
      break;
    case SyntaxTree::BinaryCondOp::GT:
      cmp = builder->create_icmp_gt(l_val, r_val);
      break;
    case SyntaxTree::BinaryCondOp::EQ:
      cmp = builder->create_icmp_eq(l_val, r_val);
      break;
    case SyntaxTree::BinaryCondOp::NEQ:
      cmp = builder->create_icmp_ne(l_val, r_val);
      break;
    default: break;
    }
    tmp_val = cmp;
  }
}

void MHSJBuilder::visit(SyntaxTree::BinaryExpr &node) {
  if (node.rhs == nullptr) {
    node.lhs->accept(*this);
  }
  else {
    node.lhs->accept(*this);
    auto l_val_const = dynamic_cast<ConstantInt *>(tmp_val);
    auto l_val = tmp_val;
    node.rhs->accept(*this);
    auto r_val_const = dynamic_cast<ConstantInt *>(tmp_val);
    auto r_val = tmp_val;
    switch (node.op) {
    case SyntaxTree::BinOp::PLUS:
      if (r_val_const != nullptr && l_val_const != nullptr){
        tmp_val = CONST_INT(l_val_const->get_value() + r_val_const->get_value());
      }
      else{
        tmp_val = builder->create_iadd(l_val, r_val);
      }
      break;
    case SyntaxTree::BinOp::MINUS:
      if (r_val_const != nullptr && l_val_const != nullptr){
        tmp_val = CONST_INT(l_val_const->get_value() - r_val_const->get_value());
      }
      else{
        tmp_val = builder->create_isub(l_val, r_val);
      }
      break;
    case SyntaxTree::BinOp::MULTIPLY:
      if (r_val_const != nullptr && l_val_const != nullptr){
        tmp_val = CONST_INT(l_val_const->get_value() * r_val_const->get_value());
      }
      else{
        tmp_val = builder->create_imul(l_val, r_val);
      }
      break;
    case SyntaxTree::BinOp::DIVIDE:
      if (r_val_const != nullptr && l_val_const != nullptr){
        tmp_val = CONST_INT(l_val_const->get_value() / r_val_const->get_value());
      }
      else{
        tmp_val = builder->create_isdiv(l_val, r_val);
      }
      break;
    case SyntaxTree::BinOp::MODULO:
      if (r_val_const != nullptr && l_val_const != nullptr){
        tmp_val = CONST_INT(l_val_const->get_value() % r_val_const->get_value());
      }
      else{
        tmp_val = builder->create_isrem(l_val, r_val);
      }
    }
  }
}

void MHSJBuilder::visit(SyntaxTree::UnaryExpr &node) {
  node.rhs->accept(*this);
  if (node.op == SyntaxTree::UnaryOp::MINUS) {
    auto val_const = dynamic_cast<ConstantInt *>(tmp_val);
    auto r_val = tmp_val;
    if (val_const != nullptr){
      tmp_val = CONST_INT(0 - val_const->get_value());
    }
    else{
      tmp_val = builder->create_isub(CONST_INT(0), r_val);
    }
  }
}

void MHSJBuilder::visit(SyntaxTree::FuncCallStmt &node) {
  auto fun = static_cast<Function *>(scope.find_func(node.name));//FIXME:STATIC OR DYNAMIC?
  std::vector<Value *> params;
  int i = 0;
  if (node.name == "starttime" || node.name == "stoptime") {
    params.push_back(ConstantInt::get(node.loc.begin.line, module.get()));
  } else {
    for (auto &param : node.params) {
      if (fun->get_function_type()->get_param_type(i++)->is_integer_type()) {
        require_lvalue = false;
      }
      else {
        require_lvalue = true;
      }
      param->accept(*this);
      require_lvalue = false;
      params.push_back(tmp_val);
    }
  }
  tmp_val = builder->create_call(static_cast<Function *>(fun), params);
}

void MHSJBuilder::visit(SyntaxTree::IfStmt &node) {
  auto trueBB = BasicBlock::create(module.get(), "", cur_fun);
  auto falseBB = BasicBlock::create(module.get(), "", cur_fun);
  auto nextBB = BasicBlock::create(module.get(), "", cur_fun);
  IF_While_Or_Cond_Stack.push_back({nullptr, nullptr});
  IF_While_Or_Cond_Stack.back().trueBB = trueBB;
  if (node.else_statement == nullptr) {
    IF_While_Or_Cond_Stack.back().falseBB = nextBB;
  }
  else {
    IF_While_Or_Cond_Stack.back().falseBB = falseBB;
  }
  node.cond_exp->accept(*this);
  IF_While_Or_Cond_Stack.pop_back();
  auto ret_val = tmp_val;
  auto *cond_val = dynamic_cast<CmpInst *>(ret_val);
  if (cond_val == nullptr) {
    cond_val = builder->create_icmp_ne(tmp_val, CONST_INT(0));
  }
  if (node.else_statement == nullptr) {
    builder->create_cond_br(cond_val, trueBB, nextBB);
  }
  else {
    builder->create_cond_br(cond_val, trueBB, falseBB);
  }
  cur_basic_block_list.pop_back();
  builder->set_insert_point(trueBB);
  cur_basic_block_list.push_back(trueBB);
  if (dynamic_cast<SyntaxTree::BlockStmt *>(node.if_statement.get())) {
    node.if_statement->accept(*this);
  }
  else {
    scope.enter();
    node.if_statement->accept(*this);
    scope.exit();
  }

  if (builder->get_insert_block()->get_terminator() == nullptr) {
    builder->create_br(nextBB);
  }
  cur_basic_block_list.pop_back();

  if (node.else_statement == nullptr) {
    falseBB->erase_from_parent();
  }
  else {
    builder->set_insert_point(falseBB);
    cur_basic_block_list.push_back(falseBB);
    if (dynamic_cast<SyntaxTree::BlockStmt *>(node.else_statement.get())) {
      node.else_statement->accept(*this);
    }
    else {
      scope.enter();
      node.else_statement->accept(*this);
      scope.exit();
    }
    if (builder->get_insert_block()->get_terminator() == nullptr) {
      builder->create_br(nextBB);
    }
    cur_basic_block_list.pop_back();
  }

  builder->set_insert_point(nextBB);
  cur_basic_block_list.push_back(nextBB);
  if (nextBB->get_pre_basic_blocks().size()==0){
    builder->set_insert_point(trueBB);
    nextBB->erase_from_parent();
  }
}

void MHSJBuilder::visit(SyntaxTree::WhileStmt &node) {
  auto whileBB = BasicBlock::create(module.get(), "", cur_fun);
  auto trueBB = BasicBlock::create(module.get(), "", cur_fun);
  auto nextBB = BasicBlock::create(module.get(), "", cur_fun);
  While_Stack.push_back({whileBB, nextBB});
  if (builder->get_insert_block()->get_terminator() == nullptr) {
    builder->create_br(whileBB);
  }
  cur_basic_block_list.pop_back();
  builder->set_insert_point(whileBB);
  IF_While_Or_Cond_Stack.push_back({trueBB, nextBB});
  node.cond_exp->accept(*this);
  IF_While_Or_Cond_Stack.pop_back();
  auto ret_val = tmp_val;
  auto *cond_val = dynamic_cast<CmpInst *>(ret_val);
  if (cond_val == nullptr) {
    cond_val = builder->create_icmp_ne(tmp_val, CONST_INT(0));
  }
  builder->create_cond_br(cond_val, trueBB, nextBB);
  builder->set_insert_point(trueBB);
  cur_basic_block_list.push_back(trueBB);
  if (dynamic_cast<SyntaxTree::BlockStmt *>(node.statement.get())) {
    node.statement->accept(*this);
  }
  else {
    scope.enter();
    node.statement->accept(*this);
    scope.exit();
  }
  if (builder->get_insert_block()->get_terminator() == nullptr) {
    builder->create_br(whileBB);
  }
  cur_basic_block_list.pop_back();
  builder->set_insert_point(nextBB);
  cur_basic_block_list.push_back(nextBB);
  While_Stack.pop_back();
}

void MHSJBuilder::visit(SyntaxTree::BreakStmt &node) {
  builder->create_br(While_Stack.back().falseBB);
}

void MHSJBuilder::visit(SyntaxTree::ContinueStmt &node) {
  builder->create_br(While_Stack.back().trueBB);
}
