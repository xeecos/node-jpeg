const binding = require('./build/Release/demo.node');
/*
    初始化数据
*/
const inBuf = Buffer.alloc(4);
const outBuf = Buffer.alloc(4);
for(let i=0;i<4;i++)
{
    inBuf[i] = i;
} 
// 处理数据
let result = binding.process_data(inBuf, inBuf.length, outBuf);

/*
    打印数据
*/
for(let i=0;i<4;i++)
{
    console.log(outBuf[i]);
} 
console.log("result:", result);