//
// Created by cjb on 7/20/21.
//

#ifndef MHSJ_PASS_H
#define MHSJ_PASS_H

#include <string>
#include <list>
#include "Module.h"



class Pass{
public:
    explicit Pass(Module* m){module = m;}
    virtual void execute() = 0;
    virtual const std::string get_name() const = 0;
    //std::string name;
protected:
    Module* module;
};

template<typename T>
using PassList = std::list<T*>;

class PassMgr{
public:
    explicit PassMgr(Module* m){module = m;pass_list = PassList<Pass>();}
    void register_pass(){
        //TODO:Finish Pass Register
    };
    template <typename PassTy> void addPass(bool print_ir = true) {
        //std::cout << "add ";
        pass_list.push_back(new PassTy(module));
    }

  void execute(bool print_ir = true) {
    //auto i = 0;
    //std::cout << "inexec\n";
    for (auto pass : pass_list) {
      //i++;
      //std::cout << pass->get_name() << "\n";
      pass->execute();
      //std::cout << i + 1 << "\n";
      if (print_ir) {
        module->print();
      }
    }
  }
private:
    Module* module;
    PassList<Pass> pass_list;
};

#endif //MHSJ_PASS_H
