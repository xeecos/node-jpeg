#include <nan.h>
#include "toojpeg.h"
using namespace Nan;  
using namespace v8;
uint8_t* outBuffer;
int idx;
void myOutput(unsigned char oneByte) { 
    outBuffer[idx] = oneByte;
    idx++;
}
NAN_METHOD(encode) {  
    
    v8::Isolate *isolate = Isolate::GetCurrent();
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    uint8_t* rgbBuffer = (uint8_t*) node::Buffer::Data(info[0]->ToObject(context).ToLocalChecked());
    unsigned int rgbBufferSize = info[1]->Uint32Value(context).ToChecked();

    unsigned int width = info[4]->Uint32Value(context).ToChecked();
    unsigned int height = info[5]->Uint32Value(context).ToChecked();

    outBuffer = (uint8_t*) node::Buffer::Data(info[2]->ToObject(context).ToLocalChecked());
    unsigned int outBufferSize = info[3]->Uint32Value(context).ToChecked();
    unsigned int quality = info[6]->Uint32Value(context).ToChecked();
    idx = 0;
    TooJpeg::writeJpeg(myOutput,rgbBuffer,width,height,true,quality);
    info.GetReturnValue().Set(idx);
}

NAN_MODULE_INIT(Init) {  
   Nan::Set(target, New<String>("encode").ToLocalChecked(),  GetFunction(New<FunctionTemplate>(encode)).ToLocalChecked());
}

NODE_MODULE(addon, Init)