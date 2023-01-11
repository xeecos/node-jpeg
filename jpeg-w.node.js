const fs = require('fs');
const Module = require('./jpeg-w.js');
Module.onRuntimeInitialized = () => {

    let width = 1920;
    let height = 1080;
    let isRGB = false;
    let in_ptr = Module._malloc(width * height * (isRGB ? 3 : 1));
    let out_ptr = Module._malloc(4 * 1024 * 1024);
    for (var i = 0, len = width * height; i < len; i++) {
        Module.HEAPU8[in_ptr + i * (isRGB ? 3 : 1)] = 0xff * Math.random();
        if (isRGB) {
            Module.HEAPU8[in_ptr + i * (isRGB ? 3 : 1) + 1] = 0;
            Module.HEAPU8[in_ptr + i * (isRGB ? 3 : 1) + 2] = 0;
        }
    }
    let out_size = Module.encode(in_ptr, width, height, out_ptr, 4 * 1024 * 1024, isRGB, 80);
    let out_buf = Buffer.alloc(out_size);
    for (var i = 0; i < out_size; i++) {
        out_buf[i] = Module.HEAPU8[out_ptr + i];
    }
    let fd = fs.openSync("tmp.jpg", "w");
    fs.writeSync(fd, out_buf, 0, out_size);
    fs.closeSync(fd);

    let decode_ptr = Module._malloc(4 * 1024 * 1024);
    let res = Module.decode(out_ptr, out_size, decode_ptr, isRGB);
    console.log(res);

    let demosaic_ptr = Module._malloc(width * height * 3);
    console.time("time");
    Module.demosaic(in_ptr, demosaic_ptr, 1, width, height);
    console.timeEnd("time");

    Module._free(in_ptr);
    Module._free(out_ptr);
    Module._free(decode_ptr);
    Module._free(demosaic_ptr);
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
