
const binding = require('./build/Release/jpeg');
const fs = require("fs");
console.log(fs.readFileSync("test.jpeg"))
const rgb = [];
const width = 1920;
const height = 1080;
const quality = 75;
const isRGB = true;
for(let i=0;i<width;i++)
{
    for(let j=0;j<height;j++)
    {
        rgb.push(j%0xff);
        if(isRGB)
        {
            // rgb.push(Math.floor(Math.random()*0x20));
            rgb.push(i%0xff);
            rgb.push(j%0xff);
        }
        // rgb.push(Math.floor(0xff*Math.random()));
        // rgb.push(Math.floor(0xff*Math.random()));
    }
}
const buffer = Buffer.from(rgb);    //Input RGB Data
const out = Buffer.alloc(4*1024*1024); //Max File Size = 4MB
console.time("encode");
console.log("start");
let size = binding.encode(buffer, buffer.length, out, out.length, width, height, isRGB, quality);
console.timeEnd("encode");
let fd = fs.openSync("./tmp.jpg","w");
fs.writeSync(fd,out,0,size);
fs.closeSync(fd);
console.log('binding.encode() =', out[0].toString(16),out[1].toString(16),out[2].toString(16),out[3].toString(16),out[4].toString(16),out[5].toString(16));
const rgbBuffer = Buffer.alloc(width*height*(isRGB?3:1));
console.time("decode");
binding.decode(out,size,rgbBuffer,isRGB);
console.timeEnd("decode");
console.log(rgbBuffer,rgb)