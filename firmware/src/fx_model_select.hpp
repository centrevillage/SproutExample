#pragma once

#include "fx_model/fx_model_delay.hpp"
#include "fx_model/fx_model_reverb.hpp"
#include "fx_model/fx_model_filter.hpp"
// TODO: <- add your fx_mpdel headers

// TODO: add your model class on this list
#define SPR_FX_MODEL_SELECT SPR_FX_MODEL_LIST(FxModelDelay, FxModelReverb, FxModelFilter)
