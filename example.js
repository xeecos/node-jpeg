const Module = require('./demo.js');
Module.onRuntimeInitialized = () => {
    let in_ptr = Module._malloc(4); //初始化输入数据
    let out_ptr = Module._malloc(4);//初始化输出数据
    for (var i = 0; i < 4; i++) {
        Module.HEAPU8[in_ptr + i] = i; //给输入数据赋值
    }
    let res = Module.process_data(in_ptr, 4, out_ptr);
    for (let i = 0; i < 4; i++) {
        console.log(Module.HEAPU8[out_ptr + i]); //打印输出数据
    }
    console.log(res);
    Module._free(in_ptr);//释放输入数据
    Module._free(out_ptr);//释放输入数据
}