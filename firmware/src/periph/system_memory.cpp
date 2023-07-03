#include "periph/system_memory.hpp"
#include "fx_model.hpp"
#include "env.hpp"
#include "app_params.hpp"
#include <igb_util/null_functor.hpp>

SystemMemory system_memory;

void SystemMemory::restore() {
  readModelIdx(fx_model, [](){
    system_memory.readParams(app_params, [](){});
    fx_model.init(false);
  });
}

void SystemMemory::reset() {
  env.setCurrentVers();
  writeEnv(env, [](){
    fx_model.setModelByIdx(0);
    system_memory.writeModelIdx(fx_model, [](){});
    fx_model.init(true);
  });
}

void SystemMemory::checkVersionAndRestore() {
  busyWait();
  readEnv(env, [](){
    if (env.isCompatible()) {
      system_memory.restore();
      system_memory.state = SystemMemory::State::restore;
    } else {
      system_memory.reset();
      system_memory.state = SystemMemory::State::reset;
    }
  });
  busyWait();
}
