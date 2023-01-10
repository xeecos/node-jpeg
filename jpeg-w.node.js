const fs = require('fs');
const Module = require('./jpeg-w.js');
Module.onRuntimeInitialized = ()=>{
    let in_ptr = Module._malloc(4);
    let out_ptr = Module._malloc(4);
    for (var i = 0; i < 4; i++) {
        Module.HEAPU8[in_ptr + i] = 0xa4+i;
        Module.HEAPU8[out_ptr + i] = 0xa4-i;
    }
    Module.encode_jpeg(in_ptr,4,out_ptr,4);
    for (var i = 0; i < 4; i++) {
        console.log(Module.HEAPU8[in_ptr + i]);
        console.log(Module.HEAPU8[out_ptr + i]);
    }
    Module._free(in_ptr);
    Module._free(out_ptr);
};
// WebAssembly.instantiate(new Uint8Array(buf),importObject).then((obj) => {
//     console.log(obj)
//     const Module = obj;//snew WebAssembly.Instance(obj,{});
//     // console.log(Module.ccall)
//     // var in_ptr = Module._malloc(4);
//     // var out_ptr = Module._malloc(4);
//     // Module._encode_jpeg(in_ptr, 4, out_ptr, 4);
//     // console.log(out_ptr);
// });
