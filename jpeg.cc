#include <nan.h>
using namespace Nan;  
using namespace v8;

NAN_METHOD(encode) {  
    
    v8::Isolate *isolate = Isolate::GetCurrent();
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    uint8_t* rgbBuffer = (uint8_t*) node::Buffer::Data(info[0]->ToObject(context).ToLocalChecked());
    unsigned int rgbBufferSize = info[1]->Uint32Value(context).ToChecked();

    unsigned int width = info[4]->Uint32Value(context).ToChecked();
    unsigned int height = info[5]->Uint32Value(context).ToChecked();

    uint8_t* outBuffer = (uint8_t*) node::Buffer::Data(info[2]->ToObject(context).ToLocalChecked());
    unsigned int outBufferSize = info[3]->Uint32Value(context).ToChecked();

    for(unsigned int i = 0; i < rgbBufferSize; i++ ) {
        outBuffer[i] = 5;
    }   
    info.GetReturnValue().Set(outBufferSize);
}

NAN_MODULE_INIT(Init) {  
   Nan::Set(target, New<String>("encode").ToLocalChecked(),  GetFunction(New<FunctionTemplate>(encode)).ToLocalChecked());
}

NODE_MODULE(addon, Init)