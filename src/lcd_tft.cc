#include <nan.h>
#include "lcd.h"

using namespace v8;

NAN_METHOD(CreateObject) {
  Nan::HandleScope scope;
  info.GetReturnValue().Set(EpdLcd::NewInstance(info[0], info[1], info[2], info[3], info[4]));
}

void InitAll(Handle<Object> exports, Handle<Object> module) {
  Nan::HandleScope scope;

  EpdLcd::Init();

  module->Set(Nan::New("exports").ToLocalChecked(),
      Nan::New<FunctionTemplate>(CreateObject)->GetFunction());
}

NODE_MODULE(epd, InitAll)
