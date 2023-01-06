
const binding = require('./build/Release/jpeg');
const fs = require("fs")
const rgb = [];
const width = 1920;
const height = 1080;
const quality = 75;
for(let i=0;i<width;i++)
{
    for(let j=0;j<height;j++)
    {
        rgb.push(Math.floor(0xff*Math.random()));
        rgb.push(Math.floor(0xff*Math.random()));
        rgb.push(Math.floor(0xff*Math.random()));
    }
}
const buffer = Buffer.from(rgb);    //Input RGB Data
const out = Buffer.alloc(4*1024*1024); //Max File Size = 4MB
console.time("encode");
console.log("start");
let size = binding.encode(buffer, buffer.length, out, out.length, width, height,quality);
console.timeEnd("encode");
let fd = fs.openSync("./tmp.jpg","w");
fs.writeSync(fd,out,0,size);
fs.closeSync(fd);
console.log('binding.encode() =', out[0],out[1],size);