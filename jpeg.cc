#include <node.h>

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

void Method(const FunctionCallbackInfo<Value> &args)
{
  Isolate *isolate = args.GetIsolate();
  args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "world").ToLocalChecked());
}

void init(Local<Object> exports)
{
  NODE_SET_METHOD(exports, "hello", Method);
}
NODE_MODULE(addon, init)